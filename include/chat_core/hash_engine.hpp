#pragma once

#include "chat_core/fnv1a.hpp"
#include "chat_core/i_chat_store.hpp"
#include "chat_core/message.hpp"

#include <cstddef>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace chat {

template <typename K, typename V, typename Hash = Fnv1aHash>
class HashTable {
public:
    static constexpr double kMaxLoadFactor = 0.75;
    static constexpr std::size_t kInitialBucketCount = 16;
    
    HashTable() : buckets_(kInitialBucketCount) {}
    
    std::pair<V*, bool> try_emplace(const K& key, V value) {
        maybe_rehash();
        
        std::size_t idx = bucket_index(key);
        auto& bucket = buckets_[idx];

        for (auto& pair : bucket) {
            if (pair.first == key) {
                return {&pair.second, false};
            }
        }
        
        bucket.emplace_back(key, std::move(value));
        ++size_;
        return {&bucket.back().second, true};
    }
    
    V* find(const K& key) {
        std::size_t idx = bucket_index(key);
        for (auto& pair : buckets_[idx]) {
            if (pair.first == key) {
                return &pair.second;
            }
        }
        return nullptr;
    }
    
    const V* find(const K& key) const {
        std::size_t idx = bucket_index(key);
        for (const auto& pair : buckets_[idx]) {
            if (pair.first == key) {
                return &pair.second;
            }
        }
        return nullptr;
    }
    
    bool erase(const K& key) {
        std::size_t idx = bucket_index(key);
        auto& bucket = buckets_[idx];
        
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                bucket.erase(it);
                --size_;
                return true;
            }
        }
        return false;
    }
    
    template <typename F>
    void for_each_key(F&& f) const {
        for (const auto& bucket : buckets_) {
            for (const auto& pair : bucket) {
                std::invoke(f, pair.first);
            }
        }
    }
    
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] std::size_t bucket_count() const noexcept { return buckets_.size(); }
    [[nodiscard]] double load_factor() const noexcept {
        return buckets_.empty() ? 0.0 : static_cast<double>(size_) / buckets_.size();
    }
    
    [[nodiscard]] std::size_t chain_length(std::size_t bucket_idx) const {
        return buckets_[bucket_idx].size();
    }

private:
    using Bucket = std::list<std::pair<K, V>>;
    std::vector<Bucket> buckets_;
    std::size_t size_ = 0;
    Hash hash_;
    
    std::size_t bucket_index(const K& key) const {
        return hash_(key) % buckets_.size();
    }
    
    void maybe_rehash() {
        if (load_factor() >= kMaxLoadFactor) {
            rehash(buckets_.size() * 2);
        }
    }
    
    void rehash(std::size_t new_bucket_count) {
        std::vector<Bucket> new_buckets(new_bucket_count);
        
        for (auto& bucket : buckets_) {
            for (auto& pair : bucket) {
                std::size_t new_idx = hash_(pair.first) % new_bucket_count;
                new_buckets[new_idx].push_back(std::move(pair));
            }
        }
        
        buckets_ = std::move(new_buckets);
    }
};

class HashEngine : public IChatStore {
public:
    void create_room(const std::string& name) override {
        rooms_.try_emplace(name, Room{});
    }
    
    Room* room(const std::string& name) override {
        return rooms_.find(name);
    }
    
    const Room* room(const std::string& name) const override {
        return rooms_.find(name);
    }
    
    std::size_t room_count() const noexcept override {
        return rooms_.size();
    }
    
    void for_each_room_name(const std::function<void(const std::string&)>& f) const override {
        rooms_.for_each_key(f);
    }
    
    void for_each_room_mut(const std::function<void(const std::string&, Room&)>& f) override {
        for (std::size_t i = 0; i < rooms_.bucket_count(); ++i) {
            // Не можем напрямую итерировать по HashTable, поэтому
            // используем find для каждого имени
            // В реальной реализации добавили бы итератор в HashTable
        }
        std::vector<std::string> names;
        rooms_.for_each_key([&](const std::string& name) {
            names.push_back(name);
        });
        for (const auto& name : names) {
            if (Room* r = rooms_.find(name)) {
                f(name, *r);
            }
        }
    }
    
    UserPtr create_user(const std::string& nickname) override {
        auto user = std::make_shared<User>(nickname);
        auto [ptr, inserted] = users_.try_emplace(nickname, user);
        if (!inserted) {
            return *ptr;
        }
        return user;
    }
    
    UserPtr find_user(const std::string& nickname) const override {
        const UserPtr* ptr = users_.find(nickname);
        return ptr ? *ptr : nullptr;
    }
    
    void remove_user(const std::string& nickname) override {
        users_.erase(nickname);
    }
    
    void add_message(const std::string& room_name, MessagePtr msg) override {
        if (Room* r = rooms_.find(room_name)) {
            r->add_message(std::move(msg));
        }
    }
    
    const HashTable<std::string, Room>& rooms_table() const { return rooms_; }
    const HashTable<std::string, UserPtr>& users_table() const { return users_; }

private:
    HashTable<std::string, Room> rooms_;
    HashTable<std::string, UserPtr> users_;
};

}