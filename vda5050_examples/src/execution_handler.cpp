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

#include <iostream>
#include <memory>

#include "vda5050_execution/execution_delegate_interface.hpp"

class CustomExecutionDelegate
: public vda5050_execution::ExecutionDelegateInterface
{
public:
  void on_navigation_node_ready(
    std::shared_ptr<const vda5050_types::Node> target_node,
    std::shared_ptr<const vda5050_types::Edge> traversal_edge) override
  {
    std::cout << "node_id: " << target_node->node_id << std::endl;
    std::cout << "edge_id: " << traversal_edge->edge_id << std::endl;
  }

  void on_navigation_status_change(bool pause = false) override
  {
    std::cout << "pause requested: " << pause << std::endl;
  }
};

int main() {}
