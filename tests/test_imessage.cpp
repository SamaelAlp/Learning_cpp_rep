#include "chat_core/imessage.hpp"
#include "chat_core/message.hpp"
#include "chat_core/reaction_message.hpp"
#include "chat_core/system_message.hpp"
#include "chat_core/text_message.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>

using namespace chat;

TEST(IMessage, TextMessageWorks) {
    auto msg = std::make_unique<TextMessage>("hello");
    EXPECT_EQ(msg->to_string(), "hello");
    EXPECT_EQ(msg->type_name(), "Text");
}

TEST(IMessage, SystemMessageWorks) {
    auto msg = std::make_unique<SystemMessage>("user joined");
    EXPECT_EQ(msg->to_string(), "[system] user joined");
    EXPECT_EQ(msg->type_name(), "System");
}

TEST(IMessage, ReactionMessageWorks) {
    auto msg = std::make_unique<ReactionMessage>("like", 5);
    EXPECT_EQ(msg->to_string(), "like x5");
    EXPECT_EQ(msg->type_name(), "Reaction");
    msg->increment();
    EXPECT_EQ(msg->to_string(), "like x6");
}

TEST(IMessage, CloneCreatesIndependentCopy) {
    auto original = std::make_unique<TextMessage>("hello");
    auto cloned = original->clone();

    EXPECT_EQ(cloned->to_string(), "hello");
    EXPECT_NE(original.get(), cloned.get());
}

TEST(IMessage, PolymorphismWorks) {
    std::unique_ptr<IMessage> msg = std::make_unique<ReactionMessage>("heart", 3);
    EXPECT_EQ(msg->type_name(), "Reaction");
    EXPECT_EQ(msg->to_string(), "heart x3");
}

TEST(Message, ContainsUniquePtrToIMessage) {
    Message msg{"alice", std::make_unique<TextMessage>("hi"), {}};
    EXPECT_EQ(msg.body->to_string(), "hi");
    EXPECT_EQ(msg.body->type_name(), "Text");
}

TEST(Message, MoveTransfersOwnership) {
    Message msg1{"alice", std::make_unique<TextMessage>("hi"), {}};
    Message msg2 = std::move(msg1);

    EXPECT_EQ(msg2.body->to_string(), "hi");
    EXPECT_EQ(msg1.body, nullptr);
}