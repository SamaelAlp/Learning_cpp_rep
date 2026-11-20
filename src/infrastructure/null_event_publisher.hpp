#pragma once

#include "ievent_publisher.hpp"

namespace chat {

class NullEventPublisher : public IEventPublisher {
public:
    void publish(const DomainEvent&) override {
    }
    
    void flush() override {
    }
};

}