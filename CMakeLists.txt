cmake_minimum_required(VERSION 3.20)
project(chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)              # ← было 23
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
    GIT_TAG        v1.14.0
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(chat_core
    src/message.cpp
    src/owning_buffer.cpp
    src/chat_store.cpp
)
target_include_directories(chat_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(chat_cli src/chat_cli/main.cpp)
target_link_libraries(chat_cli PRIVATE chat_core)

enable_testing()
add_executable(chat_tests
    tests/test_message.cpp
    tests/test_owning_buffer.cpp
    tests/test_chat_store.cpp
)
target_link_libraries(chat_tests PRIVATE chat_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(chat_tests)