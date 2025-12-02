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

#include "vda5050_bt_execution/bt_nodes/update_order.hpp"
#include "vda5050_bt_execution/bt_execution/execution_context.hpp"

namespace vda5050_bt_execution {

//============================================================================
UpdateOrder::UpdateOrder(const std::string& name, const BT::NodeConfig& config)
: BT::SyncActionNode(name, config)
{
  // Nothing to do here ...
}

//============================================================================
BT::PortsList UpdateOrder::providedPorts()
{
  return {BT::InputPort<std::shared_ptr<ExecutionContext>>("context")};
}

//============================================================================
BT::NodeStatus UpdateOrder::tick()
{
  auto context = getInput<std::shared_ptr<ExecutionContext>>("context").value();

  if (context)
  {
    std::optional<std::shared_ptr<vda5050_types::Order>> incoming_order;
    {
      std::lock_guard<std::mutex> lock(context->order_mutex);
      if (!context->incoming_order_queue.empty())
      {
        incoming_order = context->incoming_order_queue.front();
        context->incoming_order_queue.pop();
      }
    }

    if (!incoming_order.has_value()) return BT::NodeStatus::SUCCESS;
  }

  return BT::NodeStatus::SUCCESS;
}

}  // namespace vda5050_bt_execution
