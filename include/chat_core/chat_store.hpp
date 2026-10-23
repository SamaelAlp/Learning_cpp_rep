#pragma once

#include "chat_core/concepts.hpp"

#include "chat_core/message.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

namespace chat {

class User {
public:
    explicit User(std::string nickname) : nickname_(std::move(nickname)) {}

    [[nodiscard]] const std::string& nickname() const noexcept { return nickname_; }

private:
    std::string nickname_;
};

using UserPtr = std::shared_ptr<User>;

class Room {
public:
    void add_message(MessagePtr msg) {
        messages_.push_back(std::move(msg));
    }

    [[nodiscard]] const std::vector<MessagePtr>& messages() const noexcept {
        return messages_;
    }

    [[nodiscard]] IMessage* message_at(std::size_t index) {
        if (index >= messages_.size()) return nullptr;
        return messages_[index].get();
    }

    [[nodiscard]] const IMessage* message_at(std::size_t index) const {
        if (index >= messages_.size()) return nullptr;
        return messages_[index].get();
    }

    void add_participant(UserPtr user) {
        participants_.push_back(user);
    }

    void cleanup_expired() {
        std::erase_if(participants_, [](const std::weak_ptr<User>& wp) {
            return wp.expired();
        });
    }

    [[nodiscard]] std::size_t participant_count() const {
        std::size_t count = 0;
        for (const auto& wp : participants_) {
            if (!wp.expired()) ++count;
        }
        return count;
    }

    void sort_participants_by_nick_length() {
        std::ranges::sort(
            participants_,
            {},
            [](const std::weak_ptr<User>& wp) {
                if (auto sp = wp.lock()) return sp->nickname().length();
                return std::size_t{0};
            }
        );
    }

private:
    std::vector<MessagePtr>        messages_;
    std::vector<std::weak_ptr<User>> participants_;
};

class ChatStore {
public:

    template <NickLike T>
    UserPtr create_user(T&& nickname) {
        auto user = std::make_shared<User>(std::string(std::forward<T>(nickname)));
        users_[user->nickname()] = user;
        return user;
    }

    template <NickLike T>
    [[nodiscard]] UserPtr find_user(T&& nickname) const {
        auto it = users_.find(std::string(std::forward<T>(nickname)));
        if (it == users_.end()) return nullptr;
        return it->second;
    }

    template <NickLike T>
    void remove_user(T&& nickname) {
        users_.erase(std::string(std::forward<T>(nickname)));
    }

    void create_room(const std::string& name) {
        rooms_.try_emplace(name);
    }

    [[nodiscard]] Room* room(const std::string& name) {
        auto it = rooms_.find(name);
        if (it == rooms_.end()) return nullptr;
        return &it->second;
    }

    [[nodiscard]] const Room* room(const std::string& name) const {
        auto it = rooms_.find(name);
        if (it == rooms_.end()) return nullptr;
        return &it->second;
    }

    [[nodiscard]] std::size_t room_count() const noexcept {
        return rooms_.size();
    }

    template <typename Predicate>
    auto history_view(const std::string& room_name, Predicate&& pred, std::size_t limit)
        requires std::invocable<Predicate, const IMessage&>
    {
        auto* r = room(room_name);
        const auto& msgs = r ? r->messages() : empty_messages_;

        return msgs
            | std::views::filter([&pred](const MessagePtr& msg) {
                return std::invoke(pred, *msg);
            })
            | std::views::take(limit);
}

    template <typename F>
    void for_each_room(F&& f) const {
        for (const auto& [name, room] : rooms_) {
            std::invoke(std::forward<F>(f), name, room);
        }
    }

    template <typename F>
    void for_each_room(F&& f) {
        for (auto& [name, room] : rooms_) {
            std::invoke(std::forward<F>(f), name, room);
        }
    }

private:
    std::map<std::string, UserPtr> users_;
    std::map<std::string, Room>    rooms_;
    static const std::vector<MessagePtr> empty_messages_;
};

}