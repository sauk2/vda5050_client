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

#include <vda5050_core/logger/logger.hpp>

#include "vda5050_bt_execution/bt_execution/execution_context.hpp"
#include "vda5050_bt_execution/bt_nodes/execute_order.hpp"

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

  std::lock_guard<std::mutex> lock(context->order_mutex);
  if (context->current_order && !context->robot_adapter->is_moving())
  {
    if (context->next_node == context->current_order->nodes.end())
    {
      VDA5050_INFO("Order completed");
      context->current_order.reset();
    }
    else
    {
      context->robot_adapter->move_to_node(
        *context->next_node, [w = std::weak_ptr<ExecutionContext>(context)]() {
          if (auto m = w.lock())
          {
            std::lock_guard<std::mutex> lock(m->order_mutex);
            m->next_node++;
          }
        });
    }
  }

  return BT::NodeStatus::RUNNING;
}

//=============================================================================
void ExecuteOrder::onHalted()
{
  // Nothing to do here ...
}

}  // namespace vda5050_bt_execution
