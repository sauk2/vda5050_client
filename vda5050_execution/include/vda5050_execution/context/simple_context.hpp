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

#ifndef VDA5050_EXECUTION__CONTEXT__SIMPLE_CONTEXT_HPP_
#define VDA5050_EXECUTION__CONTEXT__SIMPLE_CONTEXT_HPP_

#include <memory>
#include <typeindex>
#include <unordered_map>

#include "vda5050_execution/core/base.hpp"
#include "vda5050_execution/core/context_interface.hpp"

namespace vda5050_execution {

namespace context {

class SimpleContext : public core::ContextInterface,
                      public std::enable_shared_from_this<SimpleContext>
{
public:
  std::shared_ptr<core::UpdateBase> get_update_raw(
    std::type_index type) const override;

  void add_update(std::shared_ptr<core::UpdateBase> update);

  void clear_update_raw(std::type_index type);

  void clear_all_updates();

  std::shared_ptr<core::ResourceBase> get_resource_raw(
    std::type_index type) const override;

  void add_resource(std::shared_ptr<core::ResourceBase> resource);

  void clear_resource_raw(std::type_index type);

  void clear_all_resources();

private:
  std::unordered_map<std::type_index, std::shared_ptr<core::UpdateBase>>
    updates_;
  mutable std::mutex update_mutex_;

  std::unordered_map<std::type_index, std::shared_ptr<core::ResourceBase>>
    resources_;
  mutable std::mutex resource_mutex_;
};

}  // namespace context
}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__CONTEXT__SIMPLE_CONTEXT_HPP_
