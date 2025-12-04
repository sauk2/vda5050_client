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

#ifndef VDA5050_BT_EXECUTION__BT_EXECUTION__CLIENT_CONFIG_HPP_
#define VDA5050_BT_EXECUTION__BT_EXECUTION__CLIENT_CONFIG_HPP_

#include <chrono>
#include <string>

namespace vda5050_bt_execution {

struct ClientConfig
{
  std::string interface;

  std::string version;

  std::string manufacturer;

  std::string serial_number;

  std::string mqtt_broker_address;

  std::chrono::seconds state_publish_period;
};

}  // namespace vda5050_bt_execution

#endif  // VDA5050_BT_EXECUTION__BT_EXECUTION__CLIENT_CONFIG_HPP_
