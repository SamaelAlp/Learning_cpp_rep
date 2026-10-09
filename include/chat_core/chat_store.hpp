#pragma once

#include "chat_core/errors.hpp"
#include "chat_core/message.hpp"

#include <cstddef>
#include <functional>
#include <map>
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
        it->second.push_back(std::move(msg));
    }

    void broadcast(const std::vector<std::string>& rooms, Message msg) {
        auto new_rooms = rooms_;

        for (const auto& room : rooms) {
            auto key = normalize(room);
            auto it = new_rooms.find(std::string(key));
            if (it == new_rooms.end()) {
                throw UnknownRoomError(room);
            }
            it->second.push_back(msg);
        }

        std::swap(rooms_, new_rooms);
    }

    [[nodiscard]]
    std::optional<std::reference_wrapper<const std::vector<Message>>>
    history(std::string_view room) const {
        auto it = rooms_.find(std::string(normalize(room)));
        if (it == rooms_.end()) return std::nullopt;
        return std::cref(it->second);
    }

    [[nodiscard]]
    std::optional<std::reference_wrapper<Message>>
    message_at(std::string_view room, std::size_t index) {
        auto it = rooms_.find(std::string(normalize(room)));
        if (it == rooms_.end()) return std::nullopt;
        if (index >= it->second.size()) return std::nullopt;
        return std::ref(it->second[index]);
    }

    [[nodiscard]] std::size_t room_count() const noexcept {
        return rooms_.size();
    }

    template <typename F>
    void for_each_room(F&& f) const {
        for (const auto& [name, messages] : rooms_) {
            std::invoke(std::forward<F>(f), name, messages);
        }
    }

private:
    std::map<std::string, std::vector<Message>> rooms_;

    static std::string_view normalize(std::string_view name) noexcept {
        if (!name.empty() && name.front() == '#') {
            name.remove_prefix(1);
        }
        return name;
    }
};

}