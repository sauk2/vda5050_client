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

#include "vda5050_execution/strategy/navigation_strategy.hpp"
#include "vda5050_execution/events/navigation_events.hpp"
#include "vda5050_execution/resources/order_resources.hpp"
#include "vda5050_execution/updates/navigation_updates.hpp"

namespace vda5050_execution {

namespace strategy {

//=============================================================================
std::shared_ptr<NavigationStrategy> NavigationStrategy::make()
{
  auto strategy = std::shared_ptr<NavigationStrategy>(new NavigationStrategy());
  return strategy;
}

//=============================================================================
void NavigationStrategy::set_delegate(
  std::shared_ptr<strategy::NavigationDelegateInterface> delegate)
{
  delegate_ = delegate;
}

//=============================================================================
void NavigationStrategy::init(std::shared_ptr<core::ContextInterface> context)
{
  auto provider = context->provider();

  provider->on<updates::SequenceAcknowledgement>(
    [w = std::weak_ptr<core::Engine>(this->engine())](auto update) {
      if (auto e = w.lock()) e->notify(update);
    });

  if (delegate_)
  {
    auto engine = this->engine();

    delegate_->set_provider(context->provider());

    engine->on<events::NavigationNodeReady>([d = delegate_](auto event) {
      d->on_navigation_node_ready(event->target_node, event->traversal_edge);
    });

    engine->on<events::NavigationStatusChange>([d = delegate_](auto event) {
      d->on_navigation_status_change(event->paused);
    });

    engine->on<events::NavigationReset>(
      [d = delegate_](auto /*event*/) { d->on_navigation_reset(); });
  }
}

//=============================================================================
void NavigationStrategy::step(std::shared_ptr<core::ContextInterface> context)
{
  auto order_manager = context->get_resource<resources::OrderManager>();
  if (!order_manager) return;

  engine()->step();

  if (engine()->waiting()) return;

  auto next = order_manager->get_next();
  if (next.has_value())
  {
    engine()->emit<events::NavigationNodeReady>(
      core::Priority::NORMAL,
      std::make_shared<vda5050_types::Node>(next->first),
      std::make_shared<vda5050_types::Edge>(next->second));
  }

  engine()->wait_for<updates::SequenceAcknowledgement>(
    std::chrono::seconds(30), [&next](auto update) {
      return update->sequence_id == next->first.sequence_id;
    });
}

//=============================================================================
NavigationStrategy::NavigationStrategy()
{
  // Nothing to do here ...
}

}  // namespace strategy
}  // namespace vda5050_execution
