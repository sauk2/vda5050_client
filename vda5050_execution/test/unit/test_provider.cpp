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

#include <gmock/gmock.h>

#include <vda5050_types/agv_position.hpp>
#include <vda5050_types/battery_state.hpp>

#include "vda5050_execution/base.hpp"
#include "vda5050_execution/provider.hpp"

namespace {

using namespace vda5050_execution;  // NOLINT

struct UpdateA : public Initialize<UpdateA, UpdateBase>
{};

struct UpdateB : public Initialize<UpdateB, UpdateBase>
{};

struct MockBatteryStatus : public Initialize<MockBatteryStatus, UpdateBase>
{
  double percentage;
  bool charging;

  MockBatteryStatus(double percentage, bool charging)
  : percentage(percentage), charging(charging)
  {
    // Nothing to do here ...
  }
};

struct MockNavigationStatus
: public Initialize<MockNavigationStatus, UpdateBase>
{
  uint32_t sequence_id;

  explicit MockNavigationStatus(uint32_t sequence_id) : sequence_id(sequence_id)
  {
    // Nothing to do here ...
  }
};

};  // namespace

class ProviderTest : public ::testing::Test
{
protected:
  std::shared_ptr<vda5050_execution::Provider> provider =
    std::make_shared<vda5050_execution::Provider>();
};

TEST_F(ProviderTest, SingleUpdatePush)
{
  int call_count = 0;
  int wront_type_count = 0;

  provider->on<UpdateA>([&](std::shared_ptr<UpdateA>) { call_count++; });
  provider->on<UpdateB>([&](std::shared_ptr<UpdateB>) { wront_type_count++; });

  auto update = std::make_shared<UpdateA>();
  provider->push_shared(update);

  EXPECT_EQ(call_count, 1);
  EXPECT_EQ(wront_type_count, 0);
}

TEST_F(ProviderTest, MultipleUpdatePush)
{
  int call_count_1 = 0;
  int call_count_2 = 0;

  provider->on<UpdateA>([&](std::shared_ptr<UpdateA>) { call_count_1++; });
  provider->on<UpdateB>([&](std::shared_ptr<UpdateB>) { call_count_2++; });

  auto update_a = std::make_shared<UpdateA>();
  provider->push_shared(update_a);

  auto update_b = std::make_shared<UpdateB>();
  provider->push_shared(update_b);

  provider->push_shared(update_a);

  EXPECT_EQ(call_count_1, 2);
  EXPECT_EQ(call_count_2, 1);
}

TEST_F(ProviderTest, EmptyCalls)
{
  auto update = std::make_shared<UpdateA>();
  EXPECT_NO_THROW(provider->push_shared(update));
}

TEST_F(ProviderTest, PushUpdate)
{
  int call_count_1 = 0;
  int call_count_2 = 0;

  provider->on<MockBatteryStatus>([&](auto update) {
    call_count_1++;
    EXPECT_EQ(update->percentage, 47.3);
    EXPECT_FALSE(update->charging);
  });

  provider->on<MockNavigationStatus>([&](auto update) {
    call_count_2++;
    EXPECT_EQ(update->sequence_id, 2);
  });

  provider->push<MockBatteryStatus>(47.3, false);
  provider->push<MockNavigationStatus>(2);
  provider->push<MockBatteryStatus>(47.3, false);

  EXPECT_EQ(call_count_1, 2);
  EXPECT_EQ(call_count_2, 1);
}
