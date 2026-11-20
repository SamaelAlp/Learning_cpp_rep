#pragma once

#include "iclock.hpp"
#include "message.hpp"

#include <cstdint>
#include <string>
#include <utility>

namespace chat {

class MessageFactory {
public:
    explicit MessageFactory(const IClock& clock) : clock_(clock) {}

    MessagePtr create_text(const std::string& author, std::string text) {
        auto id = next_id(author);
        return std::make_unique<TextMessage>(clock_, id, author, std::move(text));
    }

    MessagePtr create_system(std::string text) {
        auto id = next_id("system");
        return std::make_unique<SystemMessage>(clock_, id, std::move(text));
    }

    MessagePtr create_reaction(const std::string& author, std::string emoji, int count = 1) {
        auto id = next_id(author);
        return std::make_unique<ReactionMessage>(clock_, id, author, std::move(emoji), count);
    }

    void reset_seq() noexcept { seq_ = 0; }

private:
    const IClock& clock_;
    std::uint64_t seq_ = 0;

    MessageId next_id(const std::string& author) {
        return MessageId{++seq_, clock_.now(), author};
    }
};

} 