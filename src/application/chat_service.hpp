#pragma once

#include "ichat_store.hpp"
#include "iclock.hpp"
#include "ilog_sink.hpp"
#include "ievent_publisher.hpp"

#include <memory>
#include <string>

namespace chat::application {

class ChatService {
public:
    ChatService(std::unique_ptr<IChatStore> store,
                const IClock& clock,
                ILogSink& log,
                std::unique_ptr<IEventPublisher> publisher);

    std::string set_nickname(const std::string& nickname);
    std::string join_room(const std::string& room);
    std::string leave_room(const std::string& room);
    std::string post_message(const std::string& room, std::string text);
    std::string broadcast(const std::string& text);

    [[nodiscard]] const std::string& current_user() const noexcept {
        return current_user_;
    }

    [[nodiscard]] IChatStore& store() noexcept { return *store_; }
    [[nodiscard]] const IChatStore& store() const noexcept { return *store_; }

private:
    std::unique_ptr<IChatStore> store_;
    const IClock& clock_;
    ILogSink& log_;
    std::unique_ptr<IEventPublisher> publisher_;
    std::string current_user_;
};

}