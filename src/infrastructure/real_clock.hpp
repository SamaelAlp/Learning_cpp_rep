#pragma once

#include "iclock.hpp"

namespace chat {

class RealClock : public IClock {
public:
    [[nodiscard]] Timestamp now() const override {
        return std::chrono::system_clock::now();
    }
};

}