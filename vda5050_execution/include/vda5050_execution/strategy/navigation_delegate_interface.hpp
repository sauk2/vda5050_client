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

#ifndef VDA5050_EXECUTION__STRATEGY__NAVIGATION_DELEGATE_INTERFACE_HPP_
#define VDA5050_EXECUTION__STRATEGY__NAVIGATION_DELEGATE_INTERFACE_HPP_

#include <memory>

#include <vda5050_types/edge.hpp>
#include <vda5050_types/node.hpp>

#include "vda5050_execution/core/provider.hpp"

namespace vda5050_execution {

namespace strategy {

class NavigationDelegateInterface
{
public:
  virtual ~NavigationDelegateInterface() = default;

  virtual void set_provider(std::shared_ptr<core::Provider> provider) = 0;

  virtual void on_navigation_node_ready(
    std::shared_ptr<const vda5050_types::Node> /*target_node*/,
    std::shared_ptr<const vda5050_types::Edge> /*traversal_edge*/){};

  virtual void on_navigation_segment_ready(
    std::shared_ptr<const vda5050_types::Node> /*target_nodes*/,
    std::shared_ptr<const vda5050_types::Edge> /*traversal_edges*/){};

  virtual void on_navigation_status_change(bool pause = false) = 0;

  virtual void on_navigation_reset() {}
};

}  // namespace strategy
}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__NAVIGATION_DELEGATE_INTERFACE_HPP_
