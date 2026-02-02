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

#ifndef VDA5050_EXECUTION__STRATEGY__NAVIGATION_STRATEGY_HPP_
#define VDA5050_EXECUTION__STRATEGY__NAVIGATION_STRATEGY_HPP_

#include <memory>

#include "vda5050_execution/core/context_interface.hpp"
#include "vda5050_execution/core/strategy_interface.hpp"
#include "vda5050_execution/strategy/navigation_delegate_interface.hpp"

namespace vda5050_execution {

namespace strategy {

class NavigationStrategy
: public core::StrategyInterface,
  public std::enable_shared_from_this<NavigationStrategy>
{
public:
  static std::shared_ptr<NavigationStrategy> make();

  void set_delegate(
    std::shared_ptr<strategy::NavigationDelegateInterface> delegate);

  void init(std::shared_ptr<core::ContextInterface> context) override;

  void step(std::shared_ptr<core::ContextInterface> context) override;

private:
  NavigationStrategy();

  std::shared_ptr<NavigationDelegateInterface> delegate_;
};

}  // namespace strategy
}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__STRATEGY__NAVIGATION_STRATEGY_HPP_
