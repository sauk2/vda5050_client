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

#include "vda5050_examples/robot_adapter.hpp"

//=============================================================================
std::shared_ptr<RobotAdapter> RobotAdapter::make()
{
  auto robot_adapter = std::shared_ptr<RobotAdapter>(new RobotAdapter());

  auto qos = rclcpp::SystemDefaultsQoS().reliable().durability_volatile();

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

  robot_adapter->current_pose_sub_ =
    robot_adapter->create_subscription<PoseArray>(
      "pose", qos,
      [w = std::weak_ptr<RobotAdapter>(robot_adapter)](const PoseArray& msg) {
        if (auto m = w.lock())
        {
          std::lock_guard<std::mutex> lock(m->position_mutex_);

          tf2::Quaternion orientation_out;
          tf2::fromMsg(msg.poses.at(0).orientation, orientation_out);
          tf2::Matrix3x3 rotation(orientation_out);
          [[maybe_unused]]
          double roll,
            pitch;
          double yaw;
          rotation.getRPY(roll, pitch, yaw);

          vda5050_types::AGVPosition position;
          position.x = msg.poses.at(0).position.x;
          position.y = msg.poses.at(0).position.y;
          position.theta = yaw;
          position.position_initialized = true;

          m->current_position_ = position;
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
void RobotAdapter::move_to_node(const vda5050_types::Node& node)
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
bool RobotAdapter::is_moving()
{
  return this->moving_;
}

//=============================================================================
vda5050_types::AGVPosition RobotAdapter::get_position()
{
  std::lock_guard<std::mutex> lock(position_mutex_);
  return current_position_;
}

//=============================================================================
void RobotAdapter::shutdown()
{
  rclcpp::shutdown();
  if (rcl_spin_thread_.joinable()) rcl_spin_thread_.join();
}

//=============================================================================
RobotAdapter::RobotAdapter() : rclcpp::Node("robot_adapter")
{
  // Nothing to do here ...
}
