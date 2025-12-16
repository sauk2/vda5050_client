# vda5050_examples

This repository contains several libraries to create VDA5050 components along
with execution. The goal is to let robot OEMs and system integrators focus on
robot behaviour while all logic is handled by us.

## Example: Running in Gazebo

This package includes an example launch file that demonstrates how to run an
example simulation.

### Prereqisites

- ROS 2 Rolling
- Gazebo Jetty (installed through ROS 2 vendor packages)
- `ros_gz_bridge` (installed through ROS 2 vendor packages)
- MQTT Broker (e.g., Mosquitto)

Launch the simulation using the following command,

```bash
ros2 launch vda5050_examples warehouse.launch.py
```

The package also contains a mock master control that can be run using the
following command,

```bash
ros2 run vda5050_examples master_control
```

## Creating your own robot adapter

To integrate a real or simulated robot, a robot adapter can be implemented
as follows,

```cpp
#include <vda5050_bt_execution/bt_execution/robot_adapter_interface.hpp>


class RobotAdapter : public vda5050_bt_execution::RobotAdapterInterface
{
public:
  RobotAdapter()
  {
    // Initialize robot-specific resources here
  }

  void move_to_node(
    const vda5050_types::Node& node,
    std::function<void()> done_callback) override
  {
    // Called by the execution engine when a node must be executed
    //
    // Use the information in vda5050_types::Node to trigger navigation
    //
    // IMPORTANT:
    // The done_callback MUST be invoked once the robot has
    // successfully completed the navigation
  }

  bool is_moving() override
  {
    // Return true while the robot is executing motion
    // Return false once the robot is idle
  }

  vda5050_types::AGVPosition get_position() override
  {
    // Return the robot's current position
    //
    // This method requires the user to always indicate the status of robot
    // movement
  }
};
```

Pass this adapter to the execution engine along with the configuration,

```cpp

#include <vda5050_bt_execution/bt_execution/client_config.hpp>
#include <vda5050_bt_execution/bt_execution/execution_engine.hpp>

// Create client configuration
vda5050_bt_execution::ClientConfig config{
  "uagv",                      // AGV ID
  "v2",                        // VDA5050 protocol version
  "ROS-I",                     // Manufacturer
  "S001",                      // Serial number
  "tcp://localhost:1883",      // MQTT broker address
  std::chrono::seconds(10)};   // State publish interval

// Construct the adapter
auto robot_adapter = std::shared_ptr<RobotAdapter>(new RobotAdapter());
robot_adapter->start();

// Create and initialize the execution engine
auto execution_engine =
  vda5050_bt_execution::ExecutionEngine::make_and_init(config, robot_adapter);

// Main execution loop
while (!shutdown)
{
  execution_engine->spin_once();
  execution_engine->sleep(std::chrono::milliseconds(1000));
}

// Graceful shutdown
execution_engine->shutdown();
```
