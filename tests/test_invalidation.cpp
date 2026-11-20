#include "chat_core/chat_store.hpp"
#include "chat_core/message.hpp"

#include <gtest/gtest.h>

#include <vector>

using namespace chat;

TEST(Invalidation, EraseDuringIterationIsDangerous) {
    std::vector<int> v = {1, 2, 3, 4, 5};

    for (auto it = v.begin(); it != v.end(); ) {
        if (*it % 2 == 0) {
            it = v.erase(it);
        } else {
            ++it;
        }
    }
    
    EXPECT_EQ(v, (std::vector<int>{1, 3, 5}));
}

TEST(Invalidation, RingBufferInvalidationOnPushBack) {
    
    RingBuffer<int, 3> rb;
    rb.push_back(1);
    rb.push_back(2);
    rb.push_back(3);
    
    auto it = rb.begin();
    EXPECT_EQ(*it, 1);
    
    rb.push_back(4); 
    
    EXPECT_EQ(*it, 2);
}

TEST(Invalidation, MapIteratorInvalidationOnErase) {
    std::map<std::string, int> m = {{"a", 1}, {"b", 2}, {"c", 3}};
    
    for (auto it = m.begin(); it != m.end(); ) {
        if (it->second % 2 == 0) {
            it = m.erase(it);
        } else {
            ++it;
        }
    }
    
    EXPECT_EQ(m.size(), 2u);
    EXPECT_EQ(m.at("a"), 1);
    EXPECT_EQ(m.at("c"), 3);
}