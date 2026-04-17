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

#include <csignal>
#include <memory>
#include <mutex>
#include <thread>
#include <typeindex>
#include <unordered_map>

#include <vda5050_core/logger/logger.hpp>
#include <vda5050_core/mqtt_client/mqtt_client_interface.hpp>

#include <vda5050_types/order.hpp>
#include <vda5050_types/state.hpp>

#include "vda5050_execution/base.hpp"
#include "vda5050_execution/context_interface.hpp"
#include "vda5050_execution/handler.hpp"
#include "vda5050_execution/protocol_adapter.hpp"
#include "vda5050_execution/strategy_interface.hpp"

using vda5050_execution::ContextInterface;
using vda5050_execution::EventBase;
using vda5050_execution::Handler;
using vda5050_execution::Initialize;
using vda5050_execution::Priority;
using vda5050_execution::ProtocolAdapter;
using vda5050_execution::ResourceBase;
using vda5050_execution::StrategyInterface;
using vda5050_execution::UpdateBase;

std::atomic_bool running{true};

void signal_handler(int signal)
{
  VDA5050_INFO_STREAM(
    "System Signal [" << signal << "] received. Shutting down ...");
  running = false;
}

struct OrderUpdate : public Initialize<OrderUpdate, UpdateBase>
{
  vda5050_types::Order order;

  explicit OrderUpdate(const vda5050_types::Order& order)
  : order(std::move(order))
  {
    // Nothing to do here ...
  }
};

struct NodeDispatchEvent : public Initialize<NodeDispatchEvent, EventBase>
{
  uint32_t sequence_id;
  double x, y;
  std::optional<double> theta;

  NodeDispatchEvent(
    uint32_t sequence_id, double x, double y,
    std::optional<double> theta = std::nullopt)
  : sequence_id(sequence_id), x(x), y(y), theta(theta)
  {
    // Nothing to do here ...
  }
};

struct NodeAckUpdate : public Initialize<NodeAckUpdate, UpdateBase>
{
  uint32_t sequence_id;

  explicit NodeAckUpdate(uint32_t sequence_id) : sequence_id(sequence_id)
  {
    // Nothing to do here ...
  }
};

class NavigationStrategy : public StrategyInterface
{
public:
  void init(std::shared_ptr<ContextInterface> context) override
  {
    VDA5050_INFO("Initializing NavigationStrategy ...");

    engine()->on<NodeDispatchEvent>([](auto event) {
      VDA5050_INFO_STREAM(
        "Navigating to [" << event->x << ", " << event->y << "] "
                          << "with sequence " << event->sequence_id);
    });

    context->provider()->on<NodeAckUpdate>(
      [w = std::weak_ptr(engine())](auto update) {
        if (auto m = w.lock()) m->notify(update);
      });
  }

  void step(std::shared_ptr<ContextInterface> context) override
  {
    if (engine()->waiting()) return;

    if (nodes_.empty())
    {
      auto order_update = context->get_update<OrderUpdate>();
      if (order_update)
      {
        VDA5050_INFO("Adding nodes");
        nodes_ = order_update->order.nodes;
        current_idx_ = 0;
      }
      else
        return;
    }

    if (current_idx_ < nodes_.size())
    {
      auto target = nodes_[current_idx_++];
      engine()->emit<NodeDispatchEvent>(
        Priority::NORMAL, target.sequence_id, target.node_position.value().x,
        target.node_position.value().y,
        target.node_position.value().theta.value_or(0));

      VDA5050_INFO(
        "Pushing node with sequence_id [{}] to event queue",
        target.sequence_id);

      engine()->step();

      engine()->suspend<NodeAckUpdate>(
        [seq = target.sequence_id](auto update) -> bool {
          return update->sequence_id == seq;
        });
    }
    else
    {
      nodes_.clear();
    }
  }

private:
  std::vector<vda5050_types::Node> nodes_;
  size_t current_idx_ = 0;
};

class StateStrategy : public StrategyInterface
{
public:
  explicit StateStrategy(std::shared_ptr<ProtocolAdapter> protocol_adapter)
  : protocol_adapter_(protocol_adapter),
    last_pub_time_(std::chrono::steady_clock::now())
  {
    // Nothing to do here ...
  }

  void init(std::shared_ptr<ContextInterface> /*context*/) override
  {
    VDA5050_INFO("Initializing StateStrategy ...");
  }

  void step(std::shared_ptr<ContextInterface> context) override
  {
    auto now = std::chrono::steady_clock::now();
    if (now - last_pub_time_ >= std::chrono::seconds(30))
    {
    }
  }

private:
  std::shared_ptr<ProtocolAdapter> protocol_adapter_;
  std::chrono::steady_clock::time_point last_pub_time_;

  void publish_state() {}
};

class SimpleContext : public ContextInterface,
                      public std::enable_shared_from_this<SimpleContext>
{
public:
  void init() override
  {
    provider()->on<OrderUpdate>([w = weak_from_this()](auto update) {
      if (auto m = w.lock())
      {
        std::lock_guard<std::mutex> lock(m->mutex_);
        m->updates_[update->get_type()] = update;
      }
    });
  }

protected:
  std::shared_ptr<UpdateBase> get_update_raw(
    std::type_index type) const override
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return (updates_.count(type)) ? updates_.at(type) : nullptr;
  }

  std::shared_ptr<ResourceBase> get_resource_raw(
    std::type_index /*type*/) const override
  {
    return nullptr;
  }

private:
  std::unordered_map<std::type_index, std::shared_ptr<UpdateBase>> updates_;
  mutable std::mutex mutex_;
};

int main()
{
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  auto mqtt_client = vda5050_core::mqtt_client::create_default_client_unique(
    "tcp://localhost:1883", "vda5050_client");
  auto protocol_adapter = ProtocolAdapter::make(
    std::move(mqtt_client), "uagv", "2.0.0", "Manufacturer", "S001");

  vda5050_types::Connection connection_will;
  connection_will.connection_state =
    vda5050_types::ConnectionState::CONNECTIONBROKEN;
  protocol_adapter->set_will<vda5050_types::Connection>(
    connection_will, 1, true);

  protocol_adapter->connect();

  auto context = std::make_shared<SimpleContext>();
  auto navigation_strategy = std::make_shared<NavigationStrategy>();
  auto state_strategy = std::make_shared<StateStrategy>(protocol_adapter);
  auto handler = Handler::make(context, {navigation_strategy, state_strategy});

  protocol_adapter->subscribe<vda5050_types::Order>(
    [w = std::weak_ptr<ContextInterface>(context)](auto order, auto error) {
      if (error.has_value()) return;

      if (auto m = w.lock())
      {
        VDA5050_INFO("Received order with order_id: {}", order.order_id);
        m->provider()->push<OrderUpdate>(order);
      }
    },
    0);

  vda5050_types::Connection connection_online;
  connection_online.connection_state = vda5050_types::ConnectionState::ONLINE;
  protocol_adapter->publish<vda5050_types::Connection>(
    connection_online, 1, true);

  auto spin_thread = std::thread([&] { handler->spin(); });

  vda5050_types::Connection connection_offline;
  connection_offline.connection_state = vda5050_types::ConnectionState::OFFLINE;
  protocol_adapter->publish<vda5050_types::Connection>(
    connection_offline, 1, true);

  return 0;
}
