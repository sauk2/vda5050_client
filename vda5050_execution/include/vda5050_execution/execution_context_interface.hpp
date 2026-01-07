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

#ifndef VDA5050_EXECUTION__EXECUTION_CONTEXT_INTERFACE_HPP_
#define VDA5050_EXECUTION__EXECUTION_CONTEXT_INTERFACE_HPP_

#include <memory>

#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>
#include <vda5050_types/action.hpp>
#include <vda5050_types/action_status.hpp>
#include <vda5050_types/agv_position.hpp>
#include <vda5050_types/battery_state.hpp>
#include <vda5050_types/edge.hpp>
#include <vda5050_types/error.hpp>
#include <vda5050_types/node.hpp>
#include <vda5050_types/operating_mode.hpp>

#include "vda5050_execution/callback_registry.hpp"

namespace vda5050_execution {

struct Segment
{
  std::vector<std::shared_ptr<const vda5050_types::Node>> nodes;
  std::vector<std::shared_ptr<const vda5050_types::Edge>> edges;

  explicit operator bool() const
  {
    return !nodes.empty();
  }
};

class ExecutionContextInterface
{
public:
  virtual ~ExecutionContextInterface() = default;

  virtual Segment get_next_segment() = 0;

  virtual void acknowledge_node_reached(const std::string& node_id) = 0;

  virtual std::vector<std::shared_ptr<const vda5050_types::Action>>
  get_pending_actions() = 0;

  virtual std::vector<std::shared_ptr<const vda5050_types::Action>>
  get_pending_instant_actions() = 0;

  virtual void update_action_status(
    const std::string& action_id, vda5050_types::ActionStatus status,
    const std::vector<vda5050_types::Error>& errors = {}) = 0;

  virtual void add_error(const vda5050_types::Error& error) = 0;

  virtual void resolve_error(const std::string& error_type) = 0;

  virtual void clear_errors() = 0;

  CallbackRegistry& provider()
  {
    return providers_;
  }

private:
  CallbackRegistry providers_;
};

}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__EXECUTION_CONTEXT_INTERFACE_HPP_
