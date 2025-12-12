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

#ifndef VDA5050_EXAMPLES__ROBOT_ADAPTER_HPP_
#define VDA5050_EXAMPLES__ROBOT_ADAPTER_HPP_

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include <vda5050_types/agv_position.hpp>
#include <vda5050_types/node.hpp>

#include <vda5050_bt_execution/bt_execution/robot_adapter_interface.hpp>

#include <rclcpp/rclcpp.hpp>

#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/pose_array.hpp>
#include <geometry_msgs/msg/quaternion.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

class RobotAdapter : public vda5050_bt_execution::RobotAdapterInterface,
                     public rclcpp::Node
{
public:
  using Pose = geometry_msgs::msg::Pose;
  using PoseArray = geometry_msgs::msg::PoseArray;
  using Point = geometry_msgs::msg::Point;
  using Quaternion = geometry_msgs::msg::Quaternion;

  static std::shared_ptr<RobotAdapter> make();

  void start();

  void move_to_node(
    const vda5050_types::Node& node,
    std::function<void()> done_callback) override;

  bool is_moving() override;

  vda5050_types::AGVPosition get_position() override;

  void shutdown();

private:
  RobotAdapter();

  std::atomic_bool moving_;

  rclcpp::Publisher<PoseArray>::SharedPtr cmd_pose_pub_;
  rclcpp::Subscription<Pose>::SharedPtr reached_pose_sub_;
  rclcpp::Subscription<PoseArray>::SharedPtr current_pose_sub_;

  std::thread rcl_spin_thread_;

  std::mutex position_mutex_;
  vda5050_types::AGVPosition current_position_;

  std::unordered_map<std::string, std::vector<double>> map_;

  std::function<void()> done_callback_;
};

#endif  // VDA5050_EXAMPLES__ROBOT_ADAPTER_HPP_

