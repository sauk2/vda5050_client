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

#include <functional>
#include <memory>
#include <string>

#include "vda5050_core/master/master.hpp"
#include "vda5050_core/transport/mqtt_client_interface.hpp"

using vda5050_core::master::VDA5050Master;
using vda5050_core::transport::MqttClientInterface;

class MockMqttClient : public MqttClientInterface
{
public:
  MOCK_METHOD(void, connect, (), (override));
  MOCK_METHOD(void, disconnect, (), (override));
  MOCK_METHOD(bool, connected, (), (override));
  MOCK_METHOD(
    void, publish, (const std::string&, const std::string&, int, bool),
    (override));
  MOCK_METHOD(
    void, subscribe,
    (const std::string&,
     std::function<void(const std::string&, const std::string&)>, int),
    (override));
  MOCK_METHOD(void, unsubscribe, (const std::string&), (override));
  MOCK_METHOD(
    void, set_will, (const std::string&, const std::string&, int, bool),
    (override));
};

class MasterTeardownTest : public ::testing::Test
{
protected:
  std::vector<MockMqttClient*> mock_clients_;
  std::string broker_address_;
  std::shared_ptr<VDA5050Master> master_;

  void SetUp() override
  {
    broker_address_ = "tcp://localhost:1883";

    master_ = std::make_shared<VDA5050Master>(
      [this](auto, auto) { return make_mqtt_client(); });
  }

  std::unique_ptr<MqttClientInterface> make_mqtt_client()
  {
    auto client = std::make_unique<MockMqttClient>();
    auto* raw = client.get();

    // Allow connect() / connected() / subscribe() / disconnect() during
    // the test setup phase without explicit per-call expectations.
    EXPECT_CALL(*raw, connect()).Times(::testing::AnyNumber());
    EXPECT_CALL(*raw, disconnect()).Times(::testing::AnyNumber());
    EXPECT_CALL(*raw, connected())
      .Times(::testing::AnyNumber())
      .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*raw, subscribe(::testing::_, ::testing::_, ::testing::_))
      .Times(::testing::AnyNumber());

    mock_clients_.push_back(raw);

    return client;
  }
};

TEST_F(MasterTeardownTest, OffboardAgvUnsubscribesAllPerAgvTopics)
{
  master_->onboard_agv("acme", "agv-001", broker_address_);

  ASSERT_EQ(mock_clients_.size(), 1);

  auto* mock = mock_clients_[0];

  EXPECT_CALL(
    *mock, unsubscribe(::testing::HasSubstr("acme/agv-001/connection")))
    .Times(1);
  EXPECT_CALL(*mock, unsubscribe(::testing::HasSubstr("acme/agv-001/state")))
    .Times(1);
  EXPECT_CALL(
    *mock, unsubscribe(::testing::HasSubstr("acme/agv-001/factsheet")))
    .Times(1);
  EXPECT_CALL(
    *mock, unsubscribe(::testing::HasSubstr("acme/agv-001/visualization")))
    .Times(1);

  master_->offboard_agv("acme", "agv-001");
}

TEST_F(MasterTeardownTest, MasterDestructionUnsubscribesAllPerAgvTopics)
{
  {
    master_->onboard_agv("acme", "agv-001", broker_address_);

    ASSERT_EQ(mock_clients_.size(), 1);

    auto* mock = mock_clients_[0];

    EXPECT_CALL(
      *mock, unsubscribe(::testing::HasSubstr("acme/agv-001/connection")))
      .Times(1);
    EXPECT_CALL(*mock, unsubscribe(::testing::HasSubstr("acme/agv-001/state")))
      .Times(1);
    EXPECT_CALL(
      *mock, unsubscribe(::testing::HasSubstr("acme/agv-001/factsheet")))
      .Times(1);
    EXPECT_CALL(
      *mock, unsubscribe(::testing::HasSubstr("acme/agv-001/visualization")))
      .Times(1);
  }
  // master leaves scope -> AGV destructor runs -> unsubscribe chain
}

TEST_F(MasterTeardownTest, MultipleAgvsAllUnsubscribeOnOffboard)
{
  master_->onboard_agv("mfg1", "001", broker_address_);
  master_->onboard_agv("mfg2", "002", broker_address_);

  ASSERT_EQ(mock_clients_.size(), 2);

  auto* mock_1 = mock_clients_[0];
  auto* mock_2 = mock_clients_[1];

  EXPECT_CALL(*mock_1, unsubscribe(::testing::HasSubstr("mfg1/001/"))).Times(4);
  EXPECT_CALL(*mock_2, unsubscribe(::testing::HasSubstr("mfg2/002/"))).Times(4);

  master_->offboard_agv("mfg1", "001");
  master_->offboard_agv("mfg2", "002");
}
