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

#ifndef VDA5050_EXECUTION__RESOURCES__TRANSPORT_RESOURCES_HPP_
#define VDA5050_EXECUTION__RESOURCES__TRANSPORT_RESOURCES_HPP_

#include <memory>

#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>

#include "vda5050_execution/core/base.hpp"
#include "vda5050_execution/utils/protocol_adapter.hpp"

namespace vda5050_execution {

namespace resources {

class TransportHandler
: public core::Initialize<TransportHandler, core::ResourceBase>
{
public:
  TransportHandler(
    std::shared_ptr<vda5050_core::mqtt_client::MqttClientInterface> mqtt_client,
    std::shared_ptr<utils::ProtocolAdapter> adapter)
  : mqtt_client_(mqtt_client), adapter_(adapter)
  {
    // Nothing to do here ...
  }

  std::shared_ptr<utils::ProtocolAdapter> adapter() const
  {
    return adapter_;
  }

  std::shared_ptr<vda5050_core::mqtt_client::MqttClientInterface> mqtt_client()
    const
  {
    return mqtt_client_;
  }

private:
  std::shared_ptr<vda5050_core::mqtt_client::MqttClientInterface> mqtt_client_;
  std::shared_ptr<utils::ProtocolAdapter> adapter_;
};

}  // namespace resources
}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__RESOURCES__TRANSPORT_RESOURCES_HPP_
