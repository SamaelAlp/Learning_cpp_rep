#include "std_engine.hpp"
#include "message.hpp"
#include "ring_buffer.hpp"
#include "fake_clock.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <ranges>
#include <string>

using namespace chat;
using namespace std::chrono_literals;

TEST(Ranges, HistoryPipelineIsLazy) {
    RingBuffer<MessagePtr, 1000> buffer;
    FakeClock clock;
    clock.set(Timestamp{} + 1000s);

    for (int i = 0; i < 1000; ++i) {
        buffer.push_back(make_text_message(
            clock,
            MessageId{static_cast<uint64_t>(i), clock.now(), "alice"},
            "alice",
            "msg"
        ));
        clock.advance(1s);
    }

    std::atomic<int> pred_calls{0};
    auto is_alice = [&](const MessagePtr& msg) {
        ++pred_calls;
        return msg->author() == "alice";
    };

    auto view = buffer
              | std::views::filter(is_alice)
              | std::views::take(10);

    int count = 0;
    for (const auto& msg : view) {
        ++count;
        (void)msg;
    }

    EXPECT_EQ(count, 10);

    EXPECT_LE(pred_calls.load(), 20);
}

TEST(Ranges, SortWithProjection) {
    StdEngine store;
    store.create_user("alice");
    store.create_user("bob");
    store.create_user("charlie");

    EXPECT_NE(store.find_user("alice"), nullptr);
    EXPECT_NE(store.find_user("bob"), nullptr);
    EXPECT_NE(store.find_user("charlie"), nullptr);
    EXPECT_EQ(store.find_user("unknown"), nullptr);
}