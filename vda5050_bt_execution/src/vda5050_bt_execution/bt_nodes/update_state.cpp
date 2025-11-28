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

#include "vda5050_bt_execution/bt_nodes/update_state.hpp"
#include "vda5050_bt_execution/bt_execution/execution_context.hpp"

#include <vda5050_core/logger/logger.hpp>
#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>

namespace vda5050_bt_execution {

//=============================================================================
UpdateState::UpdateState(const std::string& name, const BT::NodeConfig& config)
: StatefulActionNode(name, config)
{
  // Nothing to do here ...
}

//=============================================================================
BT::PortsList UpdateState::providedPorts()
{
  return {BT::InputPort<std::shared_ptr<ExecutionContext>>("context")};
}

//=============================================================================
BT::NodeStatus UpdateState::onStart()
{
  return BT::NodeStatus::RUNNING;
}

//=============================================================================
BT::NodeStatus UpdateState::onRunning()
{
  auto context = getInput<std::shared_ptr<ExecutionContext>>("context").value();
  if (context) context->state_publisher->tick();
  return BT::NodeStatus::RUNNING;
}

//=============================================================================
void UpdateState::onHalted()
{
  // Nothing to do here ...
}

}  // namespace vda5050_bt_execution
