cmake_minimum_required(VERSION 4.0)
project(chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wshadow)
endif()

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    FIND_PACKAGE_ARGS NAMES GTest
)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_library(chat_core
    src/message.cpp
    src/owning_buffer.cpp
    src/chat_store.cpp
    src/lexer.cpp
    src/command_registry.cpp
)
target_include_directories(chat_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(chat_core PUBLIC cxx_std_20)

add_executable(chat_cli src/chat_cli/main.cpp)
target_link_libraries(chat_cli PRIVATE chat_core)

enable_testing()
add_executable(chat_tests
    tests/test_message.cpp
    tests/test_owning_buffer.cpp
    tests/test_chat_store.cpp
    tests/test_lexer.cpp
    tests/test_dangling.cpp
    tests/test_concepts.cpp
    tests/test_ranges.cpp
    tests/test_message_id.cpp
    tests/test_dangling_range.cpp
    tests/test_convert.cpp
    tests/test_command_registry.cpp
)
target_link_libraries(chat_tests PRIVATE chat_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(chat_tests)

# ── Compile-fail тест ───
# Эта цель НЕ добавляется в ALL, поэтому основная сборка не падает.
# Мы используем её только для проверки, что код действительно не компилируется.
add_executable(chat_compile_fail_test EXCLUDE_FROM_ALL tests/test_compile_fail.cpp)
target_link_libraries(chat_compile_fail_test PRIVATE chat_core)

# CTest-тест: пытается собрать цель и ожидает, что сборка упадёт.
# Если сборка пройдёт — тест считается проваленным.
add_test(
    NAME compile_fail_test
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target chat_compile_fail_test
)
set_tests_properties(compile_fail_test PROPERTIES WILL_FAIL TRUE)