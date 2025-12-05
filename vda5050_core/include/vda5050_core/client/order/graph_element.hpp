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

#ifndef VDA5050_CORE__CLIENT__ORDER__GRAPH_ELEMENT_HPP_
#define VDA5050_CORE__CLIENT__ORDER__GRAPH_ELEMENT_HPP_

#include <cstdint>
#include <optional>
#include <string>
#include <vda5050_types/edge.hpp>
#include <vda5050_types/node.hpp>

namespace vda5050_core {
namespace client {
namespace order {

/// \brief A representation of an element in a VDA5050 order's graph that can
/// be constructed from either a Node or Edge object
struct GraphElement
{
  /// \brief Constructor if GraphElement takes attributes from a
  /// vda5050types::Node \param node vda5050_types::Node
  explicit GraphElement(const vda5050_types::Node& node);

  /// \brief Constructor if GraphElement takes attributes from a
  /// vda5050types::Edge \param edge vda5050_types::Edge
  explicit GraphElement(const vda5050_types::Edge& edge);

  std::string element_id;

  uint32_t sequence_id;

  bool released;

  std::optional<std::string> start_node_id;

  std::optional<std::string> end_node_id;

  bool is_node;
};

}  // namespace order
}  // namespace client
}  // namespace vda5050_core

#endif  // VDA5050_CORE__CLIENT__ORDER__GRAPH_ELEMENT_HPP_
