/*
 * Copyright (C) 2025 ROS-Industrial Consortium Asia Pacific
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

#include "vda5050_bt_execution/bt_execution/execution_context.hpp"
#include "vda5050_bt_execution/bt_nodes/monitor_connection.hpp"

#include <vda5050_core/logger/logger.hpp>
#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>

#include <vda5050_json_utils/serialization.hpp>
#include <vda5050_types/connection.hpp>
#include <vda5050_types/header.hpp>

namespace vda5050_bt_execution {

//=============================================================================
MonitorConnection::MonitorConnection(
  const std::string& name, const BT::NodeConfig& config)
: StatefulActionNode(name, config), last_connected_(false)
{
  // Nothing to do here ...
}

//=============================================================================
BT::PortsList MonitorConnection::providedPorts()
{
  return {BT::InputPort<std::shared_ptr<ExecutionContext>>("context")};
}

//=============================================================================
BT::NodeStatus MonitorConnection::onStart()
{
  auto context = getInput<std::shared_ptr<ExecutionContext>>("context").value();

  if (context)
  {
    topic_ = fmt::format(
      "{}/{}/{}/{}/connection", context->client_config->interface,
      context->client_config->version, context->client_config->manufacturer,
      context->client_config->serial_number);

    vda5050_types::Header header;
    header.timestamp = std::chrono::system_clock::now();
    header.version = context->client_config->version;
    header.manufacturer = context->client_config->manufacturer;
    header.serial_number = context->client_config->serial_number;
    current_header_ = std::make_shared<vda5050_types::Header>(header);

    if (context->mqtt_client)
    {
      vda5050_types::Connection connection_will;
      connection_will.header = *current_header_;
      connection_will.connection_state =
        vda5050_types::ConnectionState::CONNECTIONBROKEN;
      nlohmann::json j = connection_will;
      context->mqtt_client->set_will(topic_, j.dump(), 1);
    }
  }

  return BT::NodeStatus::RUNNING;
}

//=============================================================================
BT::NodeStatus MonitorConnection::onRunning()
{
  auto context = getInput<std::shared_ptr<ExecutionContext>>("context").value();

  if (context && context->mqtt_client)
  {
    if (!context->mqtt_client->connected())
    {
      last_connected_ = false;
    }

    if (!last_connected_)
    {
      current_header_->header_id++;
      current_header_->timestamp = std::chrono::system_clock::now();

      vda5050_types::Connection connection;
      connection.header = *current_header_;
      connection.connection_state = vda5050_types::ConnectionState::ONLINE;
      nlohmann::json j = connection;

      context->mqtt_client->publish(topic_, j.dump(), 1, true);
      last_connected_ = true;
    }
  }
  return BT::NodeStatus::RUNNING;
}

//=============================================================================
void MonitorConnection::onHalted()
{
  auto context = getInput<std::shared_ptr<ExecutionContext>>("context").value();

  if (context && context->mqtt_client)
  {
    current_header_->header_id++;
    current_header_->timestamp = std::chrono::system_clock::now();

    vda5050_types::Connection connection;
    connection.header = *current_header_;
    connection.connection_state = vda5050_types::ConnectionState::OFFLINE;
    nlohmann::json j = connection;

    context->mqtt_client->publish(topic_, j.dump(), 1, true);
  }
}

}  // namespace vda5050_bt_execution
