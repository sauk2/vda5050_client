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

#ifndef VDA5050_EXECUTION__EXECUTION_HANDLER_HPP_
#define VDA5050_EXECUTION__EXECUTION_HANDLER_HPP_

#include <atomic>
#include <memory>

#include <vda5050_types/edge.hpp>
#include <vda5050_types/node.hpp>

#include "vda5050_execution/client_config.hpp"
#include "vda5050_execution/execution_context_interface.hpp"
#include "vda5050_execution/execution_delegate_interface.hpp"
#include "vda5050_execution/execution_strategy_interface.hpp"

namespace vda5050_execution {

class ExecutionHandler : public std::enable_shared_from_this<ExecutionHandler>
{
public:
  ~ExecutionHandler();

  static std::shared_ptr<ExecutionHandler> make(
    std::shared_ptr<ExecutionContextInterface> context,
    std::shared_ptr<ExecutionStrategyInterface> strategy,
    std::shared_ptr<ExecutionDelegateInterface> delegate);

  void spin();

  void spin_once();

  void shutdown();

private:
  ExecutionHandler(
    std::shared_ptr<ExecutionContextInterface> context,
    std::shared_ptr<ExecutionStrategyInterface> strategy,
    std::shared_ptr<ExecutionDelegateInterface> delegate);

  std::shared_ptr<ExecutionContextInterface> context_;
  std::shared_ptr<ExecutionStrategyInterface> strategy_;
  std::shared_ptr<ExecutionDelegateInterface> delegate_;

  std::atomic_bool shutdown_;
};

}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__EXECUTION_HANDLER_HPP_
