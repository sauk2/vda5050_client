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

#include <iostream>
#include <memory>

#include <vda5050_core/logger/logger.hpp>

#include "vda5050_execution/base_execution_context.hpp"
#include "vda5050_execution/client_config.hpp"
#include "vda5050_execution/execution_delegate_interface.hpp"
#include "vda5050_execution/execution_handler.hpp"
#include "vda5050_execution/provider.hpp"
#include "vda5050_execution/sequential_execution_strategy.hpp"

class CustomExecutionDelegate
: public vda5050_execution::ExecutionDelegateInterface
{
public:
  static std::shared_ptr<CustomExecutionDelegate> make()
  {
    auto delegate =
      std::shared_ptr<CustomExecutionDelegate>(new CustomExecutionDelegate());
    return delegate;
  }
  void set_provider(
    std::shared_ptr<vda5050_execution::Provider> provider) override
  {
    provider_ = provider;
  }

  void on_navigation_node_ready(
    std::shared_ptr<const vda5050_types::Node> target_node,
    std::shared_ptr<const vda5050_types::Edge> traversal_edge) override
  {
    VDA5050_INFO_STREAM("node_id: " << target_node->node_id);
    VDA5050_INFO_STREAM("edge_id: " << traversal_edge->edge_id);
  }

  void on_navigation_status_change(bool pause = false) override
  {
    VDA5050_INFO_STREAM("pause: " << pause);
  }

  void update()
  {
    auto pos = std::make_shared<vda5050_types::AGVPosition>();
    pos->x = 2;
    pos->y = 3;
    provider_->push<vda5050_execution::PositionData>(pos);

    auto battery = std::make_shared<vda5050_types::BatteryState>();
    battery->battery_charge = 85.3;
    provider_->push<vda5050_execution::BatteryData>(battery);
  }

private:
  CustomExecutionDelegate()
  {
    // Nothing to do here ...
  }

  std::shared_ptr<vda5050_execution::Provider> provider_;
};

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
  auto context = vda5050_execution::BaseExecutionContext::make(config);
  auto delegate = CustomExecutionDelegate::make();

  auto handler =
    vda5050_execution::ExecutionHandler::make(context, strategy, delegate);

  delegate->update();
  handler->spin_once();

  delegate->update();
  handler->spin_once();

  strategy->shutdown();
  context->shutdown();
}
