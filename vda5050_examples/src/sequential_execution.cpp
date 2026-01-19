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

#include <memory>

#include <vda5050_core/logger/logger.hpp>
#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>

#include <vda5050_types/edge.hpp>
#include <vda5050_types/node.hpp>

#include "vda5050_execution/base_execution_context.hpp"
#include "vda5050_execution/client_config.hpp"
#include "vda5050_execution/event.hpp"
#include "vda5050_execution/provider.hpp"
#include "vda5050_execution/sequential_execution_strategy.hpp"

int main()
{
  vda5050_execution::ClientConfig config{
    "uagv",
    "v2",
    "ROS-I",
    "S001",
    "tcp://localhost:1883",
    std::chrono::seconds(10)};

  auto strategy = vda5050_execution::SequentialExecutionStrategy::make();

  strategy->engine()->on<vda5050_execution::NavigationNodeReady>(
    [](std::shared_ptr<vda5050_execution::NavigationNodeReady> event) {
      VDA5050_INFO_STREAM("node_id: " << event->target_node->node_id);
      VDA5050_INFO_STREAM("edge_id: " << event->traversal_edge->edge_id);
    });

  strategy->engine()->on<vda5050_execution::NavigationStatusChange>(
    [](std::shared_ptr<vda5050_execution::NavigationStatusChange> event) {
      VDA5050_INFO_STREAM("pause: " << event->paused);
    });

  auto context = vda5050_execution::BaseExecutionContext::make(config);

  context->provider()->on<vda5050_execution::PositionData>(
    [w = context->weak_from_this()](
      std::shared_ptr<vda5050_execution::PositionData> update) {
      if (auto c = w.lock())
      {
        VDA5050_INFO_STREAM(
          "position: [" << update->agv_position->x << ", "
                        << update->agv_position->y << "]");
        c->update_position(*update->agv_position);
      }
    });

  context->provider()->on<vda5050_execution::BatteryData>(
    [w = context->weak_from_this()](
      std::shared_ptr<vda5050_execution::BatteryData> update) {
      if (auto c = w.lock())
      {
        VDA5050_INFO_STREAM(
          "battery: " << update->battery_state->battery_charge);
        c->update_battery_state(*update->battery_state);
      }
    });

  auto pos_1 = std::make_shared<vda5050_types::AGVPosition>();
  pos_1->x = 2;
  pos_1->y = 3;
  context->provider()->push<vda5050_execution::PositionData>(pos_1);

  strategy->step(context);

  auto pos_2 = std::make_shared<vda5050_types::AGVPosition>();
  pos_2->x = 3;
  pos_2->y = 3;
  context->provider()->push<vda5050_execution::PositionData>(pos_2);

  auto battery = std::make_shared<vda5050_types::BatteryState>();
  battery->battery_charge = 85.3;
  context->provider()->push<vda5050_execution::BatteryData>(battery);

  strategy->step(context);

  strategy->shutdown();
  context->shutdown();
}
