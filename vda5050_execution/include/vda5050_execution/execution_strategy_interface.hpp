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

#ifndef VDA5050_EXECUTION__EXECUTION_STRATEGY_INTERFACE_HPP_
#define VDA5050_EXECUTION__EXECUTION_STRATEGY_INTERFACE_HPP_

#include "vda5050_execution/execution_context_interface.hpp"
#include "vda5050_execution/execution_engine.hpp"

namespace vda5050_execution {

class ExecutionStrategyInterface
{
public:
  virtual ~ExecutionStrategyInterface() = default;

  virtual void step(std::shared_ptr<ExecutionContextInterface> context) = 0;

  ExecutionEngine& engine()
  {
    return engine_;
  };

protected:
  ExecutionEngine engine_;
};

}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__EXECUTION_STRATEGY_INTERFACE_HPP_
