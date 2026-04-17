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

#include <atomic>
#include <string>

#include <vda5050_core/logger/logger.hpp>
#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>

#include <vda5050_types/connection.hpp>
#include <vda5050_types/edge.hpp>
#include <vda5050_types/node.hpp>
#include <vda5050_types/node_position.hpp>
#include <vda5050_types/order.hpp>

#include "vda5050_execution/protocol_adapter.hpp"

using vda5050_execution::ProtocolAdapter;

int main()
{
  auto mqtt_client = vda5050_core::mqtt_client::create_default_client_unique(
    "tcp://localhost:1883", "mock_master_control");
  auto protocol_adapter = ProtocolAdapter::make(
    std::move(mqtt_client), "uagv", "2.0.0", "Manufacturer", "S001");

  std::atomic_bool connected = false;

  protocol_adapter->connect();

  protocol_adapter->subscribe<vda5050_types::Connection>(
    [&connected](auto message, auto error) {
      if (error.has_value()) return;

      VDA5050_INFO(
        "Received connection request from {}", message.header.serial_number);

      if (message.connection_state == vda5050_types::ConnectionState::ONLINE)
        connected = true;
    },
    0);

  while (!connected) continue;

  vda5050_types::Order order;
  order.order_id = "test_order";
  order.order_update_id = 0;

  std::vector<std::string> seq_list = {"N1", "E1", "N2", "E2",
                                       "N3", "E3", "N4"};
  for (uint32_t i = 0; i < seq_list.size(); i++)
  {
    if (i % 2 == 0)
    {
      vda5050_types::NodePosition node_position;
      node_position.x = static_cast<double>(i) * 1.0;
      node_position.y = static_cast<double>(i) * 0.5;
      node_position.theta = 0.0;
      node_position.map_id = "map_1";

      vda5050_types::Node node;
      node.node_id = seq_list[i];
      node.sequence_id = i;
      node.released = true;
      node.node_position = node_position;

      order.nodes.push_back(node);
    }
    else
    {
      vda5050_types::Edge edge;
      edge.edge_id = seq_list[i];
      edge.sequence_id = i;
      edge.start_node_id = seq_list[i - 1];
      edge.end_node_id = seq_list[i + 1];
      edge.released = true;

      order.edges.push_back(edge);
    }
  }

  protocol_adapter->publish<vda5050_types::Order>(order, 0);

  VDA5050_INFO("Published order to S001");

  return 0;
}
