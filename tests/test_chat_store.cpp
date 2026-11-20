#include "chat_core/i_chat_store.hpp"
#include "chat_core/message.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

using namespace chat;

class ChatStoreTest : public ::testing::TestWithParam<std::string> {
protected:
    void SetUp() override {
        engine_ = make_engine(GetParam());
    }
    
    std::unique_ptr<IChatStore> engine_;
};

TEST_P(ChatStoreTest, CreateRoomIncreasesCount) {
    EXPECT_EQ(engine_->room_count(), 0u);
    engine_->create_room("a");
    EXPECT_EQ(engine_->room_count(), 1u);
    engine_->create_room("b");
    EXPECT_EQ(engine_->room_count(), 2u);
}

TEST_P(ChatStoreTest, RoomReturnsNullptrForUnknown) {
    EXPECT_EQ(engine_->room("nope"), nullptr);
}

TEST_P(ChatStoreTest, RoomReturnsPointerForKnown) {
    engine_->create_room("r");
    EXPECT_NE(engine_->room("r"), nullptr);
}

TEST_P(ChatStoreTest, AddMessageToRoom) {
    engine_->create_room("r");
    auto* r = engine_->room("r");
    ASSERT_NE(r, nullptr);
    
    r->add_message(make_text_message(
        MessageId{1, Timestamp{}, "alice"}, "alice", "hello"
    ));
    
    EXPECT_EQ(r->messages().size(), 1u);
}

TEST_P(ChatStoreTest, CreateUserAndFind) {
    auto user = engine_->create_user("alice");
    ASSERT_NE(user, nullptr);
    EXPECT_EQ(user->nickname(), "alice");
    
    auto found = engine_->find_user("alice");
    EXPECT_EQ(found, user);
}

TEST_P(ChatStoreTest, RemoveUser) {
    engine_->create_user("alice");
    engine_->remove_user("alice");
    EXPECT_EQ(engine_->find_user("alice"), nullptr);
}

TEST_P(ChatStoreTest, ForEachRoomName) {
    engine_->create_room("a");
    engine_->create_room("b");
    
    std::vector<std::string> names;
    engine_->for_each_room_name([&](const std::string& name) {
        names.push_back(name);
    });
    
    EXPECT_EQ(names.size(), 2u);
}

INSTANTIATE_TEST_SUITE_P(
    Engines,
    ChatStoreTest,
    ::testing::Values("std", "hash"),
    [](const ::testing::TestParamInfo<std::string>& param_info) {
        return param_info.param;
    }
);