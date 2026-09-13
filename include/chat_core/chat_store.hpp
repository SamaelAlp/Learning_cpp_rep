#pragma once

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
    void create_room(const std::string& name) {
        rooms_.try_emplace(name);
    }

    void post(const std::string& room, Message msg) {
        auto it = rooms_.find(room);
        if (it == rooms_.end()) return;
        it->second.push_back(std::move(msg));
    }

    [[nodiscard]]
    std::optional<std::reference_wrapper<const std::vector<Message>>>
    history(const std::string& room) const {
        auto it = rooms_.find(room);
        if (it == rooms_.end()) return std::nullopt;
        return std::cref(it->second);
    }

    [[nodiscard]]
    std::optional<std::reference_wrapper<Message>>
    message_at(const std::string& room, std::size_t index) {
        auto it = rooms_.find(room);
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
};

}