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

#ifndef VDA5050_EXECUTION__RESOURCES__ORDER_RESOURCES_HPP_
#define VDA5050_EXECUTION__RESOURCES__ORDER_RESOURCES_HPP_

#include <algorithm>
#include <memory>
#include <mutex>

#include <vda5050_types/order.hpp>

#include "vda5050_execution/core/base.hpp"

namespace vda5050_execution {

namespace resources {

class OrderManager : public core::Initialize<OrderManager, core::ResourceBase>
{
public:
  OrderManager() : current_sequence_(0)
  {
    // Nothing to do here ...
  }

  void update_order(const vda5050_types::Order& order)
  {
    std::lock_guard<std::mutex> lock(order_mutex_);
    *current_order_ = std::move(order);

    if (!current_order_->nodes.empty())
      current_sequence_ = current_order_->nodes[0].sequence_id;
  }

  std::optional<std::pair<vda5050_types::Node, vda5050_types::Edge>> get_next()
  {
    std::lock_guard<std::mutex> lock(order_mutex_);

    uint32_t target_edge_seq = current_sequence_ + 1;
    uint32_t target_node_seq = current_sequence_ + 2;

    auto edge_it = std::find_if(
      current_order_->edges.begin(), current_order_->edges.end(),
      [target_edge_seq](const auto& e) {
        return e.sequence_id == target_edge_seq && e.released;
      });

    auto node_it = std::find_if(
      current_order_->nodes.begin(), current_order_->nodes.end(),
      [target_node_seq](const auto& n) {
        return n.sequence_id == target_node_seq && n.released;
      });

    if (
      node_it != current_order_->nodes.end() &&
      edge_it != current_order_->edges.end())
      return std::make_pair(*node_it, *edge_it);

    return std::nullopt;
  }

  void acknowledge_sequence(uint32_t sequence)
  {
    std::lock_guard<std::mutex> lock(order_mutex_);
    if (sequence > current_sequence_) current_sequence_ = sequence;
  }

private:
  std::shared_ptr<vda5050_types::Order> current_order_;
  uint32_t current_sequence_;
  std::mutex order_mutex_;
};

}  // namespace resources
}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__RESOURCES__ORDER_RESOURCES_HPP_
