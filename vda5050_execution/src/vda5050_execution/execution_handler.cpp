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

#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>

#include "vda5050_execution/execution_handler.hpp"

namespace vda5050_execution {

//=============================================================================
ExecutionHandler::~ExecutionHandler()
{
  shutdown();
}

//=============================================================================
std::shared_ptr<ExecutionHandler> ExecutionHandler::make(
  std::shared_ptr<ExecutionContextInterface> context,
  std::shared_ptr<ExecutionStrategyInterface> strategy,
  std::shared_ptr<ExecutionDelegateInterface> delegate)
{
  auto handler = std::shared_ptr<ExecutionHandler>(
    new ExecutionHandler(context, strategy, delegate));
  return handler;
}

//=============================================================================
void ExecutionHandler::spin()
{
  while (!shutdown_) strategy_->step(context_);
}

//=============================================================================
void ExecutionHandler::spin_once()
{
  if (!shutdown_) strategy_->step(context_);
}

//=============================================================================
void ExecutionHandler::shutdown()
{
  shutdown_ = true;
}

//=============================================================================
ExecutionHandler::ExecutionHandler(
  std::shared_ptr<ExecutionContextInterface> context,
  std::shared_ptr<ExecutionStrategyInterface> strategy,
  std::shared_ptr<ExecutionDelegateInterface> delegate)
: context_(context), strategy_(strategy), delegate_(delegate), shutdown_(false)
{
  strategy_->engine().on<NavigationNodeReady>(
    [d = delegate_](std::shared_ptr<NavigationNodeReady> event) {
      d->on_navigation_node_ready(event->target_node, event->traversal_edge);
    });

  strategy_->engine().on<NavigationSegmentReady>(
    [d = delegate_](std::shared_ptr<NavigationSegmentReady> event) {
      d->on_navigation_segment_ready(
        event->target_segment, event->traversal_edges);
    });

  strategy_->engine().on<NavigationStatusChange>(
    [d = delegate_](std::shared_ptr<NavigationStatusChange> event) {
      d->on_navigation_status_change(event->paused);
    });

  strategy_->engine().on<NavigationReset>(
    [d = delegate_](std::shared_ptr<NavigationReset> /*event*/) {
      d->on_navigation_reset();
    });

  context->provider().register_provider<vda5050_execution::PositionData>(
    [d = delegate_]() {
      auto p = std::make_shared<vda5050_execution::PositionData>();
      p->agv_position = d->get_current_position();
      return p;
    });

  context->provider().register_provider<vda5050_execution::BatteryData>(
    [d = delegate_]() {
      auto b = std::make_shared<vda5050_execution::BatteryData>();
      b->battery_state = std::make_shared<vda5050_types::BatteryState>();
      return b;
    });
}

}  // namespace vda5050_execution
