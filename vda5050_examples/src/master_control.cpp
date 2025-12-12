/*
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

#include <atomic>
#include <chrono>
#include <string>

#include <vda5050_bt_execution/bt_execution/client_config.hpp>
#include <vda5050_core/logger/logger.hpp>
#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>
#include <vda5050_json_utils/serialization.hpp>
#include <vda5050_types/header.hpp>
#include <vda5050_types/order.hpp>

int main()
{
  vda5050_bt_execution::ClientConfig config{
    "uagv",
    "v2",
    "ROS-I",
    "S001",
    "tcp://localhost:1883",
    std::chrono::seconds(10)};

  auto mqtt_client = vda5050_core::mqtt_client::create_default_client(
    config.mqtt_broker_address, "master");

  std::atomic_bool connected = false;

  std::string topic_prefix = fmt::format(
    "{}/{}/{}/{}/", config.interface, config.version, config.manufacturer,
    config.serial_number);

  mqtt_client->connect();
  if (mqtt_client->connected()) VDA5050_INFO("Connected ...");

  mqtt_client->subscribe(
    topic_prefix + "connection",
    [&connected](const std::string& /*topic*/, const std::string& msg) {
      auto j = nlohmann::json::parse(msg);
      vda5050_types::Connection connection = j;

      VDA5050_INFO(
        "Received connection request from {}", connection.header.serial_number);

      if (connection.connection_state == vda5050_types::ConnectionState::ONLINE)
        connected = true;
    },
    1);

  while (!connected);

  vda5050_types::Header header;
  header.timestamp = std::chrono::system_clock::now();
  header.version = config.version;
  header.manufacturer = config.manufacturer;
  header.serial_number = config.serial_number;

  vda5050_types::Order order;
  order.header = header;
  order.order_id = "test_order";
  order.order_update_id = 0;

  std::vector<std::string> seq_list = {"N1", "E1", "N2", "E2",
                                       "N3", "E3", "N4"};
  for (uint32_t i = 0; i < seq_list.size(); i++)
  {
    if (i % 2 == 0)
    {
      vda5050_types::Node node;
      node.node_id = seq_list[i];
      node.sequence_id = i;
      node.released = true;

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

  nlohmann::json j = order;
  VDA5050_INFO("{}", j.dump(4));
  mqtt_client->publish(topic_prefix + "order", j.dump(), 0);

  VDA5050_INFO("Published order to {}", config.serial_number);

  return 0;
}
