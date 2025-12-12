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

#include <fmt/core.h>

#include "vda5050_bt_execution/bt_execution/execution_engine.hpp"
#include "vda5050_bt_execution/bt_execution/robot_adapter_interface.hpp"
#include "vda5050_bt_execution/bt_nodes/execute_order.hpp"
#include "vda5050_bt_execution/bt_nodes/monitor_connection.hpp"
#include "vda5050_bt_execution/bt_nodes/update_order.hpp"
#include "vda5050_bt_execution/bt_nodes/update_state.hpp"

#include <vda5050_core/logger/logger.hpp>
#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>

#include <vda5050_json_utils/serialization.hpp>
#include <vda5050_types/connection.hpp>
#include <vda5050_types/header.hpp>

namespace vda5050_bt_execution {

//=============================================================================
std::shared_ptr<ExecutionEngine> ExecutionEngine::make(
  const ClientConfig& config,
  std::shared_ptr<RobotAdapterInterface> robot_adapter)
{
  auto execution_engine = std::shared_ptr<ExecutionEngine>(
    new ExecutionEngine(config, robot_adapter));
  return execution_engine;
}

//=============================================================================
std::shared_ptr<ExecutionEngine> ExecutionEngine::make_and_init(
  const ClientConfig& config,
  std::shared_ptr<RobotAdapterInterface> robot_adapter)
{
  auto execution_engine = std::shared_ptr<ExecutionEngine>(
    new ExecutionEngine(config, robot_adapter));
  execution_engine->initialize();
  return execution_engine;
}

//=============================================================================
void ExecutionEngine::initialize()
{
  BT::BehaviorTreeFactory factory;

  factory.registerNodeType<MonitorConnection>("MonitorConnection");
  factory.registerNodeType<UpdateState>("UpdateState");
  factory.registerNodeType<UpdateOrder>("UpdateOrder");
  factory.registerNodeType<ExecuteOrder>("ExecuteOrder");

  // TODO(sauk): Try to put MonitorConnection in a ReactiveSequence with all
  // other nodes inside a Parallel
  // clang-format off
  const char* xml_text = R"(
  <root BTCPP_format="4">
    <BehaviorTree ID="MainTree">
      <Parallel>
        <MonitorConnection context="{context}"/>
        <UpdateState context="{context}"/>
        <UpdateOrder context="{context}"/>
        <ExecuteOrder context="{context}"/>
      </Parallel>
    </BehaviorTree>
  </root>
  )";
  // clang-format on

  tree_ = factory.createTreeFromText(xml_text);
  tree_.rootBlackboard()->set("context", context_);

  running_ = true;
}

//=============================================================================
void ExecutionEngine::spin()
{
  tree_.tickWhileRunning();
}

//=============================================================================
void ExecutionEngine::spin_once()
{
  tree_.tickOnce();
}

//=============================================================================
void ExecutionEngine::sleep(std::chrono::milliseconds period)
{
  tree_.sleep(period);
}

//=============================================================================
void ExecutionEngine::shutdown()
{
  tree_.haltTree();

  context_->mqtt_client->disconnect();
}

//=============================================================================
ExecutionEngine::ExecutionEngine(
  const ClientConfig& config,
  std::shared_ptr<RobotAdapterInterface> robot_adapter)
: context_(std::make_shared<ExecutionContext>()), running_(false)
{
  context_->robot_adapter = robot_adapter;

  context_->mqtt_client = vda5050_core::mqtt_client::create_default_client(
    config.mqtt_broker_address, config.serial_number);

  context_->client_config =
    std::make_shared<const ClientConfig>(std::move(config));
}

}  // namespace vda5050_bt_execution
