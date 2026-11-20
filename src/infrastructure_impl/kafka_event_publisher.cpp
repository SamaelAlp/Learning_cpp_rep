#include "kafka_event_publisher.hpp"

#include <iostream>
#include <sstream>
#include <variant>

namespace chat::infrastructure {

using chat::DomainEvent;
using chat::MessagePosted;
using chat::UserJoined;
using chat::UserLeft;

namespace {

std::string serialize(const DomainEvent& event) {
    return std::visit([](const auto& e) -> std::string {
        using T = std::decay_t<decltype(e)>;
        std::ostringstream os;
        
        if constexpr (std::is_same_v<T, MessagePosted>) {
            os << "MessagePosted\t" << e.room << "\t" << e.author << "\t" << e.text;
        } else if constexpr (std::is_same_v<T, UserJoined>) {
            os << "UserJoined\t" << e.room << "\t" << e.nickname;
        } else if constexpr (std::is_same_v<T, UserLeft>) {
            os << "UserLeft\t" << e.room << "\t" << e.nickname;
        }
        
        return os.str();
    }, event);
}

std::string extract_key(const DomainEvent& event) {
    return std::visit([](const auto& e) -> std::string {
        return e.room;
    }, event);
}

}

struct KafkaEventPublisher::Impl {
    lab::kafka::Producer producer;
    
    explicit Impl(const std::string& topic)
        : producer({.topic = topic})
    {
        producer.on_delivery([](const lab::kafka::DeliveryReport& r) {
            if (!r.ok) {
                std::cerr << "[kafka] delivery failed: partition "
                          << r.partition << ", error\n";
            }
        });
    }
};

KafkaEventPublisher::KafkaEventPublisher(const std::string& topic)
    : impl_(std::make_unique<Impl>(topic))
{}

KafkaEventPublisher::~KafkaEventPublisher() = default;

void KafkaEventPublisher::publish(const DomainEvent& event) {
    
    impl_->producer.send(extract_key(event), serialize(event));
}

void KafkaEventPublisher::flush() {
    
}

}