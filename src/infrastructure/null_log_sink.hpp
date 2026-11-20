#pragma once

#include "ilog_sink.hpp"

#include <string>

namespace chat {

class NullLogSink : public ILogSink {
public:
    void append(const std::string&) override {
        // Ничего не делаем
    }
};

}