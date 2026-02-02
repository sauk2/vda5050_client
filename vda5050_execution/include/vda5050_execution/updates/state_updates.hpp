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

#ifndef VDA5050_EXECUTION__UPDATES__STATE_UPDATES_HPP_
#define VDA5050_EXECUTION__UPDATES__STATE_UPDATES_HPP_

#include <memory>
#include <utility>

#include <vda5050_types/agv_position.hpp>
#include <vda5050_types/battery_state.hpp>
#include <vda5050_types/operating_mode.hpp>

#include "vda5050_execution/core/base.hpp"

namespace vda5050_execution {

namespace updates {

struct PositionData : public core::Initialize<PositionData, core::UpdateBase>
{
  std::shared_ptr<vda5050_types::AGVPosition> agv_position;

  explicit PositionData(std::shared_ptr<vda5050_types::AGVPosition> position)
  : agv_position(std::move(position))
  {
    // Nothing to do here ...
  }
};

struct BatteryData : public core::Initialize<BatteryData, core::UpdateBase>
{
  std::shared_ptr<vda5050_types::BatteryState> battery_state;

  explicit BatteryData(std::shared_ptr<vda5050_types::BatteryState> battery)
  : battery_state(battery)
  {
    // Nothing to do here ...
  }
};

struct OperatingModeData
: public core::Initialize<OperatingModeData, core::UpdateBase>
{
  vda5050_types::OperatingMode operating_mode;

  explicit OperatingModeData(vda5050_types::OperatingMode mode)
  : operating_mode(mode)
  {
    // Nothing to do here ...
  }
};

}  // namespace updates
}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__UPDATES__STATE_UPDATES_HPP_
