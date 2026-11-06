#include "chat_core/errors.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

using namespace chat;

TEST(Errors, ParseErrorIsChatError) {
    try {
        throw ParseError("bad syntax");
    } catch (const ChatError& e) {
        EXPECT_EQ(std::string(e.what()).find("Parse error"), 0u);
    }
}

TEST(Errors, UnknownRoomErrorContainsRoomName) {
    try {
        throw UnknownRoomError("general");
    } catch (const UnknownRoomError& e) {
        EXPECT_EQ(e.room(), "general");
        EXPECT_NE(std::string(e.what()).find("general"), std::string::npos);
    }
}

TEST(Errors, NicknameTakenErrorContainsNickname) {
    try {
        throw NicknameTakenError("alice");
    } catch (const NicknameTakenError& e) {
        EXPECT_EQ(e.nickname(), "alice");
        EXPECT_NE(std::string(e.what()).find("alice"), std::string::npos);
    }
}

TEST(Errors, TypeMismatchErrorContainsTypes) {
    try {
        throw TypeMismatchError("Reaction", "TextBody");
    } catch (const TypeMismatchError& e) {
        EXPECT_EQ(e.expected(), "Reaction");
        EXPECT_EQ(e.actual(), "TextBody");
    }
}

TEST(Errors, AllErrorsCatchableAsStdException) {
    try {
        throw ParseError("test");
    } catch (const std::exception& e) {
        SUCCEED();
    }

    try {
        throw UnknownRoomError("test");
    } catch (const std::exception& e) {
        SUCCEED();
    }
}