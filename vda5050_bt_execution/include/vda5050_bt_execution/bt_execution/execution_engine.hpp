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

#ifndef VDA5050_BT_EXECUTION__BT_EXECUTION__EXECUTION_ENGINE_HPP_
#define VDA5050_BT_EXECUTION__BT_EXECUTION__EXECUTION_ENGINE_HPP_

#include <behaviortree_cpp/bt_factory.h>

#include <atomic>
#include <chrono>
#include <memory>

#include "vda5050_bt_execution/bt_execution/client_config.hpp"
#include "vda5050_bt_execution/bt_execution/execution_context.hpp"

namespace vda5050_bt_execution {

class ExecutionEngine : public std::enable_shared_from_this<ExecutionEngine>
{
public:
  static std::shared_ptr<ExecutionEngine> make(const ClientConfig& config);

  static std::shared_ptr<ExecutionEngine> make_and_init(
    const ClientConfig& config);

  void initialize();

  void spin();

  void spin_once();

  void sleep(std::chrono::milliseconds period);

  void shutdown();

private:
  ExecutionEngine(const ClientConfig& config);

  BT::Tree tree_;
  std::shared_ptr<ExecutionContext> context_;
  std::atomic_bool running_;
};

}  // namespace vda5050_bt_execution

#endif  // VDA5050_BT_EXECUTION__BT_EXECUTION__EXECUTION_ENGINE_HPP_
