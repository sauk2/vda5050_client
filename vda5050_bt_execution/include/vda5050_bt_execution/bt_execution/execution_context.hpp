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

#ifndef VDA5050_BT_EXECUTION__BT_EXECUTION__EXECUTION_CONTEXT_HPP_
#define VDA5050_BT_EXECUTION__BT_EXECUTION__EXECUTION_CONTEXT_HPP_

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>

#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>
#include <vda5050_json_utils/serialization.hpp>
#include <vda5050_types/order.hpp>

#include "vda5050_bt_execution/bt_execution/client_config.hpp"
#include "vda5050_bt_execution/bt_execution/robot_adapter_interface.hpp"

namespace vda5050_bt_execution {

struct ExecutionContext
{
  std::shared_ptr<vda5050_core::mqtt_client::MqttClientInterface> mqtt_client;

  std::shared_ptr<const ClientConfig> client_config;

  std::mutex order_mutex;
  std::queue<std::shared_ptr<vda5050_types::Order>> incoming_order_queue;
  std::shared_ptr<vda5050_types::Order> current_order;
  std::vector<vda5050_types::Node>::iterator next_node;

  std::shared_ptr<RobotAdapterInterface> robot_adapter;

  std::atomic_bool request_state_publish;
};

}  // namespace vda5050_bt_execution

#endif  // VDA5050_BT_EXECUTION__BT_EXECUTION__EXECUTION_CONTEXT_HPP_
