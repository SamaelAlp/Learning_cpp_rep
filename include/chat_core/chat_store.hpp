#pragma once

#include "chat_core/message.hpp"

#include <cstddef>
#include <functional>
#include <map>
#include <string>
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

    [[nodiscard]] const std::vector<Message>* history(const std::string& room) const {
        auto it = rooms_.find(room);
        if (it == rooms_.end()) return nullptr;
        return &it->second;
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