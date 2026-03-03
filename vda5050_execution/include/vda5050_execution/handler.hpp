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

#ifndef VDA5050_EXECUTION__HANDLER_HPP_
#define VDA5050_EXECUTION__HANDLER_HPP_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <utility>
#include <vector>

#include "vda5050_execution/context_interface.hpp"
#include "vda5050_execution/strategy_interface.hpp"

namespace vda5050_execution {

class Handler : public std::enable_shared_from_this<Handler>
{
public:
  template <typename... Strategies>
  static std::shared_ptr<Handler> make(
    std::shared_ptr<ContextInterface> context,
    std::shared_ptr<Strategies>... strategies)
  {
    auto handler =
      std::shared_ptr<Handler>(new Handler(context, strategies...));
    return handler;
  }

  void add_strategy(std::shared_ptr<StrategyInterface> strategy);

  void remove_strategy(std::shared_ptr<StrategyInterface> strategy);

  template <typename StrategyT>
  void remove_strategy_by_type()
  {
    std::lock_guard<std::mutex> lock(strategy_mutex_);
    strategies_.erase(
      std::remove_if(
        strategies_.begin(), strategies_.end(),
        [](const auto& s) {
          return std::dynamic_pointer_cast<StrategyT>(s) != nullptr;
        }),
      strategies_.end());
  }

  void wake();

  void spin(std::chrono::milliseconds timeout = std::chrono::milliseconds(100));

  void spin_once();

  bool running();

  void stop();

private:
  template <typename... Strategies>
  Handler(
    std::shared_ptr<ContextInterface> context,
    std::shared_ptr<Strategies>... strategies)
  : context_(std::move(context)), running_(false), needs_processing_(true)
  {
    context_->init();

    context->on_change([&] { wake(); });

    (add_strategy(strategies), ...);
  }

  std::shared_ptr<ContextInterface> context_;

  std::vector<std::shared_ptr<StrategyInterface>> strategies_;
  std::mutex strategy_mutex_;

  std::atomic_bool running_;

  std::atomic_bool needs_processing_;
  std::condition_variable cv_;
  std::mutex sync_mutex_;
};

}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__HANDLER_HPP_
