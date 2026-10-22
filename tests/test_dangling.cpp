#include "chat_core/command.hpp"
#include "chat_core/lexer.hpp"

#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <variant>
#include <vector>

using namespace chat;

TEST(Dangling, TokenizeOnTemporaryStringIsDangerous) {
    std::vector<std::string_view> tokens;
    {
        std::string temp = "/join #room";
        tokens = tokenize(temp);
        ASSERT_EQ(tokens.size(), 2u);
        EXPECT_EQ(tokens[0], "/join");
    }
    tokens.clear();
}

TEST(Dangling, ParseResultSurvivesInputString) {
    std::optional<Command> cmd;
    {
        std::string temp = "/msg #room hello world";
        cmd = parse(temp);
        ASSERT_TRUE(cmd.has_value());
    }
    auto* m = std::get_if<MsgCommand>(&*cmd);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(m->rooms.size(), 1u);
    EXPECT_EQ(m->rooms[0], "room");
    EXPECT_EQ(m->text, "hello world");
}