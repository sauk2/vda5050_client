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

#include <vda5050_core/logger/logger.hpp>

#include <vda5050_bt_execution/bt_execution/client_config.hpp>
#include <vda5050_bt_execution/bt_execution/execution_engine.hpp>

int main(int argc, char** argv)
{
  vda5050_bt_execution::ClientConfig config{
    "uagv",
    "v2",
    "ROS-I",
    "S001",
    "tcp://localhost:1883",
    std::chrono::seconds(10)};

  rclcpp::init(argc, argv);

  auto execution_engine =
    vda5050_bt_execution::ExecutionEngine::make_and_init(config);
  VDA5050_INFO("Starting VDA5050 Client...");

  for (int i = 0; i < 25; i++)
  {
    execution_engine->spin_once();
    execution_engine->sleep(std::chrono::milliseconds(1000));
  }

  execution_engine->shutdown();
  VDA5050_INFO("Shutdown VDA5050 Client");
}
