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

#include "vda5050_bt_execution/bt_nodes/execute_order.hpp"
#include "vda5050_bt_execution/bt_execution/execution_context.hpp"

namespace vda5050_bt_execution {

//=============================================================================
ExecuteOrder::ExecuteOrder(
  const std::string& name, const BT::NodeConfig& config)
: BT::StatefulActionNode(name, config)
{
  // Nothing to do here
}

//=============================================================================
BT::PortsList ExecuteOrder::providedPorts()
{
  return {BT::InputPort<std::shared_ptr<ExecutionContext>>("context")};
}

//=============================================================================
BT::NodeStatus ExecuteOrder::onStart()
{
  return BT::NodeStatus::RUNNING;
}

//=============================================================================
BT::NodeStatus ExecuteOrder::onRunning()
{
  auto context = getInput<std::shared_ptr<ExecutionContext>>("context").value();

  if (!context) return BT::NodeStatus::RUNNING;

  if (
    context->current_order && !context->robot_adapter->moving() &&
    context->current_node_idx < context->current_order->nodes.size())
  {
    context->robot_adapter->move(
      context->current_order->nodes[context->current_node_idx]);
  }

  return BT::NodeStatus::RUNNING;
}

//=============================================================================
void ExecuteOrder::onHalted() {}

}  // namespace vda5050_bt_execution
