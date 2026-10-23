#include "chat_core/convert.hpp"

#include <gtest/gtest.h>

#include <string>
#include <string_view>

using namespace chat;

TEST(Convert, StringView) {
    auto result = convert<std::string_view>("hello");
    EXPECT_EQ(result, "hello");
}

TEST(Convert, String) {
    auto result = convert<std::string>("world");
    EXPECT_EQ(result, "world");
}

TEST(Convert, Int) {
    auto result = convert<int>("42");
    EXPECT_EQ(result, 42);
}

TEST(Convert, Double) {
    auto result = convert<double>("3.14");
    EXPECT_NEAR(result, 3.14, 0.001);
}

TEST(Convert, RoomName) {
    auto result = convert<RoomName>("#general");
    EXPECT_EQ(result.value, "general");
}

TEST(Convert, RoomNameWithoutHash) {
    EXPECT_THROW(convert<RoomName>("general"), std::invalid_argument);
}

TEST(Convert, InvalidNumber) {
    EXPECT_THROW(convert<int>("abc"), std::invalid_argument);
}