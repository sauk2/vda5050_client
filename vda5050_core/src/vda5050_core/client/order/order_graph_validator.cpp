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

#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>
#include <variant>

#include "vda5050_core/client/order/order_graph_validator.hpp"
#include "vda5050_types/edge.hpp"
#include "vda5050_types/error.hpp"
#include "vda5050_types/error_level.hpp"
#include "vda5050_core/logger/logger.hpp"
#include "vda5050_types/node.hpp"

namespace vda5050_core {
namespace client {
namespace order {
ValidationResult OrderGraphValidator::is_valid_graph(const vda5050_types::Order& order)
{
  ValidationResult res {true, {}};

  /// impossible for number of nodes to exceed number of edges by 1
  if (!(order.nodes.size() - 1 == order.edges.size()))
  {
    VDA5050_ERROR("Graph Validation Error: Difference in number of nodes and edges is not 1.");

    vda5050_types::Error graph_validation_error {"Graph Validation Error", std::vector {vda5050_types::ErrorReference{"orderId", order.order_id}, vda5050_types::ErrorReference{"orderUpdateId", std::to_string(order.order_update_id)}}, "Difference in number of nodes and edges in the given order is not 1.", vda5050_types::ErrorLevel::FATAL
    };

    res.valid = false;
    res.errors.push_back(graph_validation_error);

    return res;
  }

  /// if there is only one node, graph must be valid.
  if (order.nodes.size() == 1)
  {
    return res;
  }

  /// if there is more than one node and at least one edge, check that their sequenceIds are in order
  ValidationResult valid_traversal_order = is_in_traversal_order(order);
  if (!valid_traversal_order.valid)
  {
    VDA5050_ERROR("Graph Validation Error: Nodes and edges are not in traversal order.");

    res.valid = false;
    res.errors.push_back(valid_traversal_order.errors.at(0));
    return res;
  }

  ValidationResult valid_edges = is_valid_edges(order);
  if (!valid_edges.valid)
  {
    VDA5050_ERROR("Graph Validation Error: start_node_id and end_node_id of an edge does not match the node_ids of the Order's first node and last node.");

    res.valid = false;
    res.errors.push_back(valid_edges.errors.at(0));
    return res;
  }

  return res;
}

ValidationResult OrderGraphValidator::is_in_traversal_order(const vda5050_types::Order& order)
{
  /// lambda functions to get sequenceIds and compare variants using sequenceIds
  auto getSequenceId
  {
    [](auto&& graph_element)
    {
      return graph_element.sequence_id;
    }
  };

  auto sequenceIdComparison
  {
    [&getSequenceId](auto const& a, auto const& b)
    {
      return std::visit(getSequenceId, a) < std::visit(getSequenceId, b);
    }
  };

  auto getNodeOrEdgeId
  {
    [](auto&& graph_element)
    {
      using T = std::decay_t<decltype(graph_element)>;
      if constexpr (std::is_same_v<T, vda5050_types::Node>)
      {
        return graph_element.node_id;
      }
      else if constexpr (std::is_same_v<T, vda5050_types::Edge>)
      {
      return graph_element.edge_id;
      }
    }
  };

  ValidationResult res {true, {}};

  /// store all nodes and edges in a vector
  std::vector<std::variant<vda5050_types::Node, vda5050_types::Edge>> graph;
  for (vda5050_types::Node n : order.nodes)
  {
    graph.push_back(n);
  }

  for (vda5050_types::Edge e : order.edges)
  {
    graph.push_back(e);
  }

  /// sort nodes and edges according to their sequenceIds
  std::sort(graph.begin(), graph.end(), sequenceIdComparison);

  /// check each sequenceId and ensure they are in running order
  int current_sequence_id = std::visit(getSequenceId, graph.front());
  for (int i = 1; i < graph.size(); i++)
  {
    if (current_sequence_id + 1 != std::visit(getSequenceId, graph.at(i)))
    {
      vda5050_types::Error not_in_traversal_order_error {};
      not_in_traversal_order_error.error_type = "Graph Validation Error";
      not_in_traversal_order_error.error_description = "Nodes and Edges of the given order are not in order of sequenceId";

      vda5050_types::ErrorReference prev_element_err_ref {};
      vda5050_types::ErrorReference current_element_err_ref {};

      if (std::holds_alternative<vda5050_types::Node>(graph.at(i - 1)))
      {
        prev_element_err_ref.reference_key = "nodeId";
        current_element_err_ref.reference_key = "edgeId";
      }
      else
      {
        prev_element_err_ref.reference_key = "edgeId";
        current_element_err_ref.reference_key = "nodeId";
      }
      prev_element_err_ref.reference_value = std::visit(getNodeOrEdgeId, graph.at(i - 1));
      current_element_err_ref.reference_key = std::visit(getNodeOrEdgeId, graph.at(i));

      not_in_traversal_order_error.error_references = {prev_element_err_ref, current_element_err_ref};

      res.valid = false;
      res.errors = {not_in_traversal_order_error};
      return res;
    }
  }

  return res;
}

ValidationResult OrderGraphValidator::is_valid_edges(const vda5050_types::Order& order)
{
  ValidationResult res {true, {}};

  std::string start_node_id = order.nodes.front().node_id;
  std::string end_node_id = order.nodes.back().node_id;

  for (vda5050_types::Edge e : order.edges)
  {
    if (e.start_node_id != start_node_id && e.end_node_id != end_node_id)
    {
      vda5050_types::Error invalid_edges_error {};
      invalid_edges_error.error_type = "Graph Validation Error";
      invalid_edges_error.error_references = {vda5050_types::ErrorReference{"edgeId", e.edge_id}};
      invalid_edges_error.error_description = "start_node_id and end_node_id of the edge does not match the node_ids of the order's first and last node respectively.";
      invalid_edges_error.error_level = vda5050_types::ErrorLevel::FATAL;

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
