#include "chat_service.hpp"
#include "events.hpp"
#include "fake_clock.hpp"
#include "fake_event_publisher.hpp"
#include "hash_engine.hpp"
#include "memory_log_sink.hpp"
#include "std_engine.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <string>
#include <variant>

using chat::FakeClock;
using chat::infrastructure::FakeEventPublisher;
using chat::MemoryLogSink;
using chat::StdEngine;
using chat::HashEngine;

using namespace chat;
using namespace std::chrono_literals;

class ChatServiceTest : public ::testing::TestWithParam<std::string> {
protected:
    void SetUp() override {
        if (GetParam() == "hash") {
            store_ = std::make_unique<StdEngine>();
        } else {
            store_ = std::make_unique<StdEngine>();
        }

        clock_ = std::make_unique<FakeClock>();
        clock_->set(std::chrono::system_clock::time_point{} + 1000s);

        log_ = std::make_unique<MemoryLogSink>();
        publisher_ = std::make_unique<FakeEventPublisher>();

        publisher_ptr_ = publisher_.get();

        service_ = std::make_unique<application::ChatService>(
            std::move(store_),
            *clock_,
            *log_,
            std::move(publisher_)
        );

        service_->store().create_room("general");
        service_->store().create_room("random");
    }

    std::unique_ptr<IChatStore> store_;
    std::unique_ptr<FakeClock> clock_;
    std::unique_ptr<MemoryLogSink> log_;
    std::unique_ptr<FakeEventPublisher> publisher_;
    FakeEventPublisher* publisher_ptr_ = nullptr;
    std::unique_ptr<application::ChatService> service_;
};

TEST_P(ChatServiceTest, SetNickname) {
    auto result = service_->set_nickname("alice");
    
    EXPECT_EQ(result, "Nickname set: alice");
    EXPECT_EQ(service_->current_user(), "alice");
    EXPECT_EQ(log_->size(), 1u);
    EXPECT_EQ(log_->at(0), "[nick] alice");
    EXPECT_EQ(publisher_ptr_->size(), 0u);
}

TEST_P(ChatServiceTest, JoinRoomPublishesEvent) {
    service_->set_nickname("alice");
    publisher_ptr_->clear();
    
    auto result = service_->join_room("general");
    
    EXPECT_EQ(result, "Joined general");
    EXPECT_EQ(publisher_ptr_->size(), 1u);
    
    auto event = publisher_ptr_->at(0);
    ASSERT_TRUE(std::holds_alternative<UserJoined>(event));
    
    const auto& joined = std::get<UserJoined>(event);
    EXPECT_EQ(joined.room, "general");
    EXPECT_EQ(joined.nickname, "alice");
}

TEST_P(ChatServiceTest, JoinRoomCreatesRoomIfMissing) {
    service_->set_nickname("alice");
    
    EXPECT_EQ(service_->store().room("newroom"), nullptr);
    
    auto result = service_->join_room("newroom");
    
    EXPECT_EQ(result, "Joined newroom");
    EXPECT_NE(service_->store().room("newroom"), nullptr);
}

TEST_P(ChatServiceTest, LeaveRoomPublishesEvent) {
    service_->set_nickname("alice");
    service_->join_room("general");
    publisher_ptr_->clear();
    
    auto result = service_->leave_room("general");
    
    EXPECT_EQ(result, "Left general");
    EXPECT_EQ(publisher_ptr_->size(), 1u);
    
    auto event = publisher_ptr_->at(0);
    ASSERT_TRUE(std::holds_alternative<UserLeft>(event));
    
    const auto& left = std::get<UserLeft>(event);
    EXPECT_EQ(left.room, "general");
    EXPECT_EQ(left.nickname, "alice");
}

TEST_P(ChatServiceTest, PostMessagePublishesEvent) {
    service_->set_nickname("alice");
    publisher_ptr_->clear();
    
    auto result = service_->post_message("general", "hello world");
    
    EXPECT_EQ(result, "Sent to general");
    
    auto* room = service_->store().room("general");
    ASSERT_NE(room, nullptr);
    EXPECT_EQ(room->messages().size(), 1u);
    EXPECT_EQ(room->messages()[0]->author(), "alice");
    EXPECT_EQ(room->messages()[0]->render(), "hello world");
    
    EXPECT_EQ(publisher_ptr_->size(), 1u);
    auto event = publisher_ptr_->at(0);
    ASSERT_TRUE(std::holds_alternative<MessagePosted>(event));
    
    const auto& msg = std::get<MessagePosted>(event);
    EXPECT_EQ(msg.room, "general");
    EXPECT_EQ(msg.author, "alice");
    EXPECT_EQ(msg.text, "hello world");
}

