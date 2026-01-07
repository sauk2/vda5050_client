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

#ifndef VDA5050_EXECUTION__SEQUENTIAL_EXECUTION_STRATEGY_HPP_
#define VDA5050_EXECUTION__SEQUENTIAL_EXECUTION_STRATEGY_HPP_

#include <memory>

#include "vda5050_execution/execution_context_interface.hpp"
#include "vda5050_execution/execution_strategy_interface.hpp"

namespace vda5050_execution {

class SequentialExecutionStrategy : public ExecutionStrategyInterface
{
public:
  static std::shared_ptr<SequentialExecutionStrategy> make();

  void step(std::shared_ptr<ExecutionContextInterface> context) override;

private:
  SequentialExecutionStrategy();

  int steps;
};

}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__SEQUENTIAL_EXECUTION_STRATEGY_HPP_
