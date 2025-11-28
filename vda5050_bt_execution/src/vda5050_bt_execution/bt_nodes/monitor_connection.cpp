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

#include "vda5050_bt_execution/bt_nodes/monitor_connection.hpp"
#include "vda5050_bt_execution/bt_execution/execution_context.hpp"

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
      vda5050_types::Connection connection;
      connection.connection_state = vda5050_types::ConnectionState::ONLINE;
      nlohmann::json j = connection;

      context->mqtt_client->publish("vda5050/connection", j.dump(), 1);
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
    vda5050_types::Connection connection;
    connection.connection_state = vda5050_types::ConnectionState::OFFLINE;
    nlohmann::json j = connection;

    context->mqtt_client->publish("vda5050/connection", j.dump(), 1);
  }
}

}  // namespace vda5050_bt_execution
