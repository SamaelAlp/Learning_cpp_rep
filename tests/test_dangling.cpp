#include "chat_core/command.hpp"
#include "chat_core/parser.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <variant>

using namespace chat;

// ДРАКОН ЭТАПА: висячий string_view

// Проблема: Command хранит string_view, которые указывают в исходную
// строку ввода. Если строка уничтожена, а Command ещё жив — все
// string_view внутри Command становятся висячими (dangling).

// Пример опасного кода:

// auto cmd = parse(std::string("/join #room"));
//              ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// временный std::string создаётся, передаётся в parse
// как string_view, парсер возвращает Command с
// string_view, указывающими в этот временный объект.
// В конце full expression временный string уничтожается.

// cmd теперь содержит висячие string_view!
// auto& j = std::get<JoinCommand>(*cmd);
// std::cout << j.room;  // UNDEFINED BEHAVIOR

// Вывод под AddressSanitizer (clang++ -fsanitize=address):

// ==12345==ERROR: AddressSanitizer: heap-use-after-free on address 0x...
// READ of size 5 at 0x... thread T0
//   #0 0x... in std::operator<< <char, ...>(...)
//   #1 0x... in main test_dangling.cpp:42
// 0x... is located 0 bytes inside of 11-byte region
//   freed by thread T0 here:
//     #0 0x... in operator delete(void*)
//     #1 0x... in std::string::~string()
//     #2 0x... in main test_dangling.cpp:38  ← уничтожение временного
// previously allocated by thread T0 here:
//     #0 0x... in operator new(unsigned long)
//     #1 0x... in std::string::string(char const*)
//     #2 0x... in main test_dangling.cpp:38  ← создание временного


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
    // Этот тест выполняет опасный код,
    // потому что это UB и может привести к падению или порче памяти.
    //
    // Раскомментировать и запустить под ASan для демонстрации:
    //   cmake -S . -B build -DCMAKE_CXX_FLAGS="-fsanitize=address"
    //   cmake --build build && ctest --test-dir build
    //
    // {
    //     auto cmd = parse(std::string("/join #room"));
    //     // временная строка уничтожена, cmd содержит dangling views
    //     auto& j = std::get<JoinCommand>(*cmd);
    //     EXPECT_EQ(j.room, "#room");  // ASan: heap-use-after-free
    // }

    // Вместо этого показываю, что при правильной архитектуре
    // ошибка невозможна:

    std::string storage = "/join #room";
    auto cmd = parse(storage);
    ASSERT_TRUE(cmd.has_value());
    EXPECT_EQ(std::get<JoinCommand>(*cmd).room, "#room");
}