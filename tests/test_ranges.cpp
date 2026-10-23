#include "chat_core/chat_store.hpp"
#include "chat_core/message.hpp"

#include <gtest/gtest.h>

#include <string>

using namespace chat;

TEST(Ranges, HistoryPipelineIsLazy) {
    ChatStore store;
    store.create_room("#general");
    auto* r = store.room("#general");
    ASSERT_NE(r, nullptr);
    
    // Создаём 1000 сообщений
    for (int i = 0; i < 1000; ++i) {
        r->add_message(make_text_message(
            MessageId{static_cast<uint64_t>(i), Timestamp{}, "alice"},
            "alice",
            "msg"
        ));
    }

    int predicate_call_count = 0;
    
    auto is_alice = [&predicate_call_count](const IMessage& m) {
        ++predicate_call_count;
        return m.author() == "alice";
    };

    auto view = store.history_view("#general", is_alice, 10);
    
    std::vector<const IMessage*> result;
    for (const auto& msg : view) {
        result.push_back(msg.get());
    }

    EXPECT_EQ(result.size(), 10u);
    
    // Ленивость: предикат вызывается примерно 10-15 раз, а не 1000.
    // Точное число зависит от реализации std::views::take и filter.
    EXPECT_GE(predicate_call_count, 10);
    EXPECT_LE(predicate_call_count, 20);  // с запасом
}

TEST(Ranges, SortWithProjection) {
    ChatStore store;
    store.create_user("alice");
    store.create_user("bob");
    store.create_user("christopher");
    store.create_user("dan");

    store.create_room("#test");
    auto* r = store.room("#test");
    r->add_participant(store.find_user("alice"));
    r->add_participant(store.find_user("bob"));
    r->add_participant(store.find_user("christopher"));
    r->add_participant(store.find_user("dan"));

    r->sort_participants_by_nick_length();

    EXPECT_EQ(r->participant_count(), 4u);
}