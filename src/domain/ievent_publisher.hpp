#pragma once

#include "events.hpp"

#include <string>

namespace chat {

class IEventPublisher {
public:
    virtual ~IEventPublisher() = default;
    
    virtual void publish(const DomainEvent& event) = 0;
    
    virtual void flush() = 0;
};

}