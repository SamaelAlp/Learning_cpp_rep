#pragma once

#include "chat_core/message.hpp"
#include "chat_core/chat_store.hpp"

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace chat {

class IChatStore {
public:
    virtual ~IChatStore() = default;

    virtual void create_room(const std::string& name) = 0;
    virtual Room* room(const std::string& name) = 0;
    virtual const Room* room(const std::string& name) const = 0;
    virtual std::size_t room_count() const noexcept = 0;
    
    template <typename F>
    void for_each_room_sorted(F&& f) const {
        std::vector<std::string> names;
        for_each_room_name([&](const std::string& name) {
            names.push_back(name);
        });
        std::sort(names.begin(), names.end());
        for (const auto& name : names) {
            if (const Room* r = room(name)) {
                std::invoke(std::forward<F>(f), name, *r);
            }
        }
    }
    
    virtual void for_each_room_name(const std::function<void(const std::string&)>& f) const = 0;
    
    virtual void for_each_room_mut(const std::function<void(const std::string&, Room&)>& f) = 0;

    virtual UserPtr create_user(const std::string& nickname) = 0;
    virtual UserPtr find_user(const std::string& nickname) const = 0;
    virtual void remove_user(const std::string& nickname) = 0;

    virtual void add_message(const std::string& room, MessagePtr msg) = 0;
};

std::unique_ptr<IChatStore> make_engine(const std::string& name);

}