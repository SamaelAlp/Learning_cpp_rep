#include "chat_core/history_iterator.hpp"
#include "chat_core/ring_buffer.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <vector>

using namespace chat;

static_assert(std::forward_iterator<RingBuffer<int, 5>::iterator>,
              "RingBuffer::iterator must satisfy forward_iterator");
static_assert(std::forward_iterator<RingBuffer<int, 5>::const_iterator>,
              "RingBuffer::const_iterator must satisfy forward_iterator");

TEST(HistoryIterator, EmptyRange) {
    RingBuffer<int, 5> rb;
    EXPECT_EQ(rb.begin(), rb.end());
}

TEST(HistoryIterator, IterateOverFullBuffer) {
    RingBuffer<int, 3> rb;
    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3);
    
    std::vector<int> result;
    for (int x : rb) {
        result.push_back(x);
    }
    
    EXPECT_EQ(result, (std::vector<int>{1, 2, 3}));
}

TEST(HistoryIterator, IterateAfterEviction) {
    RingBuffer<int, 3> rb;
    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3);
    rb.push_back(4);
    rb.push_back(5);
    
    std::vector<int> result;
    for (int x : rb) {
        result.push_back(x);
    }
    
    EXPECT_EQ(result, (std::vector<int>{3, 4, 5}));
}

TEST(HistoryIterator, WorksWithSTLAlgorithms) {
    RingBuffer<int, 5> rb;
    rb.push_back(3);
    rb.push_back(1);
    rb.push_back(4);
    rb.push_back(1);
    rb.push_back(5);
    
    int sum = std::accumulate(rb.begin(), rb.end(), 0);
    EXPECT_EQ(sum, 14);
    
    auto it = std::find(rb.begin(), rb.end(), 4);
    ASSERT_NE(it, rb.end());
    EXPECT_EQ(*it, 4);
}

TEST(HistoryIterator, ConstIterator) {
    RingBuffer<int, 3> rb;
    rb.push_back(1);
    rb.push_back(2);
    
    const auto& crb = rb;
    std::vector<int> result;
    for (int x : crb) {
        result.push_back(x);
    }
    EXPECT_EQ(result, (std::vector<int>{1, 2}));
}