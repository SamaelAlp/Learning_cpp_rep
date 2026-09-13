#include "chat_core/chat_store.hpp"
#include "chat_core/message.hpp"
#include "chat_core/spy.hpp"

#include <gtest/gtest.h>

#include <utility>
#include <vector>

using namespace chat;

TEST(Message, DefaultConstructionIsEmpty) {
    Message m;
    EXPECT_TRUE(m.author.empty());
    EXPECT_TRUE(m.text.empty());
}

TEST(Message, MoveDoesNotInvokeCopy) {
    Spy::reset();
    Message m1{"alice", "hi", {}};
    Spy::reset();

    Message m2 = std::move(m1);

    EXPECT_EQ(Spy::copy_ctor_count, 0u);
    EXPECT_EQ(Spy::move_ctor_count, 1u);
    EXPECT_EQ(m2.author, "alice");
    EXPECT_EQ(m2.text, "hi");
}

TEST(Message, CopyInvokesCopyNotMove) {
    Spy::reset();
    Message m1{"alice", "hi", {}};
    Spy::reset();

    Message m2 = m1;

    EXPECT_EQ(Spy::copy_ctor_count, 1u);
    EXPECT_EQ(Spy::move_ctor_count, 0u);
}

TEST(ChatStore, PostingTemporaryDoesNotCopy) {
    Spy::reset();
    ChatStore store;
    store.create_room("r");
    Spy::reset();

    store.post("r", Message{"a", "b", {}});

    EXPECT_EQ(Spy::copy_ctor_count, 0u);
    EXPECT_GE(Spy::move_ctor_count, 1u);
}

TEST(ChatStore, PostingLvalueCopies) {
    Spy::reset();
    ChatStore store;
    store.create_room("r");
    Message m{"a", "b", {}};
    Spy::reset();

    store.post("r", m);

    EXPECT_EQ(Spy::copy_ctor_count, 1u);
}