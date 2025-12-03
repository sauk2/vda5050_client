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

#include <cstdint>
#include <optional>
#include <string>
#include <vda5050_types/edge.hpp>
#include <vda5050_types/node.hpp>

#include "vda5050_core/client/order/graph_element.hpp"

namespace vda5050_core {
namespace client {
namespace order {

GraphElement::GraphElement(const vda5050_types::Node& node)
: element_id(node.node_id),
  sequence_id(node.sequence_id),
  released(node.released),
  is_node(true)
{
}

GraphElement::GraphElement(const vda5050_types::Edge& edge)
: element_id(edge.edge_id),
  sequence_id(edge.sequence_id),
  released(edge.released),
  start_node_id(edge.start_node_id),
  end_node_id(edge.end_node_id),
  is_node(false)
{
}

}  // namespace order
}  // namespace client
}  // namespace vda5050_core
