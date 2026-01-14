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

#include <fmt/core.h>

#include <chrono>

#include <vda5050_core/logger/logger.hpp>
#include <vda5050_json_utils/serialization.hpp>
#include <vda5050_types/header.hpp>

#include "vda5050_execution/base_execution_context.hpp"

constexpr int CONNECTION_QOS = 1;
constexpr int DEFAULT_QOS = 0;

namespace vda5050_execution {

//=============================================================================
BaseExecutionContext::~BaseExecutionContext()
{
  shutdown();
}

//=============================================================================
std::shared_ptr<BaseExecutionContext> BaseExecutionContext::make(
  const ClientConfig& config)
{
  auto context =
    std::shared_ptr<BaseExecutionContext>(new BaseExecutionContext(config));
  return context;
}

//=============================================================================
Segment BaseExecutionContext::get_next_segment() {}

//=============================================================================
void BaseExecutionContext::acknowledge_sequence_reached(
  const std::uint32_t seq_id)
{
}

//=============================================================================
std::vector<std::shared_ptr<const vda5050_types::Action>>
BaseExecutionContext::get_pending_actions()
{
}

//=============================================================================
std::vector<std::shared_ptr<const vda5050_types::Action>>
BaseExecutionContext::get_pending_instant_actions()
{
}

//=============================================================================
void BaseExecutionContext::update_action_status(
  const std::string& action_id, vda5050_types::ActionStatus status)
{
}

//=============================================================================
void BaseExecutionContext::update_position(
  const vda5050_types::AGVPosition& position)
{
}

//=============================================================================
void BaseExecutionContext::update_battery_state(
  const vda5050_types::BatteryState& battery)
{
}

//=============================================================================
void BaseExecutionContext::update_operating_mode(
  vda5050_types::OperatingMode mode)
{
}

//=============================================================================
void BaseExecutionContext::add_error(const vda5050_types::Error& error) {}

//=============================================================================
void BaseExecutionContext::resolve_error(const std::string& error_type) {}

//=============================================================================
void BaseExecutionContext::clear_errors() {}

//=============================================================================
void BaseExecutionContext::request_state_publish()
{
  request_state_publish_ = true;
  state_cv_.notify_one();
}

void BaseExecutionContext::shutdown()
{
  shutdown_ = true;
  if (state_update_thread_.joinable()) state_update_thread_.join();

  mqtt_client_->disconnect();
}

//=============================================================================
BaseExecutionContext::BaseExecutionContext(const ClientConfig& config)
: config_(config),
  mqtt_client_(vda5050_core::mqtt_client::create_default_client(
    config.mqtt_broker_address, config.serial_number)),
  request_state_publish_(true),
  shutdown_(false)
{
  vda5050_types::Connection connection_will;
  connection_will.header.header_id = 0;
  connection_will.header.version = config_.version;
  connection_will.header.manufacturer = config_.manufacturer;
  connection_will.header.serial_number = config_.serial_number;
  connection_will.header.timestamp = std::chrono::system_clock::now();
  connection_will.connection_state =
    vda5050_types::ConnectionState::CONNECTIONBROKEN;

  nlohmann::json j = connection_will;

  mqtt_client_->set_will(
    fmt::format(
      "{}/{}/{}/{}/connection", config_.interface, config_.version,
      config_.manufacturer, config_.serial_number),
    j.dump(), CONNECTION_QOS);

  // mqtt_client_->connect();

  protocol_adapter_ = ProtocolAdapter::make(
    mqtt_client_, config_.interface, config_.version, config_.manufacturer,
    config_.serial_number);

  protocol_adapter_->subscribe<vda5050_types::Order>(
    [w = weak_from_this()](
      vda5050_types::Order order, std::optional<vda5050_types::Error> error) {
      if (auto c = w.lock())
      {
        if (error.has_value())
        {
          // process error
        }
        else
        {
          std::lock_guard<std::mutex> lock(c->order_mutex_);
          c->current_order_ = std::make_shared<vda5050_types::Order>(order);
        }
      }
    },
    DEFAULT_QOS);

  state_update_thread_ = std::thread([w = weak_from_this()] {
    while (true)
    {
      auto c = w.lock();
      if (!c || c->shutdown_) break;

      std::unique_lock<std::mutex> lock(c->state_mutex_);
      c->state_cv_.wait_for(lock, c->config_.state_publish_period, [&c] {
        return c->shutdown_ || c->request_state_publish_;
      });

      if (c->shutdown_) break;

      lock.unlock();

      // c->current_state_->agv_position =
      //   *c->provider().query<PositionData>()->agv_position;
      // c->current_state_->battery_state =
      //   *c->provider().query<BatteryData>()->battery_state;
      c->protocol_adapter_->publish<vda5050_types::State>(
        *c->current_state_, DEFAULT_QOS);

      c->request_state_publish_ = false;
    }
  });

  vda5050_types::Connection connection;
  connection.connection_state = vda5050_types::ConnectionState::ONLINE;
  protocol_adapter_->publish<vda5050_types::Connection>(
    connection, DEFAULT_QOS);
}

}  // namespace vda5050_execution
