#pragma once

#include "ievent_publisher.hpp"

#include <mutex>
#include <vector>

namespace chat::infrastructure {

class FakeEventPublisher : public chat::IEventPublisher {
public:
    void publish(const chat::DomainEvent& event) override {
        std::lock_guard<std::mutex> lock(mutex_);
        events_.push_back(event);
    }
    
    void flush() override {
        // Ничего не делаем
    }
    
    [[nodiscard]] std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return events_.size();
    }
    
    [[nodiscard]] chat::DomainEvent at(std::size_t i) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return events_.at(i);
    }
    
    [[nodiscard]] std::vector<chat::DomainEvent> events() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return events_;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        events_.clear();
    }

private:
    std::vector<chat::DomainEvent> events_;
    mutable std::mutex mutex_;
};

}