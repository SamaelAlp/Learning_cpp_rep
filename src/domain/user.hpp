#pragma once

#include <memory>
#include <string>
#include <utility>

namespace chat {

class User {
public:
    explicit User(std::string nickname) : nickname_(std::move(nickname)) {}

    [[nodiscard]] const std::string& nickname() const noexcept {
        return nickname_;
    }

private:
    std::string nickname_;
};

using UserPtr = std::shared_ptr<User>;

}