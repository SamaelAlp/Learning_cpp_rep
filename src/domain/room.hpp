#pragma once

#include "iclock.hpp"
#include "history_iterator.hpp"
#include "message.hpp"
#include "ring_buffer.hpp"
#include "user.hpp"

#include <functional>
#include <algorithm>
#include <cstddef>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

namespace chat {

class Room {
public:
    static constexpr std::size_t HISTORY_CAPACITY = 1000;

    void add_message(MessagePtr msg) {
        messages_.push_back(std::move(msg));
    }

    void post_message(const IClock& clock, const std::string& author, std::string text) {
        auto msg = make_text_message(
            clock,
            MessageId{static_cast<std::uint64_t>(messages_.size()), clock.now(), author},
            author,
            std::move(text)
        );
        messages_.push_back(std::move(msg));
    }

    template <typename Predicate>
    auto history_view(Predicate&& pred, std::size_t limit) const {
        auto filter_pred = [&pred](const MessagePtr& msg) {
            return std::invoke(pred, *msg);
        };
        return messages_
            | std::views::filter(filter_pred)
            | std::views::take(limit);
    }

    [[nodiscard]] const RingBuffer<MessagePtr, HISTORY_CAPACITY>& messages() const noexcept {
        return messages_;
    }

    [[nodiscard]] RingBuffer<MessagePtr, HISTORY_CAPACITY>& messages() noexcept {
        return messages_;
    }

    [[nodiscard]] IMessage* message_at(std::size_t index) {
        if (index >= messages_.size()) return nullptr;
        return messages_[index].get();
    }

    [[nodiscard]] const IMessage* message_at(std::size_t index) const {
        if (index >= messages_.size()) return nullptr;
        return messages_[index].get();
    }

    void add_participant(UserPtr user) {
        participants_.push_back(user);
    }

    void cleanup_expired() {
        std::erase_if(participants_, [](const std::weak_ptr<User>& wp) {
            return wp.expired();
        });
    }

    [[nodiscard]] std::size_t participant_count() const {
        std::size_t count = 0;
        for (const auto& wp : participants_) {
            if (!wp.expired()) ++count;
        }
        return count;
    }

    void sort_participants_by_nick_length() {
        std::ranges::sort(
            participants_,
            {},
            [](const std::weak_ptr<User>& wp) {
                if (auto sp = wp.lock()) return sp->nickname().length();
                return std::size_t{0};
            }
        );
    }

private:
    RingBuffer<MessagePtr, HISTORY_CAPACITY> messages_;
    std::vector<std::weak_ptr<User>> participants_;
};

} 