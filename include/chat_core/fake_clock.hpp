#pragma once

#include "chat_core/iclock.hpp"

#include <chrono>

namespace chat {

class FakeClock : public IClock {
public:
    explicit FakeClock(Timestamp start = Timestamp{}) : current_(start) {}

    [[nodiscard]] Timestamp now() const override {
        return current_;
    }

    void set(Timestamp ts) { current_ = ts; }

    void advance(std::chrono::seconds s) {
        current_ += s;
    }

private:
    Timestamp current_;
};

}