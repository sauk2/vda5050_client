/**
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

#include <fmt/core.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <queue>
#include <string>
#include <variant>
#include <vector>

#include "vda5050_core/client/order/order_graph_validator.hpp"
#include "vda5050_core/logger/logger.hpp"
#include "vda5050_types/edge.hpp"
#include "vda5050_types/error.hpp"
#include "vda5050_types/error_level.hpp"
#include "vda5050_types/node.hpp"

namespace vda5050_core {
namespace client {
namespace order {
ValidationResult OrderGraphValidator::is_valid_graph(
  const vda5050_types::Order& order)
{
  ValidationResult res{true, {}};

  /// impossible for number of nodes to exceed number of edges by 1
  if (!(order.nodes.size() - 1 == order.edges.size()))
  {
    VDA5050_ERROR(
      "Graph Validation Error: The order.edges.size() must be "
      "order.nodes.size() - 1.");

    vda5050_types::Error graph_validation_error{
      "Graph Validation Error",
      std::vector{
        vda5050_types::ErrorReference{"orderId", order.order_id},
        vda5050_types::ErrorReference{
          "orderUpdateId", std::to_string(order.order_update_id)}},
      "Difference in number of nodes and edges in the given order is not 1.",
      vda5050_types::ErrorLevel::WARNING};

    res.valid = false;
    res.errors.push_back(graph_validation_error);
    return res;
  }

  /// if there is only one node, graph must be valid.
  if (order.nodes.size() == 1)
  {
    return res;
  }

  /// if there is more than one node and at least one edge, check that their
  /// sequenceIds are in order
  ValidationResult valid_traversal_order_result = is_in_traversal_order(order);
  if (!valid_traversal_order_result.valid)
  {
    VDA5050_ERROR(
      "Graph Validation Error: Nodes and edges are not organized according to "
      "their sequenceIds.");

    return valid_traversal_order_result;
  }

  ValidationResult valid_edges_result = is_valid_edges(order);
  if (!valid_edges_result.valid)
  {
    VDA5050_ERROR(
      "Graph Validation Error: start_node_id and end_node_id of an edge do not "
      "match the node_ids of the Order's first node and last node.");

    return valid_edges_result;
  }

  return res;
}

ValidationResult OrderGraphValidator::is_in_traversal_order(
  const vda5050_types::Order& order)
{
  ValidationResult res{true, {}};

  /// establish two indices to track the current edge and node that we are
  /// comparing
  size_t node_idx = 0;
  size_t edge_idx = 0;

  while (node_idx < order.nodes.size() && edge_idx < order.edges.size())
  {
    vda5050_types::Edge current_edge = order.edges.at(edge_idx);
    vda5050_types::Node current_node = order.nodes.at(node_idx);

    if (node_idx == edge_idx)
    {
      if (current_edge.sequence_id == current_node.sequence_id + 1)
      {
        node_idx++;
      }
      else
      {
        vda5050_types::Error error{};
        error.error_type = "Graph Validation Error";
        error.error_description = fmt::format(
          "EdgeId {} expected to have sequence_id equal to {}, but is actually "
          "{}",
          current_edge.edge_id, current_node.sequence_id + 1,
          current_edge.sequence_id);
        error.error_level = vda5050_types::ErrorLevel::WARNING;
        error.error_references = {
          vda5050_types::ErrorReference{"edgeId", current_edge.edge_id}};

        res.valid = false;
        res.errors.push_back(error);
        return res;
      }
    }
    else
    {
      if (current_edge.sequence_id + 1 == current_node.sequence_id)
      {
        edge_idx++;
      }
      else
      {
        vda5050_types::Error error{};
        error.error_type = "Graph Validation Error";
        error.error_description = fmt::format(
          "NodeId {} expected to have sequence_id equal to {}, but is actually "
          "{}",
          current_node.node_id, current_edge.sequence_id + 1,
          current_node.sequence_id);
        error.error_level = vda5050_types::ErrorLevel::WARNING;
        error.error_references = {
          vda5050_types::ErrorReference{"nodeId", current_node.node_id}};

        res.valid = false;
        res.errors.push_back(error);
        return res;
      }
    }
  }

  return res;
}

ValidationResult OrderGraphValidator::is_valid_edges(
  const vda5050_types::Order& order)
{
  ValidationResult res{true, {}};

  std::string start_node_id = order.nodes.front().node_id;
  std::string end_node_id = order.nodes.back().node_id;

  for (vda5050_types::Edge e : order.edges)
  {
    if (e.start_node_id != start_node_id && e.end_node_id != end_node_id)
    {
      vda5050_types::Error invalid_edges_error{};
      invalid_edges_error.error_type = "Graph Validation Error";
      invalid_edges_error.error_references = {
        vda5050_types::ErrorReference{"edgeId", e.edge_id}};
      invalid_edges_error.error_description =
        "start_node_id and end_node_id of the edge does not match the node_ids "
        "of the order's first and last node respectively.";
      invalid_edges_error.error_level = vda5050_types::ErrorLevel::WARNING;

      res.valid = false;
      res.errors = {invalid_edges_error};
      return res;
    }
  }
  return res;
}

}  // namespace order
}  // namespace client
}  // namespace vda5050_core
