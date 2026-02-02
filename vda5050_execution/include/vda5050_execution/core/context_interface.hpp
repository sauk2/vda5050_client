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

#ifndef VDA5050_EXECUTION__CORE__CONTEXT_INTERFACE_HPP_
#define VDA5050_EXECUTION__CORE__CONTEXT_INTERFACE_HPP_

#include <memory>
#include <type_traits>
#include <typeindex>

#include "vda5050_execution/core/base.hpp"
#include "vda5050_execution/core/provider.hpp"

namespace vda5050_execution {

namespace core {

class ContextInterface
{
public:
  ContextInterface() : provider_(std::make_shared<Provider>())
  {
    // Nothing to do here ...
  }

  virtual ~ContextInterface() = default;

  virtual void init() = 0;

  template <typename UpdateT>
  std::shared_ptr<UpdateT> get_update() const
  {
    static_assert(
      std::is_base_of_v<UpdateBase, UpdateT>,
      "Update must be derived from UpdateBase");

    return std::static_pointer_cast<UpdateT>(
      get_update_raw(std::type_index(typeid(UpdateT))));
  }

  template <typename ResourceT>
  std::shared_ptr<ResourceT> get_resource() const
  {
    static_assert(
      std::is_base_of_v<ResourceBase, ResourceT>,
      "Resource must be derived from ResourceBase");

    return std::static_pointer_cast<ResourceT>(
      get_resource_raw(std::type_index(typeid(ResourceT))));
  }

  std::shared_ptr<Provider> provider()
  {
    return provider_;
  }

protected:
  virtual std::shared_ptr<UpdateBase> get_update_raw(
    std::type_index type) const = 0;

  virtual std::shared_ptr<ResourceBase> get_resource_raw(
    std::type_index type) const = 0;

private:
  std::shared_ptr<Provider> provider_;
};

}  // namespace core
}  // namespace vda5050_execution

#endif  // VDA5050_EXECUTION__CORE__CONTEXT_INTERFACE_HPP_
