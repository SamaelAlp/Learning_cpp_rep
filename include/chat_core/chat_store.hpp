#pragma once

#include "chat_core/errors.hpp"
#include "chat_core/message.hpp"
#include "chat_core/user.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace chat {

class ChatStore {
public:
    void create_room(std::string_view name) {
        rooms_.try_emplace(std::string(normalize(name)));
    }

    void post(std::string_view room, Message msg) {
        auto it = rooms_.find(std::string(normalize(room)));
        if (it == rooms_.end()) {
            throw UnknownRoomError(room);
        }
        it->second.messages.push_back(std::move(msg));
    }

    void broadcast(const std::vector<std::string>& rooms, Message msg) {
        auto new_rooms = rooms_;

        for (const auto& room : rooms) {
            auto key = normalize(room);
            auto it = new_rooms.find(std::string(key));
            if (it == new_rooms.end()) {
                throw UnknownRoomError(room);
            }
            it->second.messages.push_back(msg);
        }

        std::swap(rooms_, new_rooms);
    }

    void add_participant(std::string_view room, std::weak_ptr<User> user) {
        auto it = rooms_.find(std::string(normalize(room)));
        if (it == rooms_.end()) {
            throw UnknownRoomError(room);
        }
        it->second.participants.push_back(std::move(user));
    }

    void cleanup_expired_participants(std::string_view room) {
        auto it = rooms_.find(std::string(normalize(room)));
        if (it == rooms_.end()) return;

        auto& participants = it->second.participants;
        participants.erase(
            std::remove_if(participants.begin(), participants.end(),
                [](const std::weak_ptr<User>& wp) {
                    return wp.expired();
                }),
            participants.end()
        );
    }

    [[nodiscard]] std::vector<std::shared_ptr<User>> get_active_participants(std::string_view room) const {
        auto it = rooms_.find(std::string(normalize(room)));
        if (it == rooms_.end()) return {};

        std::vector<std::shared_ptr<User>> active;
        for (const auto& wp : it->second.participants) {
            if (auto sp = wp.lock()) {
                active.push_back(std::move(sp));
            }
        }
        return active;
    }

    [[nodiscard]]
    std::optional<std::reference_wrapper<const std::vector<Message>>>
    history(std::string_view room) const {
        auto it = rooms_.find(std::string(normalize(room)));
        if (it == rooms_.end()) return std::nullopt;
        return std::cref(it->second.messages);
    }

    [[nodiscard]]
    std::optional<std::reference_wrapper<Message>>
    message_at(std::string_view room, std::size_t index) {
        auto it = rooms_.find(std::string(normalize(room)));
        if (it == rooms_.end()) return std::nullopt;
        if (index >= it->second.messages.size()) return std::nullopt;
        return std::ref(it->second.messages[index]);
    }

    [[nodiscard]] std::size_t room_count() const noexcept {
        return rooms_.size();
    }

    template <typename F>
    void for_each_room(F&& f) const {
        for (const auto& [name, room_data] : rooms_) {
            std::invoke(std::forward<F>(f), name, room_data.messages);
        }
    }

private:
    struct RoomData {
        std::vector<Message> messages;
        std::vector<std::weak_ptr<User>> participants;
    };

    std::map<std::string, RoomData> rooms_;

    static std::string_view normalize(std::string_view name) noexcept {
        if (!name.empty() && name.front() == '#') {
            name.remove_prefix(1);
        }
        return name;
    }
};

}