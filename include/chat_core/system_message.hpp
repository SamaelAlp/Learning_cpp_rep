#pragma once

#include "chat_core/imessage.hpp"

#include <memory>
#include <string>

namespace chat {

class SystemMessage : public IMessage {
public:
    explicit SystemMessage(std::string notice)
        : notice_(std::move(notice))
    {}

    [[nodiscard]] std::string to_string() const override {
        return "[system] " + notice_;
    }

    [[nodiscard]] std::string type_name() const override {
        return "System";
    }

    [[nodiscard]] std::unique_ptr<IMessage> clone() const override {
        return std::make_unique<SystemMessage>(*this);
    }

    [[nodiscard]] const std::string& notice() const noexcept {
        return notice_;
    }

private:
    std::string notice_;
};

}