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

#ifndef VDA5050_EXECUTION__BASE_EXECUTION_CONTEXT_HPP_
#define VDA5050_EXECUTION__BASE_EXECUTION_CONTEXT_HPP_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>

#include <vda5050_types/order.hpp>
#include <vda5050_types/state.hpp>

#include "vda5050_execution/client_config.hpp"
#include "vda5050_execution/execution_context_interface.hpp"
#include "vda5050_execution/protocol_adapter.hpp"

namespace vda5050_execution {

class BaseExecutionContext
: public ExecutionContextInterface,
  public std::enable_shared_from_this<BaseExecutionContext>
{
public:
  ~BaseExecutionContext();

  static std::shared_ptr<BaseExecutionContext> make(const ClientConfig& config);

  Segment get_next_segment() override;

  std::vector<std::shared_ptr<const vda5050_types::Action>>
  get_pending_actions() override;

  std::vector<std::shared_ptr<const vda5050_types::Action>>
  get_pending_instant_actions() override;

  void acknowledge_sequence_reached(const std::uint32_t seq_id) override;

  void update_action_status(
    const std::string& action_id, vda5050_types::ActionStatus status) override;

  void update_position(const vda5050_types::AGVPosition& position) override;

  void update_battery_state(
    const vda5050_types::BatteryState& battery) override;

  void update_operating_mode(vda5050_types::OperatingMode mode) override;

  void add_error(const vda5050_types::Error& error) override;

  void resolve_error(const std::string& error_type) override;

  void clear_errors() override;

  void request_state_publish();

  void shutdown();

private:
  explicit BaseExecutionContext(const ClientConfig& config);

  ClientConfig config_;
  std::shared_ptr<vda5050_core::mqtt_client::MqttClientInterface> mqtt_client_;
  std::shared_ptr<ProtocolAdapter> protocol_adapter_;

  std::mutex order_mutex_;
  std::shared_ptr<vda5050_types::Order> current_order_;

  std::mutex state_mutex_;
  std::condition_variable state_cv_;
  std::atomic_bool request_state_publish_;
  std::shared_ptr<vda5050_types::State> current_state_;
  std::thread state_update_thread_;

  std::atomic_bool shutdown_;
};

}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__BASE_EXECUTION_CONTEXT_HPP_
