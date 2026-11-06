#include "chat_core/bad_hash.hpp"
#include "chat_core/fnv1a.hpp"

#include <gtest/gtest.h>

#include <string_view>
#include <unordered_set>

using namespace chat;

TEST(Fnv1a, DifferentStringsDifferentHashes) {
    Fnv1aHash hash;
    
    std::unordered_set<uint64_t> hashes;
    for (int i = 0; i < 1000; ++i) {
        std::string s = "key" + std::to_string(i);
        hashes.insert(hash(s));
    }
    
    EXPECT_GT(hashes.size(), 990u);
}

TEST(Fnv1a, EmptyString) {
    Fnv1aHash hash;
    uint64_t h = hash("");
    EXPECT_EQ(h, Fnv1aHash::offset_basis);
}

TEST(BadHash, AnagramsCollide) {
    BadHash hash;
    
    EXPECT_EQ(hash("abc"), hash("cba"));
    EXPECT_EQ(hash("hello"), hash("olleh"));
}

TEST(BadHash, SameLengthCollisions) {
    BadHash hash;
    
    std::unordered_set<uint64_t> hashes;
    for (int i = 0; i < 100; ++i) {
        std::string s = "user" + std::to_string(i);
        hashes.insert(hash(s));
    }
    
    EXPECT_LT(hashes.size(), 50u);
}