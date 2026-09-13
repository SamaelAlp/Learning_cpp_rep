#include "chat_core/command.hpp"
#include "chat_core/parser.hpp"

#include <gtest/gtest.h>

#include <string>
#include <variant>

using namespace chat;

TEST(Parser, EmptyInput) {
    EXPECT_FALSE(parse("").has_value());
    EXPECT_FALSE(parse("   ").has_value());
}

TEST(Parser, UnknownCommand) {
    EXPECT_FALSE(parse("/unknown").has_value());
    EXPECT_FALSE(parse("hello").has_value());
}

TEST(Parser, JoinCommand) {
    auto cmd = parse("/join #general");
    ASSERT_TRUE(cmd.has_value());
    ASSERT_TRUE(std::holds_alternative<JoinCommand>(*cmd));
    EXPECT_EQ(std::get<JoinCommand>(*cmd).room, "#general");
}

TEST(Parser, LeaveCommand) {
    auto cmd = parse("/leave #random");
    ASSERT_TRUE(cmd.has_value());
    ASSERT_TRUE(std::holds_alternative<LeaveCommand>(*cmd));
    EXPECT_EQ(std::get<LeaveCommand>(*cmd).room, "#random");
}

TEST(Parser, MsgCommandRestOfLine) {
    auto cmd = parse("/msg #general hello world how are you");
    ASSERT_TRUE(cmd.has_value());
    ASSERT_TRUE(std::holds_alternative<MsgCommand>(*cmd));
    const auto& m = std::get<MsgCommand>(*cmd);
    EXPECT_EQ(m.room, "#general");
    EXPECT_EQ(m.text, "hello world how are you");
}

TEST(Parser, MsgCommandMissingText) {
    EXPECT_FALSE(parse("/msg #room").has_value());
}

TEST(Parser, WhoCommand) {
    auto cmd = parse("/who");
    ASSERT_TRUE(cmd.has_value());
    EXPECT_TRUE(std::holds_alternative<WhoCommand>(*cmd));
}

TEST(Parser, HistoryCommand) {
    auto cmd = parse("/history #general");
    ASSERT_TRUE(cmd.has_value());
    ASSERT_TRUE(std::holds_alternative<HistoryCommand>(*cmd));
    EXPECT_EQ(std::get<HistoryCommand>(*cmd).room, "#general");
}

TEST(Parser, TypeCommand) {
    auto cmd = parse("/type #room 2");
    ASSERT_TRUE(cmd.has_value());
    ASSERT_TRUE(std::holds_alternative<TypeCommand>(*cmd));
    const auto& t = std::get<TypeCommand>(*cmd);
    EXPECT_EQ(t.room, "#room");
    EXPECT_EQ(t.index, 2u);
}

TEST(Parser, ReactCommand) {
    auto cmd = parse("/react #room 0");
    ASSERT_TRUE(cmd.has_value());
    ASSERT_TRUE(std::holds_alternative<ReactCommand>(*cmd));
    const auto& r = std::get<ReactCommand>(*cmd);
    EXPECT_EQ(r.room, "#room");
    EXPECT_EQ(r.index, 0u);
}

TEST(Parser, TypeCommandBadIndex) {
    EXPECT_FALSE(parse("/type #room abc").has_value());
    EXPECT_FALSE(parse("/type #room").has_value());
}

TEST(Parser, ExtraSpaces) {
    auto cmd = parse("  /join   #room  ");
    ASSERT_TRUE(cmd.has_value());
    EXPECT_EQ(std::get<JoinCommand>(*cmd).room, "#room");
}

TEST(Parser, QuotedRoomName) {
    auto cmd = parse(R"(/join "my room")");
    ASSERT_TRUE(cmd.has_value());
    EXPECT_EQ(std::get<JoinCommand>(*cmd).room, "my room");
}