#pragma once

#include "chat_core/imessage.hpp"

#include <memory>
#include <string>

namespace chat {

class TextMessage : public IMessage {
public:
    explicit TextMessage(std::string text)
        : text_(std::move(text))
    {}

    [[nodiscard]] std::string to_string() const override {
        return text_;
    }

    [[nodiscard]] std::string type_name() const override {
        return "Text";
    }

    [[nodiscard]] std::unique_ptr<IMessage> clone() const override {
        return std::make_unique<TextMessage>(*this);
    }

    [[nodiscard]] const std::string& text() const noexcept {
        return text_;
    }

private:
    std::string text_;
};

}