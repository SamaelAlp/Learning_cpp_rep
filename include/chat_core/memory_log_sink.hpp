#pragma once

#include "chat_core/ilog_sink.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace chat {

class MemoryLogSink : public ILogSink {
public:
    void append(const std::string& line) override {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_.push_back(line);
    }

    [[nodiscard]] std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return entries_.size();
    }

    [[nodiscard]] std::string at(std::size_t i) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return entries_.at(i);
    }

    [[nodiscard]] std::vector<std::string> entries() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return entries_;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_.clear();
    }

private:
    std::vector<std::string> entries_;
    mutable std::mutex mutex_;
};

}