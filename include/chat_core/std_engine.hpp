#pragma once

#include "chat_core/i_chat_store.hpp"
#include "chat_core/message.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace chat {

class StdEngine : public IChatStore {
public:
    void create_room(const std::string& name) override {
        rooms_.try_emplace(name);
    }
    
    Room* room(const std::string& name) override {
        auto it = rooms_.find(name);
        return it != rooms_.end() ? &it->second : nullptr;
    }
    
    const Room* room(const std::string& name) const override {
        auto it = rooms_.find(name);
        return it != rooms_.end() ? &it->second : nullptr;
    }
    
    std::size_t room_count() const noexcept override {
        return rooms_.size();
    }
    
    void for_each_room_name(const std::function<void(const std::string&)>& f) const override {
        for (const auto& [name, _] : rooms_) {
            f(name);
        }
    }
    
    void for_each_room_mut(const std::function<void(const std::string&, Room&)>& f) override {
        for (auto& [name, room] : rooms_) {
            f(name, room);
        }
    }
    
    UserPtr create_user(const std::string& nickname) override {
        auto user = std::make_shared<User>(nickname);
        auto [it, inserted] = users_.try_emplace(nickname, user);
        if (!inserted) {
            return it->second;
        }
        return user;
    }
    
    UserPtr find_user(const std::string& nickname) const override {
        auto it = users_.find(nickname);
        return it != users_.end() ? it->second : nullptr;
    }
    
    void remove_user(const std::string& nickname) override {
        users_.erase(nickname);
    }
    
    void add_message(const std::string& room_name, MessagePtr msg) override {
        auto it = rooms_.find(room_name);
        if (it != rooms_.end()) {
            it->second.add_message(std::move(msg));
        }
    }

private:
    std::map<std::string, Room> rooms_;
    std::map<std::string, UserPtr> users_;
};

}