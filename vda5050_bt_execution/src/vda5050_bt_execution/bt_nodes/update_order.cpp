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

#include <vda5050_core/logger/logger.hpp>

#include "vda5050_bt_execution/bt_execution/execution_context.hpp"
#include "vda5050_bt_execution/bt_nodes/update_order.hpp"

namespace vda5050_bt_execution {

//============================================================================
UpdateOrder::UpdateOrder(const std::string& name, const BT::NodeConfig& config)
: BT::StatefulActionNode(name, config)
{
  // Nothing to do here ...
}

//============================================================================
BT::PortsList UpdateOrder::providedPorts()
{
  return {BT::InputPort<std::shared_ptr<ExecutionContext>>("context")};
}

//============================================================================
BT::NodeStatus UpdateOrder::onStart()
{
  auto context = getInput<std::shared_ptr<ExecutionContext>>("context").value();

  if (context && context->mqtt_client)
  {
    topic_ = fmt::format(
      "{}/{}/{}/{}/order", context->client_config->interface,
      context->client_config->version, context->client_config->manufacturer,
      context->client_config->serial_number);

    context->mqtt_client->subscribe(
      topic_,
      [w = std::weak_ptr<ExecutionContext>(context)](
        const std::string& /*topic*/, const std::string& message) {
        if (auto m = w.lock())
        {
          std::lock_guard<std::mutex> lock(m->order_mutex);
          auto j = nlohmann::json::parse(message);
          vda5050_types::Order order = j;

          m->incoming_order_queue.push(
            std::make_shared<vda5050_types::Order>(order));

          VDA5050_INFO("Received order: {}", order.order_id);
        }
      },
      0);
  }
  return BT::NodeStatus::RUNNING;
}

//============================================================================
BT::NodeStatus UpdateOrder::onRunning()
{
  auto context = getInput<std::shared_ptr<ExecutionContext>>("context").value();

  if (!context) return BT::NodeStatus::RUNNING;

  std::lock_guard<std::mutex> lock(context->order_mutex);
  if (!context->incoming_order_queue.empty())
  {
    context->current_order = context->incoming_order_queue.front();
    context->incoming_order_queue.pop();
    context->next_node = context->current_order->nodes.begin();

    VDA5050_INFO("Processed order: {}", context->current_order->order_id);
  }

  return BT::NodeStatus::RUNNING;
}

//============================================================================
void UpdateOrder::onHalted()
{
  // Nothing to do here ...
}

}  // namespace vda5050_bt_execution
