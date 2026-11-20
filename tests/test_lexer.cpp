#include "chat_core/lexer.hpp"

#include <gtest/gtest.h>

#include <string_view>
#include <vector>

using namespace chat;

TEST(Lexer, EmptyInput) {
    EXPECT_TRUE(tokenize("").empty());
    EXPECT_TRUE(tokenize("   ").empty());
    EXPECT_TRUE(tokenize("\t\n").empty());
}

TEST(Lexer, SingleToken) {
    auto tokens = tokenize("/who");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0], "/who");
}

TEST(Lexer, MultipleTokens) {
    auto tokens = tokenize("/join #general");
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0], "/join");
    EXPECT_EQ(tokens[1], "#general");
}

TEST(Lexer, ExtraSpaces) {
    auto tokens = tokenize("  /join   #general  ");
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0], "/join");
    EXPECT_EQ(tokens[1], "#general");
}

TEST(Lexer, QuotedString) {
    auto tokens = tokenize(R"(/topic #general "обсуждаем курс")");
    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[0], "/topic");
    EXPECT_EQ(tokens[1], "#general");
    EXPECT_EQ(tokens[2], "обсуждаем курс");
}

TEST(Lexer, QuotedStringWithSpaces) {
    auto tokens = tokenize(R"(/msg #room "hello   world")");
    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[2], "hello   world");
}

TEST(Lexer, MsgRestOfLine) {
    auto tokens = tokenize("/msg #room hello world");
    ASSERT_EQ(tokens.size(), 4u);
    EXPECT_EQ(tokens[0], "/msg");
    EXPECT_EQ(tokens[1], "#room");
    EXPECT_EQ(tokens[2], "hello");
    EXPECT_EQ(tokens[3], "world");
}

TEST(Lexer, NoStringAllocations) {
    std::string_view input = "/join #room";
    auto tokens = tokenize(input);
    for (auto t : tokens) {
        EXPECT_GE(t.data(), input.data());
        EXPECT_LE(t.data() + t.size(), input.data() + input.size());
    }
}