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

#include "vda5050_execution/context/simple_context.hpp"

namespace vda5050_execution {

namespace context {

//=============================================================================
void SimpleContext::add_update(std::shared_ptr<core::UpdateBase> update)
{
  if (!update) return;

  std::lock_guard<std::mutex> lock(update_mutex_);
  updates_[update->get_type()] = update;
}

//=============================================================================
std::shared_ptr<core::UpdateBase> SimpleContext::get_update_raw(
  std::type_index type) const
{
  std::lock_guard<std::mutex> lock(update_mutex_);
  auto it = updates_.find(type);
  if (it != updates_.end()) return it->second;
  return nullptr;
}

//=============================================================================
void SimpleContext::clear_update_raw(std::type_index type)
{
  std::lock_guard<std::mutex> lock(update_mutex_);
  updates_.erase(type);
}

//=============================================================================
void SimpleContext::clear_all_updates()
{
  std::lock_guard<std::mutex> lock(update_mutex_);
  updates_.clear();
}

//=============================================================================
void SimpleContext::add_resource(std::shared_ptr<core::ResourceBase> resource)
{
  if (!resource) return;

  std::lock_guard<std::mutex> lock(resource_mutex_);
  resources_[resource->get_type()] = resource;
}

//=============================================================================
std::shared_ptr<core::ResourceBase> SimpleContext::get_resource_raw(
  std::type_index type) const
{
  std::lock_guard<std::mutex> lock(resource_mutex_);
  auto it = resources_.find(type);
  if (it != resources_.end()) return it->second;
  return nullptr;
}

//=============================================================================
void SimpleContext::clear_resource_raw(std::type_index type)
{
  std::lock_guard<std::mutex> lock(resource_mutex_);
  resources_.erase(type);
}

//=============================================================================
void SimpleContext::clear_all_resources()
{
  std::lock_guard<std::mutex> lock(resource_mutex_);
  resources_.clear();
}

}  // namespace context
}  // namespace vda5050_execution
