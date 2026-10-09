#pragma once

#include "chat_core/imessage.hpp"

#include <memory>
#include <string>

namespace chat {

class ReactionMessage : public IMessage {
public:
    ReactionMessage(std::string emoji, int count = 0)
        : emoji_(std::move(emoji))
        , count_(count)
    {}

    [[nodiscard]] std::string to_string() const override {
        return emoji_ + " x" + std::to_string(count_);
    }

    [[nodiscard]] std::string type_name() const override {
        return "Reaction";
    }

    [[nodiscard]] std::unique_ptr<IMessage> clone() const override {
        return std::make_unique<ReactionMessage>(*this);
    }

    [[nodiscard]] const std::string& emoji() const noexcept {
        return emoji_;
    }

    [[nodiscard]] int count() const noexcept {
        return count_;
    }

    void increment() noexcept {
        ++count_;
    }

private:
    std::string emoji_;
    int count_;
};

}