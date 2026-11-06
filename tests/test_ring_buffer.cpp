#include "chat_core/ring_buffer.hpp"

#include <gtest/gtest.h>

#include <string>

using namespace chat;

TEST(RingBuffer, EmptyOnConstruction) {
    RingBuffer<int, 5> rb;
    EXPECT_TRUE(rb.empty());
    EXPECT_EQ(rb.size(), 0u);
    EXPECT_EQ(rb.capacity(), 5u);
}

TEST(RingBuffer, PushBackIncreasesSize) {
    RingBuffer<int, 5> rb;
    rb.push_back(1);
    rb.push_back(2);
    EXPECT_EQ(rb.size(), 2u);
    EXPECT_EQ(rb[0], 1);
    EXPECT_EQ(rb[1], 2);
}

TEST(RingBuffer, EvictsOldestOnOverflow) {
    RingBuffer<int, 3> rb;
    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3);
    rb.push_back(4);
    
    EXPECT_EQ(rb.size(), 3u);
    EXPECT_EQ(rb[0], 2);
    EXPECT_EQ(rb[1], 3);
    EXPECT_EQ(rb[2], 4);
}

TEST(RingBuffer, WraparoundWorks) {
    RingBuffer<int, 3> rb;
    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3);
    rb.push_back(4);
    rb.push_back(5);
    
    EXPECT_EQ(rb.size(), 3u);
    EXPECT_EQ(rb[0], 3);
    EXPECT_EQ(rb[1], 4);
    EXPECT_EQ(rb[2], 5);
}

TEST(RingBuffer, AtThrowsOnOutOfRange) {
    RingBuffer<int, 3> rb;
    rb.push_back(1);
    EXPECT_EQ(rb.at(0), 1);
    EXPECT_THROW((void)rb.at(1), std::out_of_range);
}

TEST(RingBuffer, ClearResetsState) {
    RingBuffer<int, 3> rb;
    rb.push_back(1);
    rb.push_back(2);
    rb.clear();
    EXPECT_TRUE(rb.empty());
    EXPECT_EQ(rb.size(), 0u);
}