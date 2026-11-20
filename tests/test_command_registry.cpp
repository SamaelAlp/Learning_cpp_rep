#include "chat_core/command_registry.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace chat;

TEST(CommandRegistry, RegisterAndExecute) {
    CommandRegistry registry;
    
    registry.register_command("/echo", [](std::string text) -> std::string {
        return "Echo: " + text;
    });
    
    std::vector<std::string_view> tokens = {"/echo", "hello"};
    std::string result = registry.execute("/echo hello", tokens);
    EXPECT_EQ(result, "Echo: hello");
}

TEST(CommandRegistry, WrongArity) {
    CommandRegistry registry;
    
    registry.register_command("/add", [](int a, int b) -> std::string {
        return std::to_string(a + b);
    });
    
    std::vector<std::string_view> tokens = {"/add", "1"};
    std::string result = registry.execute("/add 1", tokens);
    EXPECT_NE(result.find("Wrong number"), std::string::npos);
}

TEST(CommandRegistry, UnknownCommand) {
    CommandRegistry registry;
    
    std::vector<std::string_view> tokens = {"/unknown"};
    std::string result = registry.execute("/unknown", tokens);
    EXPECT_NE(result.find("Unknown command"), std::string::npos);
}

TEST(CommandRegistry, MultipleArguments) {
    CommandRegistry registry;
    
    registry.register_command("/concat", [](std::string a, std::string b, std::string c) -> std::string {
        return a + b + c;
    });
    
    std::vector<std::string_view> tokens = {"/concat", "a", "b", "c"};
    std::string result = registry.execute("/concat a b c", tokens);
    EXPECT_EQ(result, "abc");
}