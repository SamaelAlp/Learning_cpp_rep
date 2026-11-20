#include "chat_service.hpp"
#include "events.hpp"
#include "message.hpp"

#include <utility>

namespace chat::application {

ChatService::ChatService(std::unique_ptr<IChatStore> store,
                         const IClock& clock,
                         ILogSink& log,
                         std::unique_ptr<IEventPublisher> publisher)
    : store_(std::move(store))
    , clock_(clock)
    , log_(log)
    , publisher_(std::move(publisher))
{}

std::string ChatService::set_nickname(const std::string& nickname) {
    auto user = store_->create_user(nickname);
    current_user_ = user->nickname();
    log_.append("[nick] " + current_user_);
    return "Nickname set: " + current_user_;
}

std::string ChatService::join_room(const std::string& room) {
    store_->create_room(room);
    auto* r = store_->room(room);
    if (!r) return "Failed to create room";

    if (!current_user_.empty()) {
        auto user = store_->find_user(current_user_);
        if (user) {
            r->add_participant(user);
            publisher_->publish(UserJoined{room, current_user_});
        }
    }

    log_.append("[join] " + current_user_ + " -> " + room);
    return "Joined " + room;
}

std::string ChatService::leave_room(const std::string& room) {
    if (!current_user_.empty()) {
        publisher_->publish(UserLeft{room, current_user_});
    }
    log_.append("[leave] " + current_user_ + " <- " + room);
    return "Left " + room;
}

std::string ChatService::post_message(const std::string& room, std::string text) {
    auto* r = store_->room(room);
    if (!r) return "Room not found: " + room;

    r->post_message(clock_, current_user_, text);
    publisher_->publish(MessagePosted{room, current_user_, text});

    log_.append("[msg] " + room + ": " + current_user_ + ": " + text);
    return "Sent to " + room;
}

std::string ChatService::broadcast(const std::string& text) {
    int count = 0;
    store_->for_each_room_mut([&]([[maybe_unused]] const std::string& name, Room& r) {
        r.post_message(clock_, current_user_, text);
        publisher_->publish(MessagePosted{name, current_user_, text});
        ++count;
    });

    log_.append("[broadcast] " + current_user_ + ": " + text);
    return "Broadcasted to " + std::to_string(count) + " room(s)";
}

}