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

#ifndef VDA5050_BT_EXECUTION__BT_UTILS__STATE_PUBLISHER_HPP_
#define VDA5050_BT_EXECUTION__BT_UTILS__STATE_PUBLISHER_HPP_

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>

namespace vda5050_bt_execution {

class StatePublisher : public std::enable_shared_from_this<StatePublisher>
{
public:
  using Clock = std::chrono::steady_clock;
  using TimePoint = std::chrono::steady_clock::time_point;

  static std::shared_ptr<StatePublisher> make(
    std::chrono::seconds period = std::chrono::seconds(30));

  void set_publish_callback(std::function<void()> callback);

  void request_event_publish();

  void tick();

private:
  explicit StatePublisher(std::chrono::seconds period);

  std::function<void()> publish_callback_;
  std::atomic_bool pending_event_publish_;
  std::chrono::seconds publish_period_;
  TimePoint last_publish_time_;
};

}  // namespace vda5050_bt_execution

#endif  // VDA5050_BT_EXECUTION__BT_UTILS__STATE_PUBLISHER_HPP_
