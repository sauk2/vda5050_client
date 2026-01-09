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

#include "vda5050_execution/sequential_execution_strategy.hpp"
#include "vda5050_execution/execution_engine.hpp"

namespace vda5050_execution {

SequentialExecutionStrategy::~SequentialExecutionStrategy()
{
  shutdown();
}

//=============================================================================
std::shared_ptr<SequentialExecutionStrategy> SequentialExecutionStrategy::make()
{
  auto strategy = std::shared_ptr<SequentialExecutionStrategy>(
    new SequentialExecutionStrategy());
  return strategy;
}

//=============================================================================
void SequentialExecutionStrategy::step(
  std::shared_ptr<ExecutionContextInterface> context)
{
  if (auto segment = context->get_next_segment())
  {
    auto target_node = segment.nodes.front();
    auto traversal_edge =
      segment.edges.empty() ? nullptr : segment.edges.front();
    engine_.emit<NavigationNodeReady>(target_node, traversal_edge);
  }
  // auto node = std::make_shared<vda5050_types::Node>(vda5050_types::Node{});
  // node->node_id = "N1";
  // auto edge = std::make_shared<vda5050_types::Edge>(vda5050_types::Edge{});
  // edge->edge_id = "E1";

  // auto paused = true;

  // engine_.emit<NavigationNodeReady>(node, edge);
  // engine_.emit<NavigationStatusChange>(paused);

  // engine_.step();

  steps++;
}

//=============================================================================
void SequentialExecutionStrategy::shutdown()
{
  shutdown_ = true;
}

//=============================================================================
SequentialExecutionStrategy::SequentialExecutionStrategy()
: steps(0), shutdown_(false)
{
  // Nothing to do here ...
}

}  // namespace vda5050_execution
