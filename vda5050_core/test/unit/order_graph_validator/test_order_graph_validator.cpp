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

#include <gtest/gtest.h>
#include <optional>
#include <vector>

#include "vda5050_core/client/order/graph_element.hpp"
#include "vda5050_core/client/order/order_graph_validator.hpp"
#include "vda5050_core/client/order/validation_result.hpp"
#include "vda5050_types/edge.hpp"
#include "vda5050_types/node.hpp"
#include "vda5050_types/order.hpp"

class OrderGraphValidatorTest : public testing::Test
{
protected:
  vda5050_types::Node n0_{"node0", 0, true, {}, std::nullopt, std::nullopt};
  vda5050_types::Edge e1_{"edge1",      1,
                          "node0",      "node2",
                          true,         {},
                          std::nullopt, std::nullopt,
                          std::nullopt, std::nullopt,
                          std::nullopt, std::nullopt,
                          std::nullopt, std::nullopt,
                          std::nullopt, std::nullopt,
                          std::nullopt};
  vda5050_types::Node n2_{"node2", 2, true, {}, std::nullopt, std::nullopt};
  vda5050_types::Edge e3_{"edge3",      3,
                          "node2",      "node4",
                          true,         {},
                          std::nullopt, std::nullopt,
                          std::nullopt, std::nullopt,
                          std::nullopt, std::nullopt,
                          std::nullopt, std::nullopt,
                          std::nullopt, std::nullopt,
                          std::nullopt};
  vda5050_types::Node n4_{"node4", 4, true, {}, std::nullopt, std::nullopt};

  vda5050_types::Order order_{};
};

/// \brief Tests that graph validator returns true on a valid graph
TEST_F(OrderGraphValidatorTest, ValidGraphTest)
{
  std::vector<vda5050_types::Node> nodes;
  std::vector<vda5050_types::Edge> edges;

  nodes.push_back(n0_);
  edges.push_back(e1_);
  nodes.push_back(n2_);
  edges.push_back(e3_);
  nodes.push_back(n4_);

  order_.edges = edges;
  order_.nodes = nodes;

  vda5050_core::client::order::OrderGraphValidator order_graph_validator{
    order_};
  vda5050_core::client::order::ValidationResult res =
    order_graph_validator.is_valid_graph();

  EXPECT_TRUE(res.valid);
  EXPECT_TRUE(res.errors.empty());
}

// /// \brief Tests that graph validator returns false when nodes and edges are
// not in traversal order
TEST_F(OrderGraphValidatorTest, NotInTraversalOrderTest)
{
  std::vector<vda5050_types::Node> nodes;
  std::vector<vda5050_types::Edge> edges;

  nodes.push_back(n0_);
  edges.push_back(e3_);
  nodes.push_back(n2_);
  edges.push_back(e1_);
  nodes.push_back(n4_);

  order_.edges = edges;
  order_.nodes = nodes;

  vda5050_core::client::order::OrderGraphValidator order_graph_validator{
    order_};
  vda5050_core::client::order::ValidationResult res =
    order_graph_validator.is_valid_graph();

  EXPECT_FALSE(res.valid);

  /// e3_ should be the cause of the error
  std::string expected_error_reference_value =
    res.errors.at(0).error_references.value().at(0).reference_value;
  EXPECT_EQ(e3_.edge_id, expected_error_reference_value);
}

/// \brief Tests that graph validator returns false if the difference in number
/// of nodes and edges is greater than one
TEST_F(OrderGraphValidatorTest, IncorrectNumberOfNodesAndEdgesTest)
{
  std::vector<vda5050_types::Node> nodes;
  std::vector<vda5050_types::Edge> edges;

  nodes.push_back(n0_);
  edges.push_back(e3_);
  nodes.push_back(n2_);
  edges.push_back(e1_);

  vda5050_types::Node n6 =
    vda5050_types::Node{"node6", 6, true, {}, std::nullopt, std::nullopt};
  nodes.push_back(n6);

  nodes.push_back(n4_);

  order_.order_id = "order0";
  order_.nodes = nodes;
  order_.edges = edges;

  vda5050_core::client::order::OrderGraphValidator order_graph_validator{
    order_};
  vda5050_core::client::order::ValidationResult res =
    order_graph_validator.is_valid_graph();

  EXPECT_FALSE(res.valid);

  std::string expected_error_reference_value =
    res.errors.at(0).error_references.value().at(0).reference_value;
  EXPECT_EQ(order_.order_id, expected_error_reference_value);
}
