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

#include "vda5050_bt_execution/bt_execution/execution_engine.hpp"
#include "vda5050_bt_execution/bt_nodes/monitor_connection.hpp"
#include "vda5050_bt_execution/bt_nodes/update_state.hpp"
#include "vda5050_bt_execution/bt_utils/state_publisher.hpp"

#include <vda5050_core/logger/logger.hpp>
#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>

#include <vda5050_json_utils/serialization.hpp>
#include <vda5050_types/connection.hpp>
#include <vda5050_types/header.hpp>

namespace vda5050_bt_execution {

//=============================================================================
std::shared_ptr<ExecutionEngine> ExecutionEngine::make()
{
  auto execution_engine =
    std::shared_ptr<ExecutionEngine>(new ExecutionEngine());
  return execution_engine;
}

//=============================================================================
std::shared_ptr<ExecutionEngine> ExecutionEngine::make_and_init()
{
  auto execution_engine =
    std::shared_ptr<ExecutionEngine>(new ExecutionEngine());
  execution_engine->initialize();
  return execution_engine;
}

//=============================================================================
void ExecutionEngine::initialize()
{
  context_->mqtt_client->connect();

  BT::BehaviorTreeFactory factory;

  factory.registerNodeType<MonitorConnection>("MonitorConnection");
  factory.registerNodeType<UpdateState>("UpdateState");

  const char* xml_text = R"(
<root BTCPP_format="4">
  <BehaviorTree ID="MainTree">
    <Parallel>
      <MonitorConnection context="{context}"/>
      <UpdateState context="{context}"/>
    </Parallel>
  </BehaviorTree>
</root>
)";

  tree_ = factory.createTreeFromText(xml_text);
  tree_.rootBlackboard()->set("context", context_);

  running_ = true;
}

//=============================================================================
void ExecutionEngine::spin()
{
  tree_.tickWhileRunning();
}

//=============================================================================
void ExecutionEngine::spin_once()
{
  tree_.tickOnce();
}

//=============================================================================
void ExecutionEngine::sleep(std::chrono::milliseconds period)
{
  tree_.sleep(period);
}

//=============================================================================
void ExecutionEngine::shutdown()
{
  tree_.haltTree();

  context_->mqtt_client->disconnect();
}

//=============================================================================
ExecutionEngine::ExecutionEngine()
: context_(std::make_shared<ExecutionContext>()), running_(false)
{
  context_->mqtt_client = vda5050_core::mqtt_client::create_default_client(
    "tcp://localhost:1883", "S001");

  vda5050_types::Connection connection_will;
  connection_will.connection_state =
    vda5050_types::ConnectionState::CONNECTIONBROKEN;
  nlohmann::json j = connection_will;
  context_->mqtt_client->set_will("vda5050/connection", j.dump(), 1);

  context_->state_publisher = StatePublisher::make(std::chrono::seconds(10));
  context_->state_publisher->set_publish_callback([ctx = context_]() {
    std::lock_guard<std::mutex> lock(ctx->state_mutex);

    if (ctx->mqtt_client && ctx->mqtt_client->connected())
    {
      vda5050_types::State state;
      nlohmann::json j = state;
      ctx->mqtt_client->publish("vda5050/state", j.dump(), 0);
      VDA5050_INFO("Published state");
    }
  });
  context_->state_publisher->request_event_publish();
}

}  // namespace vda5050_bt_execution
