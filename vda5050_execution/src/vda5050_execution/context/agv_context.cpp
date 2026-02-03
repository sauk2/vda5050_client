/*
 * Copyright (C) 2026 ROS-Industrial Consortium Asia Pacific
 * Advanced Remanufacturing and Technology Centre
 * A*STAR Research Entities (Co. Registration No. 199702110H)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chrono>

#include <vda5050_types/connection.hpp>
#include <vda5050_types/connection_state.hpp>
#include <vda5050_types/order.hpp>

#include "vda5050_execution/context/agv_context.hpp"

#include "vda5050_execution/resources/order_resources.hpp"
#include "vda5050_execution/resources/transport_resources.hpp"

#include "vda5050_execution/updates/navigation_updates.hpp"
#include "vda5050_execution/updates/state_updates.hpp"

constexpr int CONNECTION_QOS = 1;
constexpr int DEFAULT_QOS = 0;

namespace vda5050_execution {

namespace context {

//=============================================================================
std::shared_ptr<AGVContext> AGVContext::make(
  std::shared_ptr<resources::Config> config)
{
  auto context = std::shared_ptr<AGVContext>(new AGVContext(std::move(config)));
  return context;
}

//=============================================================================
void AGVContext::init()
{
  auto provider = this->provider();

  auto order_manager = std::make_shared<resources::OrderManager>();
  this->add_resource(order_manager);

  auto config = this->get_resource<resources::Config>();
  auto agv_config = config->agv_identity();

  auto mqtt_client = vda5050_core::mqtt_client::create_default_client(
    config->mqtt().broker_address, config->agv_identity().serial_number);

  vda5050_types::Connection connection_will;
  connection_will.header.header_id = 0;
  connection_will.header.version = agv_config.protocol_version;
  connection_will.header.manufacturer = agv_config.manufacturer;
  connection_will.header.serial_number = agv_config.serial_number;
  connection_will.header.timestamp = std::chrono::system_clock::now();
  connection_will.connection_state =
    vda5050_types::ConnectionState::CONNECTIONBROKEN;

  nlohmann::json j = connection_will;

  mqtt_client->set_will(
    fmt::format(
      "{}/{}/{}/{}/connection", agv_config.interface_name,
      agv_config.protocol_version, agv_config.manufacturer,
      agv_config.serial_number),
    j.dump(), CONNECTION_QOS);

  mqtt_client->connect();

  auto protocol_adapter = utils::ProtocolAdapter::make(
    mqtt_client, config->agv_identity().interface_name,
    config->agv_identity().protocol_version,
    config->agv_identity().manufacturer, config->agv_identity().serial_number);

  protocol_adapter->subscribe<vda5050_types::Order>(
    [w = weak_from_this(), order_manager](
      vda5050_types::Order order, std::optional<vda5050_types::Error> error) {
      if (!error.has_value())
      {
        order_manager->update_order(order);
      }
      else
      {
        auto c = w.lock();
        if (!c) return;
        auto provider = c->provider();

        auto error_update = std::make_shared<updates::Errors>();
        error_update->errors.push_back(error.value());

        provider->push_shared(error_update);
      }
    },
    DEFAULT_QOS);

  auto transport_handler = std::make_shared<resources::TransportHandler>(
    mqtt_client, protocol_adapter);
  this->add_resource(transport_handler);

  provider->on<updates::SequenceAcknowledgement>(
    [w = weak_from_this()](auto update) {
      if (auto c = w.lock())
      {
        auto order_manager = c->get_resource<resources::OrderManager>();
        if (order_manager)
          order_manager->acknowledge_sequence(update->sequence_id);
      }
    });

  provider->on<updates::PositionData>([w = weak_from_this()](auto update) {
    if (auto c = w.lock())
    {
      c->add_update(update);
    }
  });

  provider->on<updates::BatteryData>([w = weak_from_this()](auto update) {
    if (auto c = w.lock())
    {
      c->add_update(update);
    }
  });

  provider->on<updates::Errors>([w = weak_from_this()](auto update) {
    if (auto c = w.lock())
    {
      auto error_update = c->get_update<updates::Errors>();
      if (!error_update)
      {
        c->add_update(update);
      }
      else
      {
        error_update->errors.insert(
          error_update->errors.end(), update->errors.begin(),
          update->errors.end());

        c->add_update(error_update);
      }
    }
  });
}

//=============================================================================
AGVContext::AGVContext(std::shared_ptr<resources::Config> config)
{
  this->add_resource(config);
}

}  // namespace context
}  // namespace vda5050_execution
