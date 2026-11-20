#pragma once

#include "ievent_publisher.hpp"

#include <lab/kafka/event_bus.hpp>

#include <memory>
#include <string>

namespace chat::infrastructure {

class KafkaEventPublisher : public chat::IEventPublisher {
public:
    explicit KafkaEventPublisher(const std::string& topic);
    ~KafkaEventPublisher() override;
    
    void publish(const chat::DomainEvent& event) override;
    void flush() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}