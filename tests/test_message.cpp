#include "chat_core/message.hpp"
#include "chat_core/spy.hpp"
#include "chat_core/fake_clock.hpp"

#include <gtest/gtest.h>


#include <utility>

using namespace chat;
using namespace std::chrono_literals; 

TEST(Message, TextMessageCreation) {
    auto msg = make_text_message(
        MessageId{1, Timestamp{}, "alice"},
        "alice",
        "hello"
    );
    EXPECT_EQ(msg->author(), "alice");
    EXPECT_EQ(msg->render(), "hello");
    EXPECT_EQ(msg->type_name(), "text");
}

TEST(Message, SystemMessageCreation) {
    auto msg = make_system_message(
        MessageId{1, Timestamp{}, "system"},
        "user joined"
    );
    EXPECT_EQ(msg->author(), "system");
    EXPECT_EQ(msg->render(), "[SYSTEM] user joined");
    EXPECT_EQ(msg->type_name(), "system");
}

TEST(Message, ReactionMessageCreation) {
    auto msg = make_reaction_message(
        MessageId{1, Timestamp{}, "alice"},
        "alice",
        ":thumbsup:",
        3
    );
    EXPECT_EQ(msg->render(), ":thumbsup: x3");
    EXPECT_EQ(msg->type_name(), "reaction");
    EXPECT_EQ(msg->reaction_count(), 3);
}

TEST(Message, ReactionMessageTryReact) {
    auto msg = make_reaction_message(
        MessageId{1, Timestamp{}, "alice"},
        "alice",
        ":+1:",
        1
    );
    EXPECT_TRUE(msg->try_react());
    EXPECT_EQ(msg->reaction_count(), 2);
}

TEST(Message, TextMessageTryReactFails) {
    auto msg = make_text_message(
        MessageId{1, Timestamp{}, "alice"},
        "alice",
        "hello"
    );
    EXPECT_FALSE(msg->try_react());
}

TEST(Message, MoveDoesNotInvokeCopy) {
    Spy::reset();
    
    TextMessage msg1{
        MessageId{1, Timestamp{}, "alice"},
        "alice",
        "hi"
    };
    
    Spy::reset();

    TextMessage msg2 = std::move(msg1);

    EXPECT_EQ(Spy::copy_ctor_count, 0u);
    EXPECT_GE(Spy::move_ctor_count, 1u);
    EXPECT_EQ(msg2.author(), "alice");
    EXPECT_EQ(msg2.render(), "hi");
}

TEST(Message, CopyInvokesCopyNotMove) {
    Spy::reset();
    
    TextMessage msg1{
        MessageId{1, Timestamp{}, "alice"},
        "alice",
        "hi"
    };
    
    Spy::reset();

    TextMessage msg2 = msg1;

    EXPECT_GE(Spy::copy_ctor_count, 1u);
    EXPECT_EQ(Spy::move_ctor_count, 0u);
}

TEST(Message, TimestampFromClock) {
    FakeClock clock;
    clock.set(Timestamp{} + 100s);
    
    auto msg = make_text_message(clock, MessageId{1, Timestamp{}, "alice"}, "alice", "hello");
    
    EXPECT_EQ(msg->time(), Timestamp{} + 100s);
}

TEST(Message, MessagesInOrder) {
    FakeClock clock;
    
    auto m1 = make_text_message(clock, MessageId{1, Timestamp{}, "a"}, "a", "first");
    clock.advance(1s);
    auto m2 = make_text_message(clock, MessageId{2, Timestamp{}, "a"}, "a", "second");
    clock.advance(1s);
    auto m3 = make_text_message(clock, MessageId{3, Timestamp{}, "a"}, "a", "third");
    
    EXPECT_LT(m1->time(), m2->time());
    EXPECT_LT(m2->time(), m3->time());
}