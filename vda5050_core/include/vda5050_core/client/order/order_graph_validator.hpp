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

#ifndef VDA5050_CORE__CLIENT__ORDER__ORDER_GRAPH_VALIDATOR_HPP_
#define VDA5050_CORE__CLIENT__ORDER__ORDER_GRAPH_VALIDATOR_HPP_

#include <vector>

#include "vda5050_core/client/order/graph_element.hpp"
#include "vda5050_core/client/order/validation_result.hpp"
#include "vda5050_types/order.hpp"

namespace vda5050_core {
namespace client {
namespace order {

/// \brief Utility class with functions to perform validity checks on the graph
/// contained in a VDA5050 Order message
class OrderGraphValidator
{
public:
  explicit OrderGraphValidator(const vda5050_types::Order& order);

  /// \brief Checks that the nodes and edges in a VDA5050 Order form a valid
  /// graph according to the VDA5050 specification sheet.
  ///
  /// \param order The order to be checked.
  /// \return ValidationResult containing if the order being checked is valid,
  /// and any errors if it is not.
  ///
  /// \return True if nodes and edges create a valid graph, false otherwise
  ValidationResult is_valid_graph();

private:
  const vda5050_types::Order& order_;

  std::vector<GraphElement> graph_;

  /// \brief Helper function to populate graph_ in the constructor.
  void create_graph();

  /// \brief Checks that the nodes and edges contained in a VDA5050 Order are
  /// arranged according to their sequenceId
  ///
  /// \param order The order to be checked.
  /// \return ValidationResult containing if the order being checked is valid,
  /// and any errors if it is not.
  ///
  /// \return True if nodes and edges are arranged according to their
  /// sequenceId, false otherwise
  ValidationResult is_in_traversal_order();

  /// \brief Checks that after the first unreleased node or edge, the subsequent
  /// nodes and edges are not released.
  ///
  /// \return ValidationResult containing if the order being checked is valid,
  /// and any errors if it is not.
  ValidationResult has_only_one_base();

  /// \brief Checks that all node sequenceIds are always even, and that all edge
  /// sequenceIds are always odd.
  ///
  /// \param order The order to be checked.
  ///
  /// \return ValidationResult containing if the order being checked is valid,
  /// and any errors if it is not.
  ValidationResult has_valid_sequence_ids();

  /// \brief Checks that startNodeId and endNodeId of all edges in a VDA5050
  /// Order match with its the start and end nodeIds
  ///
  /// \param order The order to be checked.
  /// \return ValidationResult containing if the order being checked is valid,
  /// and any errors if it is not.
  ///
  /// \return True if all edges' startNodeId and endNodId match, false otherwise
  ValidationResult has_valid_edges();
};

}  // namespace order
}  // namespace client
}  // namespace vda5050_core

#endif  // VDA5050_CORE__CLIENT__ORDER__ORDER_GRAPH_VALIDATOR_HPP_
