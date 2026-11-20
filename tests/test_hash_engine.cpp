#include "chat_core/hash_engine.hpp"

#include <gtest/gtest.h>

#include <string>

using namespace chat;

TEST(HashTable, EmptyOnConstruction) {
    HashTable<std::string, int> ht;
    EXPECT_EQ(ht.size(), 0u);
    EXPECT_EQ(ht.bucket_count(), 16u);
}

TEST(HashTable, TryEmplaceInserts) {
    HashTable<std::string, int> ht;
    auto [ptr, inserted] = ht.try_emplace("key", 42);
    EXPECT_TRUE(inserted);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
    EXPECT_EQ(ht.size(), 1u);
}

TEST(HashTable, TryEmplaceDoesNotDuplicate) {
    HashTable<std::string, int> ht;
    ht.try_emplace("key", 42);
    auto [ptr, inserted] = ht.try_emplace("key", 100);
    EXPECT_FALSE(inserted);
    EXPECT_EQ(*ptr, 42);
    EXPECT_EQ(ht.size(), 1u);
}

TEST(HashTable, FindReturnsPointer) {
    HashTable<std::string, int> ht;
    ht.try_emplace("key", 42);
    
    int* ptr = ht.find("key");
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
    
    EXPECT_EQ(ht.find("nonexistent"), nullptr);
}

TEST(HashTable, EraseRemovesElement) {
    HashTable<std::string, int> ht;
    ht.try_emplace("key", 42);
    EXPECT_TRUE(ht.erase("key"));
    EXPECT_EQ(ht.size(), 0u);
    EXPECT_EQ(ht.find("key"), nullptr);
}

TEST(HashTable, RehashOnLoadFactor) {
    HashTable<std::string, int> ht;
    
    for (int i = 0; i < 100; ++i) {
        ht.try_emplace("key" + std::to_string(i), i);
    }
    
    EXPECT_EQ(ht.size(), 100u);
    EXPECT_GT(ht.bucket_count(), 16u); 
    
    const double max_lf = HashTable<std::string, int>::kMaxLoadFactor;
    EXPECT_LE(ht.load_factor(), max_lf);
}

TEST(HashTable, ForEachKey) {
    HashTable<std::string, int> ht;
    ht.try_emplace("a", 1);
    ht.try_emplace("b", 2);
    ht.try_emplace("c", 3);
    
    std::vector<std::string> keys;
    ht.for_each_key([&](const std::string& key) {
        keys.push_back(key);
    });
    
    EXPECT_EQ(keys.size(), 3u);
}