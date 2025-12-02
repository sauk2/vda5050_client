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

#ifndef VDA5050_BT_EXECUTION__BT_NODES__UPDATE_ORDER_HPP_
#define VDA5050_BT_EXECUTION__BT_NODES__UPDATE_ORDER_HPP_

#include <behaviortree_cpp/bt_factory.h>

#include <string>

namespace vda5050_bt_execution {

class UpdateOrder : public BT::SyncActionNode
{
public:
  UpdateOrder(const std::string& name, const BT::NodeConfig& config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;
};

}  // namespace vda5050_bt_execution

#endif  // VDA5050_BT_EXECUTION__BT_NODES__UPDATE_ORDER_HPP_
