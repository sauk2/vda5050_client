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

  strategy->engine().on<vda5050_execution::NavigationNodeReady>(
    [](std::shared_ptr<vda5050_execution::NavigationNodeReady> event) {
      VDA5050_INFO_STREAM("node_id: " << event->target_node->node_id);
      VDA5050_INFO_STREAM("edge_id: " << event->traversal_edge->edge_id);
    });

  strategy->engine().on<vda5050_execution::NavigationStatusChange>(
    [](std::shared_ptr<vda5050_execution::NavigationStatusChange> event) {
      VDA5050_INFO_STREAM("pause: " << event->paused);
    });

  auto context = vda5050_execution::BaseExecutionContext::make(config);

  context->provider().register_provider<vda5050_execution::PositionData>([]() {
    auto p = std::make_shared<vda5050_execution::PositionData>();
    p->agv_position = std::make_shared<vda5050_types::AGVPosition>();
    return p;
  });

  context->provider().register_provider<vda5050_execution::BatteryData>([]() {
    auto b = std::make_shared<vda5050_execution::BatteryData>();
    b->battery_state = std::make_shared<vda5050_types::BatteryState>();
    return b;
  });

  strategy->step(context);
  strategy->step(context);

  strategy->shutdown();
  context->shutdown();
}

