#include "chat_core/message.hpp"

#include <gtest/gtest.h>

using namespace chat;

TEST(MessageId, DefaultSpaceshipOperator) {
    Timestamp t1{}, t2{};
    MessageId id1{1, t1, "alice"};
    MessageId id2{1, t1, "bob"};

    EXPECT_TRUE(id1 == id2);
    EXPECT_FALSE(id1 != id2);
    EXPECT_FALSE(id1 < id2);
    EXPECT_FALSE(id1 > id2);
    EXPECT_TRUE(id1 <= id2);
    EXPECT_TRUE(id1 >= id2);
}

TEST(MessageId, ComparisonWithDifferentSeq) {
    Timestamp t{};
    MessageId id1{1, t, "alice"};
    MessageId id2{2, t, "alice"};

    EXPECT_FALSE(id1 == id2);
    EXPECT_TRUE(id1 < id2);
    EXPECT_TRUE(id1 <= id2);
    EXPECT_FALSE(id1 > id2);
    EXPECT_FALSE(id1 >= id2);
}