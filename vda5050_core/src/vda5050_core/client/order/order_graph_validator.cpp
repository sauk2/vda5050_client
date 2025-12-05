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

#include "vda5050_core/client/order/graph_element.hpp"
#include "vda5050_core/client/order/order_graph_validator.hpp"
#include "vda5050_core/logger/logger.hpp"
#include "vda5050_types/edge.hpp"
#include "vda5050_types/error.hpp"
#include "vda5050_types/error_level.hpp"
#include "vda5050_types/node.hpp"

namespace vda5050_core {
namespace client {
namespace order {
OrderGraphValidator::OrderGraphValidator(const vda5050_types::Order& order)
: order_(order)
{
  create_graph();
}

void OrderGraphValidator::create_graph()
{
  bool add_node{true};
  size_t node_idx = 0;
  size_t edge_idx = 0;

  while (node_idx < order_.nodes.size() || edge_idx < order_.edges.size())
  {
    if (add_node)
    {
      if (node_idx == order_.nodes.size())
      {
        for (size_t i = edge_idx; i < order_.edges.size(); i++)
        {
          GraphElement graph_element{order_.edges.at(i)};
          graph_.push_back(graph_element);
        }
        return;
      }
      GraphElement graph_element{order_.nodes.at(node_idx)};
      graph_.push_back(graph_element);
      node_idx++;
      add_node = false;
    }
    else
    {
      if (edge_idx == order_.edges.size())
      {
        for (size_t i = node_idx; i < order_.nodes.size(); i++)
        {
          GraphElement graph_element{order_.nodes.at(i)};
          graph_.push_back(graph_element);
        }
        return;
      }
      GraphElement graph_element{order_.edges.at(edge_idx)};
      graph_.push_back(graph_element);
      edge_idx++;
      add_node = true;
    }
  }
}

ValidationResult OrderGraphValidator::is_valid_graph()
{
  ValidationResult res{true, {}};

  /// number of nodes can only be 1 more than number of edges
  if (!(order_.nodes.size() - 1 == order_.edges.size()))
  {
    VDA5050_ERROR(
      "Graph Validation Error: The order.edges.size() must be "
      "order.nodes.size() - 1.");

    vda5050_types::Error graph_validation_error{
      "Graph Validation Error",
      std::vector{
        vda5050_types::ErrorReference{"orderId", order_.order_id},
        vda5050_types::ErrorReference{
          "orderUpdateId", std::to_string(order_.order_update_id)}},
      "Difference in number of nodes and edges in the given order is not 1.",
      vda5050_types::ErrorLevel::WARNING};

    res.valid = false;
    res.errors.push_back(graph_validation_error);

    return res;
  }

  /// if there is more than one node and at least one edge, check that their
  /// sequenceIds are in order
  ValidationResult valid_traversal_order_result = is_in_traversal_order();
  if (!valid_traversal_order_result.valid)
  {
    VDA5050_ERROR(
      "Graph Validation Error: Nodes and edges are not organized according to "
      "their sequenceIds.");

    return valid_traversal_order_result;
  }

  ValidationResult valid_sequence_ids_result = has_valid_sequence_ids();
  if (!valid_sequence_ids_result.valid)
  {
    VDA5050_ERROR(
      "Graph Validation Error: Nodes must have even sequenceIds and edges must "
      "have odd sequenceIds.");

    return valid_sequence_ids_result;
  }

  ValidationResult valid_edges_result = has_valid_edges();
  if (!valid_edges_result.valid)
  {
    VDA5050_ERROR(
      "Graph Validation Error: start_node_id and end_node_id of an edge do not "
      "match the node_ids of the Order's first node and last node.");

    return valid_edges_result;
  }

  ValidationResult only_one_base_result = has_only_one_base();
  if (!only_one_base_result.valid)
  {
    VDA5050_ERROR(
      "Graph Validation Error: Node or edge with attribute \"released = true\" "
      "found after a node or edge with attribute  \"released = false\".");

    return only_one_base_result;
  }

  return res;
}

ValidationResult OrderGraphValidator::is_in_traversal_order()
{
  ValidationResult res{true, {}};

  /// TODO: (shawnkchan) probably don't need this variable
  uint32_t current_sequence_id = graph_.at(0).sequence_id;

  for (size_t i = 1; i < graph_.size(); i++)
  {
    uint32_t next_sequence_id = graph_.at(i).sequence_id;

    if (next_sequence_id != current_sequence_id + 1)
    {
      /// return false validation result if next_sequence_id is not contiguous
      vda5050_types::Error error{};
      error.error_type = "Graph Validation Error";
      error.error_level = vda5050_types::ErrorLevel::WARNING;

      if (graph_.at(i).is_node)
      {
        error.error_description = fmt::format(
          "Node with nodeId \"{}\" expected to have sequence_id equal to {}, "
          "but is actually {}",
          graph_.at(i).element_id, current_sequence_id + 1, next_sequence_id);
        error.error_references = {
          vda5050_types::ErrorReference{"nodeId", graph_.at(i).element_id}};
      }
      else
      {
        error.error_description = fmt::format(
          "Edge with edgeId \"{}\" expected to have sequence_id equal to {}, "
          "but is actually "
          "{}",
          graph_.at(i).element_id, current_sequence_id + 1, next_sequence_id);
        error.error_references = {
          vda5050_types::ErrorReference{"edgeId", graph_.at(i).element_id}};
      }

      res.valid = false;
      res.errors.push_back(error);
      return res;
    }

    current_sequence_id = next_sequence_id;
  }
  return res;
}

ValidationResult OrderGraphValidator::has_valid_sequence_ids()
{
  ValidationResult res{true, {}};

  for (const vda5050_types::Node& n : order_.nodes)
  {
    if (n.sequence_id % 2 != 0)
    {
      vda5050_types::Error invalid_node_sequence_id_error{};
      invalid_node_sequence_id_error.error_type = "Graph Validation Error";
      invalid_node_sequence_id_error.error_level =
        vda5050_types::ErrorLevel::WARNING;
      invalid_node_sequence_id_error.error_description = fmt::format(
        "Node with nodeId \"{}\" has a sequenceId that is odd. All node "
        "sequenceIds can only be even.",
        n.node_id);

      vda5050_types::ErrorReference node_error_reference{"nodeId", n.node_id};
      invalid_node_sequence_id_error.error_references = {node_error_reference};

      res.valid = false;
      res.errors.push_back(invalid_node_sequence_id_error);
    }
  }

  for (const vda5050_types::Edge& e : order_.edges)
  {
    if (e.sequence_id % 2 == 0)
    {
      vda5050_types::Error invalid_edge_sequence_id_error{};
      invalid_edge_sequence_id_error.error_type = "Graph Validation Error";
      invalid_edge_sequence_id_error.error_level =
        vda5050_types::ErrorLevel::WARNING;
      invalid_edge_sequence_id_error.error_description = fmt::format(
        "Edge with edgeId \"{}\" has a sequenceId that is even. All edge "
        "sequenceIds can only be even.",
        e.edge_id);

      vda5050_types::ErrorReference edge_error_reference{"edgeId", e.edge_id};
      invalid_edge_sequence_id_error.error_references = {edge_error_reference};

      res.valid = false;
      res.errors.push_back(invalid_edge_sequence_id_error);
    }
  }
  return res;
}

ValidationResult OrderGraphValidator::has_only_one_base()
{
  ValidationResult res{true, {}};

  bool has_base_ended = false;

  for (const GraphElement& g : graph_)
  {
    if (!has_base_ended)
    {
      /// if an unreleased node or edge is encountered, we have reached the end
      /// of the base
      if (!g.released)
      {
        has_base_ended = true;
      }
    }
    /// if we have reached the end of the base, add an error to res if we
    /// encounter a released node or edge
    else
    {
      if (g.released)
      {
        vda5050_types::Error multiple_bases_error{};
        multiple_bases_error.error_type = "Graph Validation Error";
        multiple_bases_error.error_level = vda5050_types::ErrorLevel::WARNING;

        if (g.is_node)
        {
          multiple_bases_error.error_description = fmt::format(
            "Node with nodeId \"{}\" is released, but previous nodes and edges "
            "are unreleased. Ensure that there is only one base in the order",
            g.element_id);

          vda5050_types::ErrorReference node_error_reference{
            "nodeId", g.element_id};
          multiple_bases_error.error_references = {node_error_reference};
        }
        else
        {
          multiple_bases_error.error_description = fmt::format(
            "Edge with edgeId \"{}\" is released, but previous nodes and edges "
            "are unreleased. Ensure that there is only one base in the order",
            g.element_id);

          vda5050_types::ErrorReference edge_error_reference{
            "edgeId", g.element_id};
          multiple_bases_error.error_references = {edge_error_reference};
        }

        res.valid = false;
        res.errors.push_back(multiple_bases_error);
      }
    }
  }
  return res;
}

ValidationResult OrderGraphValidator::has_valid_edges()
{
  ValidationResult res{true, {}};

  /// if the graph consists of only one node, we can skip this check
  if (graph_.size() == 1)
  {
    return res;
  }

  for (size_t i = 1; i < graph_.size(); i += 2)
  {
    GraphElement current_edge = graph_.at(i);
    GraphElement previous_node = graph_.at(i - 1);
    GraphElement next_node = graph_.at(i + 1);

    if (
      current_edge.start_node_id != previous_node.element_id ||
      current_edge.end_node_id != next_node.element_id)
    {
      vda5050_types::Error invalid_edges_error{};
      invalid_edges_error.error_type = "Graph Validation Error";
      invalid_edges_error.error_level = vda5050_types::ErrorLevel::WARNING;
      invalid_edges_error.error_description = fmt::format(
        "Edge with edgeId \"{}\" does not have start and end nodeIds that "
        "match the nodeIds of its neighbouring nodes.",
        current_edge.element_id);

      vda5050_types::ErrorReference edge_error_reference{
        "edgeId", current_edge.element_id};
      invalid_edges_error.error_references = {edge_error_reference};

      res.valid = false;
      res.errors.push_back(invalid_edges_error);
    }
  }
  return res;
}

}  // namespace order
}  // namespace client
}  // namespace vda5050_core
