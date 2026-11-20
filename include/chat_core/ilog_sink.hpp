#pragma once

#include <string>

namespace chat {

class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void append(const std::string& line) = 0;
};

}