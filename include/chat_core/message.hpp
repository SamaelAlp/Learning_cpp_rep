#pragma once

#include "chat_core/spy.hpp"
#include "chat_core/concepts.hpp"

#include <chrono>
#include <compare>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace chat {

using Timestamp = std::chrono::system_clock::time_point;

struct MessageId {
    std::uint64_t seq;
    Timestamp     ts;
    std::string   author_nickname;   // НЕ участвует в сравнении

    std::strong_ordering operator<=>(const MessageId& other) const {
        if (auto cmp = seq <=> other.seq; cmp != 0) return cmp;
        return ts <=> other.ts;
    }

    bool operator==(const MessageId& other) const {
        return seq == other.seq && ts == other.ts;
    }
};

class IMessage {
public:
    virtual ~IMessage() = default;

    [[nodiscard]] virtual MessageId   id()     const = 0;
    [[nodiscard]] virtual std::string author() const = 0;
    [[nodiscard]] virtual Timestamp   time()   const = 0;
    [[nodiscard]] virtual std::string render() const = 0;
    [[nodiscard]] virtual std::string type_name() const = 0;

    virtual bool try_react() { return false; }
    [[nodiscard]] virtual int reaction_count() const { return 0; }
};

class TextMessage : public IMessage {
public:
    TextMessage(MessageId id, std::string author, std::string text)
        : id_(std::move(id))
        , author_(std::move(author))
        , text_(std::move(text))
    {}

    [[nodiscard]] MessageId   id()     const override { return id_; }
    [[nodiscard]] std::string author() const override { return author_; }
    [[nodiscard]] Timestamp   time()   const override { return id_.ts; }
    [[nodiscard]] std::string render() const override { return text_; }
    [[nodiscard]] std::string type_name() const override { return "text"; }

    [[nodiscard]] std::vector<std::byte> to_bytes() const { return {}; }
    [[nodiscard]] static TextMessage from_bytes(const std::vector<std::byte>&) {
        return TextMessage{{0, {}, ""}, "", ""};
    }

private:
    MessageId   id_;
    std::string author_;
    std::string text_;
    Spy         spy_;
};

class SystemMessage : public IMessage {
public:
    SystemMessage(MessageId id, std::string text)
        : id_(std::move(id)), text_(std::move(text))
    {}

    [[nodiscard]] MessageId   id()     const override { return id_; }
    [[nodiscard]] std::string author() const override { return "system"; }
    [[nodiscard]] Timestamp   time()   const override { return id_.ts; }
    [[nodiscard]] std::string render() const override { return "[SYSTEM] " + text_; }
    [[nodiscard]] std::string type_name() const override { return "system"; }

    [[nodiscard]] std::vector<std::byte> to_bytes() const { return {}; }
    [[nodiscard]] static SystemMessage from_bytes(const std::vector<std::byte>&) {
        return SystemMessage{{0, {}, ""}, ""};
    }

private:
    MessageId   id_;
    std::string text_;
    Spy         spy_;
};

class ReactionMessage : public IMessage {
public:
    ReactionMessage(MessageId id, std::string author, std::string emoji, int count = 1)
        : id_(std::move(id))
        , author_(std::move(author))
        , emoji_(std::move(emoji))
        , count_(count)
    {}

    [[nodiscard]] MessageId   id()     const override { return id_; }
    [[nodiscard]] std::string author() const override { return author_; }
    [[nodiscard]] Timestamp   time()   const override { return id_.ts; }
    [[nodiscard]] std::string render() const override { return emoji_ + " x" + std::to_string(count_); }
    [[nodiscard]] std::string type_name() const override { return "reaction"; }

    bool try_react() override { ++count_; return true; }
    [[nodiscard]] int reaction_count() const override { return count_; }

    [[nodiscard]] std::vector<std::byte> to_bytes() const { return {}; }
    [[nodiscard]] static ReactionMessage from_bytes(const std::vector<std::byte>&) {
        return ReactionMessage{{0, {}, ""}, "", "", 0};
    }

private:
    MessageId   id_;
    std::string author_;
    std::string emoji_;
    int         count_;
    Spy         spy_;
};

static_assert(Serializable<TextMessage>,    "TextMessage must be Serializable");
static_assert(Serializable<SystemMessage>,  "SystemMessage must be Serializable");
static_assert(Serializable<ReactionMessage>,"ReactionMessage must be Serializable");

using MessagePtr = std::unique_ptr<IMessage>;

template <typename... Args>
MessagePtr make_text_message(Args&&... args) {
    return std::make_unique<TextMessage>(std::forward<Args>(args)...);
}

template <typename... Args>
MessagePtr make_system_message(Args&&... args) {
    return std::make_unique<SystemMessage>(std::forward<Args>(args)...);
}

template <typename... Args>
MessagePtr make_reaction_message(Args&&... args) {
    return std::make_unique<ReactionMessage>(std::forward<Args>(args)...);
}

}