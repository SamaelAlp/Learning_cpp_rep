#pragma once

#include <chrono>

namespace chat {

using Timestamp = std::chrono::system_clock::time_point;

class IClock {
public:
    virtual ~IClock() = default;
    [[nodiscard]] virtual Timestamp now() const = 0;
};

}