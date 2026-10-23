#pragma once

#include "chat_core/user.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace chat {

class UserRegistry {
public:
    std::shared_ptr<User> create_user(const std::string& nickname) {
        auto user = std::make_shared<User>(nickname);
        users_.emplace(nickname, user);
        return user;
    }

    void remove_user(const std::string& nickname) {
        users_.erase(nickname);
    }

    [[nodiscard]] std::shared_ptr<User> get_user(const std::string& nickname) const {
        auto it = users_.find(nickname);
        if (it == users_.end()) return nullptr;
        return it->second;
    }

    [[nodiscard]] std::size_t user_count() const noexcept {
        return users_.size();
    }

private:
    std::unordered_map<std::string, std::shared_ptr<User>> users_;
};

}