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

#include <atomic>

#include <vda5050_core/logger/logger.hpp>
#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>
#include <vda5050_json_utils/serialization.hpp>
#include <vda5050_types/header.hpp>
#include <vda5050_types/order.hpp>

int main()
{
  auto mqtt_client = vda5050_core::mqtt_client::create_default_client(
    "tcp://localhost:1883", "master");

  std::atomic_bool connected = false;
  std::string topic_prefix = "uagv/v2/ROS-I/S001/";
  vda5050_types::Header header;

  mqtt_client->connect();
  if (mqtt_client->connected()) VDA5050_INFO("Connected ...");

  mqtt_client->subscribe(
    topic_prefix + "connection",
    [&connected, &header](
      const std::string& /*topic*/, const std::string& msg) {
      VDA5050_INFO("Received connection request from S001");
      auto j = nlohmann::json::parse(msg);
      vda5050_types::Connection connection = j;
      header = connection.header;

      connected = true;
    },
    1);

  while (!connected);

  // vda5050_types::Order order;

  // nlohmann::json j = order;
  // mqtt_client->publish(topic_prefix + "order", j.dump(), 0);

  // VDA5050_INFO("Published order to S001");

  return 0;
}
