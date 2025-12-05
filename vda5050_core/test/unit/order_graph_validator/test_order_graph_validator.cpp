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

#include <vda5050_types/edge.hpp>
#include <vda5050_types/node.hpp>
#include <vda5050_types/order.hpp>
#include "vda5050_core/client/order/graph_element.hpp"
#include "vda5050_core/client/order/order_graph_validator.hpp"
#include "vda5050_core/client/order/validation_result.hpp"

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
  std::vector<vda5050_types::Node> nodes;
  std::vector<vda5050_types::Edge> edges;
};

/// \brief Tests that graph validator returns true on a valid graph
TEST_F(OrderGraphValidatorTest, ValidGraphTest)
{
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

/// \brief Tests that graph validator returns false if there are more nodes
/// than edges
TEST_F(OrderGraphValidatorTest, MoreNodesThanEdgesTest)
{
  nodes.push_back(n0_);
  edges.push_back(e1_);
  nodes.push_back(n2_);
  edges.push_back(e3_);
  nodes.push_back(n4_);

  vda5050_types::Node n6{"node6", 6, true, {}, std::nullopt, std::nullopt};
  nodes.push_back(n6);

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

/// \brief Tests that validation fails if there are more edges
/// than nodes
TEST_F(OrderGraphValidatorTest, MoreEdgesThanNodesTest)
{
  nodes.push_back(n0_);
  edges.push_back(e1_);
  nodes.push_back(n2_);
  edges.push_back(e3_);

  vda5050_types::Edge e5{"edge5",      5,
                         "node4",      "node6",
                         true,         {},
                         std::nullopt, std::nullopt,
                         std::nullopt, std::nullopt,
                         std::nullopt, std::nullopt,
                         std::nullopt, std::nullopt,
                         std::nullopt, std::nullopt,
                         std::nullopt};
  edges.push_back(e5);

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

/// \brief Tests that an edge in the right sequenceId order causes validation
/// to fail if its startNodeId and endNodeId do not match the nodeIds of
/// its neighbouring nodes
TEST_F(OrderGraphValidatorTest, ValidEdgesTest)
{
  nodes.push_back(n0_);

  e1_.start_node_id = "foo";
  e1_.end_node_id = "bar";
  edges.push_back(e1_);

  nodes.push_back(n2_);

  order_.order_id = "order0";
  order_.nodes = nodes;
  order_.edges = edges;

  vda5050_core::client::order::OrderGraphValidator order_graph_validator{
    order_};
  vda5050_core::client::order::ValidationResult res =
    order_graph_validator.is_valid_graph();

  EXPECT_FALSE(res.valid);
}

/// \brief Tests that an order with odd node sequenceIds and even edge
/// sequenceIds causes validation to fail
TEST_F(OrderGraphValidatorTest, NodeWithOddSequenceIdTest)
{
  vda5050_types::Node odd_node1{"oddNode1",   1,           true, {},
                                std::nullopt, std::nullopt};
  nodes.push_back(odd_node1);
  vda5050_types::Node odd_node2{"oddNode2",   3,           true, {},
                                std::nullopt, std::nullopt};
  nodes.push_back(odd_node2);

  vda5050_types::Edge even_edge{"evenEdge",   2,
                                "node4",      "node6",
                                true,         {},
                                std::nullopt, std::nullopt,
                                std::nullopt, std::nullopt,
                                std::nullopt, std::nullopt,
                                std::nullopt, std::nullopt,
                                std::nullopt, std::nullopt,
                                std::nullopt};
  edges.push_back(even_edge);

  order_.order_id = "order0";
  order_.nodes = nodes;
  order_.edges = edges;

  vda5050_core::client::order::OrderGraphValidator order_graph_validator{
    order_};
  vda5050_core::client::order::ValidationResult res =
    order_graph_validator.is_valid_graph();

  EXPECT_FALSE(res.valid);
}

/// \brief Tests that no two nodes share the same sequenceId
TEST_F(OrderGraphValidatorTest, DuplicateNodeSequenceIdTest)
{
  n2_.sequence_id = 0;

  nodes.push_back(n0_);
  nodes.push_back(n2_);

  edges.push_back(e1_);

  order_.order_id = "order0";
  order_.nodes = nodes;
  order_.edges = edges;

  vda5050_core::client::order::OrderGraphValidator order_graph_validator{
    order_};
  vda5050_core::client::order::ValidationResult res =
    order_graph_validator.is_valid_graph();

  EXPECT_FALSE(res.valid);
}

/// \brief Tests that there is only one base in the order
TEST_F(OrderGraphValidatorTest, MultipleBaseTest)
{
  /// n0_, e1_, n2_, and n4_ all released. Create a gap by setting e3_ to
  /// unreleased.
  e3_.released = false;

  nodes.push_back(n0_);
  nodes.push_back(n2_);
  nodes.push_back(n4_);

  edges.push_back(e1_);
  edges.push_back(e3_);

  order_.order_id = "order0";
  order_.nodes = nodes;
  order_.edges = edges;

  vda5050_core::client::order::OrderGraphValidator order_graph_validator{
    order_};
  vda5050_core::client::order::ValidationResult res =
    order_graph_validator.is_valid_graph();

  EXPECT_FALSE(res.valid);
}

/// \brief Tests that if an order with a base is received, its graph ends with a relased node
