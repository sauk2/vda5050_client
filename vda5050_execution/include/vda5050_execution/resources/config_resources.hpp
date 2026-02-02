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

#ifndef VDA5050_EXECUTION__RESOURCES__CONFIG_RESOURCES_HPP_
#define VDA5050_EXECUTION__RESOURCES__CONFIG_RESOURCES_HPP_

#include <chrono>
#include <string>

#include "vda5050_execution/core/base.hpp"

namespace vda5050_execution {

namespace resources {

struct MqttConfig
{
  std::string broker_address;
  std::chrono::seconds state_publish_period;
};

struct AGVIdentity
{
  std::string manufacturer;
  std::string serial_number;
  std::string interface_name;
  std::string protocol_version;
};

class Config : public core::Initialize<Config, core::ResourceBase>
{
public:
  Config(MqttConfig mqtt, AGVIdentity identity)
  : mqtt_config_(std::move(mqtt)), agv_identity_(std::move(identity))
  {
    // Nothing to do here ...
  }

  const MqttConfig& mqtt() const
  {
    return mqtt_config_;
  }

  const AGVIdentity& agv_identity() const
  {
    return agv_identity_;
  }

private:
  MqttConfig mqtt_config_;
  AGVIdentity agv_identity_;
};

}  // namespace resources
}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__RESOURCES__CONFIG_RESOURCES_HPP_
