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

#ifndef VDA5050_EXECUTION__EXECUTION_DELEGATE_INTERFACE_HPP_
#define VDA5050_EXECUTION__EXECUTION_DELEGATE_INTERFACE_HPP_

#include <memory>
#include <vector>

#include <vda5050_types/agv_position.hpp>
#include <vda5050_types/battery_state.hpp>
#include <vda5050_types/edge.hpp>
#include <vda5050_types/node.hpp>

#include "vda5050_execution/provider.hpp"

namespace vda5050_execution {

class ExecutionDelegateInterface
{
public:
  virtual ~ExecutionDelegateInterface() = default;

  virtual void set_provider(std::shared_ptr<Provider> provider) = 0;

  virtual void on_navigation_node_ready(
    std::shared_ptr<const vda5050_types::Node> target_node,
    std::shared_ptr<const vda5050_types::Edge> traversal_edge)
  {
  }

  virtual void on_navigation_segment_ready(
    std::vector<std::shared_ptr<const vda5050_types::Node>> target_segment,
    std::vector<std::shared_ptr<const vda5050_types::Edge>> traversal_edges)
  {
  }

  virtual void on_navigation_status_change(bool pause = false) = 0;

  virtual void on_navigation_reset() {}

  // virtual std::shared_ptr<vda5050_types::AGVPosition>
  // get_current_position() = 0;

  // virtual std::shared_ptr<vda5050_types::BatteryState>
  // get_current_battery() = 0;
};

}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__EXECUTION_DELEGATE_INTERFACE_HPP_
