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

#include "vda5050_core/client/order/order_graph_validator.hpp"
#include "vda5050_core/client/order/validation_result.hpp"
#include "vda5050_types/edge.hpp"
#include "vda5050_types/node.hpp"
#include "vda5050_types/order.hpp"

class OrderGraphValidatorTest : public testing::Test
{
protected:
    vda5050_types::Node n1_{"node1", 1, true, {}, std::nullopt, std::nullopt};
    vda5050_types::Edge e2_{"edge2",      2,
                            "node1",      "node5",
                            true,         {},
                            std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt,
                            std::nullopt};
    vda5050_types::Node n3_{"node3", 3, true, {}, std::nullopt, std::nullopt};
    vda5050_types::Edge e4_{"edge4",      4,
                            "node1",      "node5",
                            true,         {},
                            std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt,
                            std::nullopt};
    vda5050_types::Node n5_{"node5", 5, true, {}, std::nullopt, std::nullopt};

    vda5050_types::Order order_{};
};

/// \brief Tests that graph validator returns true on a valid graph
TEST_F(OrderGraphValidatorTest, ValidGraphTest)
{
  std::vector<vda5050_types::Node> nodes;
  std::vector<vda5050_types::Edge> edges;

  nodes.push_back(n1_);
  edges.push_back(e2_);
  nodes.push_back(n3_);
  edges.push_back(e4_);
  nodes.push_back(n5_);

  order_.edges = edges;
  order_.nodes = nodes;

  vda5050_core::client::order::ValidationResult res = vda5050_core::client::order::OrderGraphValidator::is_valid_graph(order_);

  EXPECT_TRUE(res.valid);
}

// /// \brief Tests that graph validator returns false when nodes and edges are not in traversal order
TEST_F(OrderGraphValidatorTest, NotInTraversalOrderTest)
{
  std::vector<vda5050_types::Node> nodes;
  std::vector<vda5050_types::Edge> edges;

  nodes.push_back(n1_);
  edges.push_back(e4_);
  nodes.push_back(n3_);
  edges.push_back(e2_);
  nodes.push_back(n5_);

  order_.edges = edges;
  order_.nodes = nodes;

  vda5050_core::client::order::ValidationResult res = vda5050_core::client::order::OrderGraphValidator::is_valid_graph(order_);

  EXPECT_FALSE(res.valid);
}

///\brief Tests that graph validator returns false if the difference in number of nodes and edges is greater than one
TEST_F(OrderGraphValidatorTest, IncorrectNumberOfNodesAndEdgesTest)
{
  std::vector<vda5050_types::Node> nodes;
  std::vector<vda5050_types::Edge> edges;

  nodes.push_back(n1_);
  edges.push_back(e4_);
  nodes.push_back(n3_);
  edges.push_back(e2_);

  vda5050_types::Node n6 =
    vda5050_types::Node{"node6", 6, true, {}, std::nullopt, std::nullopt};
  nodes.push_back(n6);

  nodes.push_back(n5_);

  order_.nodes = nodes;
  order_.edges = edges;

  vda5050_core::client::order::ValidationResult res = vda5050_core::client::order::OrderGraphValidator::is_valid_graph(order_);

  EXPECT_FALSE(res.valid);
}
