#include "chat_core/command_registry.hpp"
#include "chat_core/lexer.hpp"

#include <gtest/gtest.h>

#include <string>
#include <string_view>
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
    CommandRegistry registry;
    std::string captured_room;
    
    registry.register_command("/msg", [&](RoomName room, [[maybe_unused]] std::string text) -> std::string {
        captured_room = room.value;
        return "OK";
    });

    std::string result;
    {
        std::string temp = "/msg #room hello";
        auto tokens = tokenize(temp);
        result = registry.execute(temp, tokens);
    }
    
    EXPECT_EQ(result, "OK");
    EXPECT_EQ(captured_room, "room");
}