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

#include "vda5050_execution/handler.hpp"

namespace vda5050_execution {

//=============================================================================
void Handler::add_strategy(std::shared_ptr<StrategyInterface> strategy)
{
  if (!strategy) return;

  std::lock_guard<std::mutex> lock(strategy_mutex_);
  strategies_.push_back(strategy);
  strategy->init(context_);
}

//=============================================================================
void Handler::remove_strategy(std::shared_ptr<StrategyInterface> strategy)
{
  std::lock_guard<std::mutex> lock(strategy_mutex_);
  auto it = std::find(strategies_.begin(), strategies_.end(), strategy);
  if (it != strategies_.end()) strategies_.erase(it);
}

//=============================================================================
void Handler::wake()
{
  std::lock_guard<std::mutex> lock(sync_mutex_);
  needs_processing_ = true;
  cv_.notify_all();
}

//=============================================================================
void Handler::spin(std::chrono::milliseconds timeout)
{
  running_ = true;

  while (running_)
  {
    std::unique_lock lock(sync_mutex_);
    cv_.wait_for(lock, timeout, [&] { return needs_processing_ || !running_; });

    if (!running_) break;

    needs_processing_ = false;
    lock.unlock();

    spin_once();
  }
}

//=============================================================================
void Handler::spin_once()
{
  {
    std::lock_guard<std::mutex> lock(sync_mutex_);
    needs_processing_ = false;
  }

  std::vector<std::shared_ptr<StrategyInterface>> active_strategies;
  {
    std::lock_guard<std::mutex> lock(strategy_mutex_);
    active_strategies = strategies_;
  }

  for (auto& strategy : active_strategies)
  {
    strategy->step(context_);
  }
}

//=============================================================================
bool Handler::running()
{
  return running_;
}

//=============================================================================
void Handler::stop()
{
  running_ = false;
}

}  // namespace vda5050_execution
