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

#include <fmt/core.h>

#include <chrono>

#include <vda5050_core/logger/logger.hpp>
#include <vda5050_json_utils/serialization.hpp>
#include <vda5050_types/header.hpp>

#include "vda5050_execution/base_execution_context.hpp"

constexpr int CONNECTION_QOS = 1;
constexpr int DEFAULT_QOS = 0;

namespace vda5050_execution {

//=============================================================================
vda5050_types::Header create_header(
  uint32_t header_id, const ClientConfig& config)
{
  return vda5050_types::Header{
    header_id, std::chrono::system_clock::now(), config.version,
    config.manufacturer, config.serial_number};
}

//=============================================================================
std::string create_topic(std::string sub_topic, const ClientConfig& config)
{
  return fmt::format(
    "{}/{}/{}/{}/{}", config.interface, config.version, config.manufacturer,
    config.serial_number, sub_topic);
}

//=============================================================================
std::shared_ptr<BaseExecutionContext> BaseExecutionContext::make(
  const ClientConfig& config)
{
  auto context =
    std::shared_ptr<BaseExecutionContext>(new BaseExecutionContext(config));
  return context;
}

//=============================================================================
Segment BaseExecutionContext::get_next_segment() {}

//=============================================================================
void BaseExecutionContext::acknowledge_node_reached(const std::string& node_id)
{
}

//=============================================================================
std::vector<std::shared_ptr<const vda5050_types::Action>>
BaseExecutionContext::get_pending_actions()
{
}

//=============================================================================
std::vector<std::shared_ptr<const vda5050_types::Action>>
BaseExecutionContext::get_pending_instant_actions()
{
}

//=============================================================================
void BaseExecutionContext::update_action_status(
  const std::string& action_id, vda5050_types::ActionStatus status,
  const std::vector<vda5050_types::Error>& errors)
{
}

//=============================================================================
void BaseExecutionContext::add_error(const vda5050_types::Error& error) {}

//=============================================================================
void BaseExecutionContext::resolve_error(const std::string& error_type) {}

//=============================================================================
void BaseExecutionContext::clear_errors() {}

//=============================================================================
void BaseExecutionContext::request_state_publish()
{
  request_state_publish_ = true;
  state_cv_.notify_one();
}

//=============================================================================
BaseExecutionContext::BaseExecutionContext(const ClientConfig& config)
: config_(config),
  mqtt_client_(vda5050_core::mqtt_client::create_default_client(
    config.mqtt_broker_address, config.serial_number)),
  request_state_publish_(true)
{
  topic_names_ = {
    {MessageType::CONNECTION, "connection"},
    {MessageType::STATE, "state"},
    {MessageType::ORDER, "order"},
    {MessageType::INSTANT_ACTIONS, "instantActions"},
    {MessageType::FACTSHEET, "factsheet"},
    {MessageType::VISUALIZATION, "visualization"}};

  header_ids_ = {
    {MessageType::CONNECTION, 0}, {MessageType::STATE, 0},
    {MessageType::ORDER, 0},      {MessageType::INSTANT_ACTIONS, 0},
    {MessageType::FACTSHEET, 0},  {MessageType::VISUALIZATION, 0}};

  mqtt_client_->connect();

  mqtt_client_->subscribe(
    create_topic(topic_names_[MessageType::ORDER], config_),
    [w = weak_from_this()](
      const std::string& /*topic*/, const std::string& message) {
      if (auto c = w.lock())
      {
        std::lock_guard<std::mutex> lock(c->order_mutex_);
        vda5050_types::Order order = nlohmann::json::parse(message);
        c->current_order_ = std::make_shared<vda5050_types::Order>(order);
      }
    },
    DEFAULT_QOS);

  state_update_thread_ = std::thread([w = weak_from_this()] {
    while (true)
    {
      auto c = w.lock();
      if (!c || c->shutdown_) break;

      std::unique_lock<std::mutex> lock(c->state_mutex_);
      c->state_cv_.wait_for(lock, c->config_.state_publish_period, [&c] {
        return c->shutdown_ || c->request_state_publish_;
      });

      if (c->shutdown_) break;

      lock.unlock();

      try
      {
        auto header =
          create_header(c->header_ids_[MessageType::STATE]++, c->config_);
        c->current_state_->header = header;
        nlohmann::json j = *c->current_state_;

        c->mqtt_client_->publish(
          create_topic(c->topic_names_[MessageType::STATE], c->config_),
          j.dump(), DEFAULT_QOS);

        c->request_state_publish_ = false;
      }
      catch (const nlohmann::json::exception& e)
      {
      }
      catch (const std::exception& e)
      {
      }
      catch (...)
      {
      }
    }
  });
}

}  // namespace vda5050_execution
