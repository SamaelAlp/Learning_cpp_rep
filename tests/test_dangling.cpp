#include "chat_core/command.hpp"
#include "chat_core/parser.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <variant>

using namespace chat;

TEST(Dangling, SafeUsage_OwningStringOutlivesCommand) {
    std::string input = "/join #room";
    auto cmd = parse(input);

    ASSERT_TRUE(cmd.has_value());
    ASSERT_TRUE(std::holds_alternative<JoinCommand>(*cmd));

    const auto& join = std::get<JoinCommand>(*cmd);
    EXPECT_EQ(join.room, "#room");
}

TEST(Dangling, SafeUsage_MsgRestOfLine) {
    std::string input = "/msg #general hello world";
    auto cmd = parse(input);

    ASSERT_TRUE(cmd.has_value());
    const auto& msg = std::get<MsgCommand>(*cmd);
    EXPECT_EQ(msg.room, "#general");
    EXPECT_EQ(msg.text, "hello world");
}

TEST(Dangling, DemonstrateDangerCommented) {
    std::string storage = "/join #room";
    auto cmd = parse(storage);
    ASSERT_TRUE(cmd.has_value());
    EXPECT_EQ(std::get<JoinCommand>(*cmd).room, "#room");
}