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

#include "vda5050_bt_execution/bt_utils/state_publisher.hpp"

namespace vda5050_bt_execution {

//=============================================================================
std::shared_ptr<StatePublisher> StatePublisher::make(
  std::chrono::seconds period)
{
  auto state_publisher =
    std::shared_ptr<StatePublisher>(new StatePublisher(period));
  return state_publisher;
}

//=============================================================================
void StatePublisher::set_publish_callback(std::function<void()> callback)
{
  publish_callback_ = std::move(callback);
}

//=============================================================================
void StatePublisher::request_event_publish()
{
  pending_event_publish_ = true;
}

//=============================================================================
void StatePublisher::tick()
{
  auto now = Clock::now();

  if (pending_event_publish_)
  {
    if (publish_callback_) publish_callback_();
    pending_event_publish_ = false;
    return;
  }

  if (now - last_publish_time_ >= publish_period_)
  {
    if (publish_callback_) publish_callback_();
    last_publish_time_ = now;
    return;
  }
}

//=============================================================================
StatePublisher::StatePublisher(std::chrono::seconds period)
: pending_event_publish_(false),
  publish_period_(period),
  last_publish_time_(Clock::now())
{
  // Nothing to do here ...
}

}  // namespace vda5050_bt_execution
