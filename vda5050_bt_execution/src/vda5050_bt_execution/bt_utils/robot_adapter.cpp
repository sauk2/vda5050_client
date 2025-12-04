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

#include "vda5050_bt_execution/bt_utils/robot_adapter.hpp"

namespace vda5050_bt_execution {

//=============================================================================
std::shared_ptr<RobotAdapter> RobotAdapter::make()
{
  auto robot_adapter = std::shared_ptr<RobotAdapter>(new RobotAdapter());

  auto qos = rclcpp::SystemDefaultsQoS().reliable().transient_local();

  robot_adapter->cmd_pose_pub_ =
    robot_adapter->create_publisher<PoseArray>("cmd_pose", qos);

  robot_adapter->reached_pose_sub_ = robot_adapter->create_subscription<Pose>(
    "reached_pose", qos,
    [w = std::weak_ptr<RobotAdapter>(robot_adapter)](const Pose& /*msg*/) {
      if (auto m = w.lock())
      {
        m->moving_ = false;
      }
    });

  return robot_adapter;
}

//=============================================================================
void RobotAdapter::start()
{
  rcl_spin_thread_ = std::thread([w = weak_from_this()] {
    if (auto m = w.lock()) rclcpp::spin(m);
  });
}

//=============================================================================
void RobotAdapter::move(const vda5050_types::Node& node)
{
  if (moving_) return;

  Point point;
  point.x = node.node_position.value().x;
  point.y = node.node_position.value().y;

  tf2::Quaternion quaternion;
  quaternion.setRPY(0.0, 0.0, node.node_position.value().theta.value_or(0.0));
  Quaternion orientation = tf2::toMsg(quaternion);

  auto pose =
    geometry_msgs::build<Pose>().position(point).orientation(orientation);
  PoseArray msg;
  msg.poses.push_back(pose);
  this->cmd_pose_pub_->publish(msg);

  moving_ = true;
}

//=============================================================================
bool RobotAdapter::moving()
{
  return this->moving_;
}

//=============================================================================
void RobotAdapter::shutdown()
{
  rclcpp::shutdown();
  if (rcl_spin_thread_.joinable()) rcl_spin_thread_.join();
}

//=============================================================================
RobotAdapter::RobotAdapter() : Node("robot_adapter")
{
  // Nothing to do here ...
}

}  // namespace vda5050_bt_execution
