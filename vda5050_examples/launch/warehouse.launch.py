# Copyright 2025 ROS-Industrial Consortium Asia Pacific
# Advanced Remanufacturing and Technology Centre
# A*STAR Research Entities (Co. Registration No. 199702110H)
# Copyright 2025 Kabam Pte Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
import rclpy.logging

logger = rclpy.logging.get_logger('root')


def generate_launch_description():
    # Get package paths
    pkg_vda5050_examples = get_package_share_directory('vda5050_examples')
    pkg_ros_gz_sim = get_package_share_directory('ros_gz_sim')

    # Set up world for gazebo launch
    gz_sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(pkg_ros_gz_sim, 'launch', 'gz_sim.launch.py')),
        launch_arguments={'gz_args': [' -r ', PathJoinSubstitution([
            pkg_vda5050_examples,
            'worlds',
            'warehouse.sdf'
        ])]}.items(),)

    # ROS 2 <-> GZ bridge
    bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        parameters=[{
            'config_file': os.path.join(
                pkg_vda5050_examples, 'config', 'ros_gz_interface_bridge.yaml')
        }],
        output='screen')

    # AGV Interface node to connect to VDA5050++
    agv_client = Node(
        package='vda5050_examples',
        executable='bt_execution_main',
        output='screen')

    return LaunchDescription([
        gz_sim,
        bridge,
        agv_client
    ])