TEST_P(ChatServiceTest, PostMessageToUnknownRoom) {
    service_->set_nickname("alice");
    
    auto result = service_->post_message("unknown", "hello");
    
    EXPECT_EQ(result, "Room not found: unknown");
    EXPECT_EQ(publisher_ptr_->size(), 0u);  // событие не публикуется
}

TEST_P(ChatServiceTest, BroadcastPublishesMultipleEvents) {
    service_->set_nickname("alice");
    publisher_ptr_->clear();
    
    auto result = service_->broadcast("hello everyone");
    
    EXPECT_EQ(result, "Broadcasted to 2 room(s)");
    EXPECT_EQ(publisher_ptr_->size(), 2u);

    for (std::size_t i = 0; i < 2; ++i) {
        auto event = publisher_ptr_->at(i);
        ASSERT_TRUE(std::holds_alternative<MessagePosted>(event));
        
        const auto& msg = std::get<MessagePosted>(event);
        EXPECT_EQ(msg.author, "alice");
        EXPECT_EQ(msg.text, "hello everyone");
    }

    auto* general = service_->store().room("general");
    auto* random = service_->store().room("random");
    ASSERT_NE(general, nullptr);
    ASSERT_NE(random, nullptr);
    EXPECT_EQ(general->messages().size(), 1u);
    EXPECT_EQ(random->messages().size(), 1u);
}

TEST_P(ChatServiceTest, MessagesHaveCorrectTimestamps) {
    service_->set_nickname("alice");

    service_->post_message("general", "first");
    

    clock_->advance(5s);

    service_->post_message("general", "second");
    
    auto* room = service_->store().room("general");
    ASSERT_NE(room, nullptr);
    ASSERT_EQ(room->messages().size(), 2u);
    
    auto t1 = room->messages()[0]->time();
    auto t2 = room->messages()[1]->time();
    
    EXPECT_LT(t1, t2);
    EXPECT_EQ(t2 - t1, 5s);
}

TEST_P(ChatServiceTest, LogsAllOperations) {
    service_->set_nickname("alice");
    service_->join_room("general");
    service_->post_message("general", "hello");
    service_->leave_room("general");
    
    EXPECT_EQ(log_->size(), 4u);
    EXPECT_EQ(log_->at(0), "[nick] alice");
    EXPECT_EQ(log_->at(1), "[join] alice -> general");
    EXPECT_EQ(log_->at(2), "[msg] general: alice: hello");
    EXPECT_EQ(log_->at(3), "[leave] alice <- general");
}


TEST_P(ChatServiceTest, EventsPublishedInOrder) {
    service_->set_nickname("alice");
    publisher_ptr_->clear();
    
    service_->join_room("general");
    service_->post_message("general", "hello");
    service_->leave_room("general");
    
    ASSERT_EQ(publisher_ptr_->size(), 3u);
    
    EXPECT_TRUE(std::holds_alternative<UserJoined>(publisher_ptr_->at(0)));
    EXPECT_TRUE(std::holds_alternative<MessagePosted>(publisher_ptr_->at(1)));
    EXPECT_TRUE(std::holds_alternative<UserLeft>(publisher_ptr_->at(2)));
    
    auto room_of = [](const DomainEvent& e) -> std::string {
        return std::visit([](const auto& ev) { return ev.room; }, e);
    };
    
    EXPECT_EQ(room_of(publisher_ptr_->at(0)), "general");
    EXPECT_EQ(room_of(publisher_ptr_->at(1)), "general");
    EXPECT_EQ(room_of(publisher_ptr_->at(2)), "general");
}

INSTANTIATE_TEST_SUITE_P(
    Engines,
    ChatServiceTest,
    ::testing::Values("std", "hash"),
    [](const ::testing::TestParamInfo<std::string>& param_info) {
        return param_info.param;
    }
);