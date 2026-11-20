cmake_minimum_required(VERSION 3.28)
project(chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wshadow)
endif()

# ── GoogleTest ────────────────────────────────────────────────
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

# ── lab_kafka ─────────────────────────────────────────────────
find_package(lab_kafka REQUIRED)

# ═══════════════════════════════════════════════════════════════
# СЛОЙ 1: domain
# ═══════════════════════════════════════════════════════════════
add_library(chat_domain INTERFACE)
target_include_directories(chat_domain INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/domain
)
target_compile_features(chat_domain INTERFACE cxx_std_20)

# ═══════════════════════════════════════════════════════════════
# СЛОЙ 2: application
# ═══════════════════════════════════════════════════════════════
add_library(chat_application
    src/application_impl/chat_service.cpp
    src/application_impl/command_registry.cpp
    src/application_impl/lexer.cpp
)
target_include_directories(chat_application PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/domain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application
)
target_link_libraries(chat_application PUBLIC chat_domain)

# ═══════════════════════════════════════════════════════════════
# СЛОЙ 3: infrastructure
# ═══════════════════════════════════════════════════════════════
add_library(chat_infrastructure
    src/infrastructure_impl/std_engine.cpp
    src/infrastructure_impl/hash_engine.cpp
    src/infrastructure_impl/file_log_sink.cpp
    src/infrastructure_impl/kafka_event_publisher.cpp
    src/infrastructure_impl/engine_factory.cpp
    src/infrastructure_impl/message.cpp
)
target_include_directories(chat_infrastructure PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/domain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application
    ${CMAKE_CURRENT_SOURCE_DIR}/src/infrastructure
)
target_link_libraries(chat_infrastructure
    PUBLIC chat_application
    PRIVATE lab::kafka
)

# ── Консольное приложение ─────────────────────────────────────
add_executable(chat_cli src/chat_cli/main.cpp)
target_include_directories(chat_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/domain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application
    ${CMAKE_CURRENT_SOURCE_DIR}/src/infrastructure
)
target_link_libraries(chat_cli PRIVATE chat_infrastructure)

# ── Потребитель событий (chat_tail) ───────────────────────────
add_executable(chat_tail src/chat_tail/main.cpp)
target_include_directories(chat_tail PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/domain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application
    ${CMAKE_CURRENT_SOURCE_DIR}/src/infrastructure
)
target_link_libraries(chat_tail PRIVATE lab::kafka)
# ── Тесты ─────────────────────────────────────────────────────
enable_testing()

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
set(UNIT_TEST_SOURCES "")
foreach(test_file ${TEST_SOURCES})
    get_filename_component(filename "${test_file}" NAME)
    if(NOT filename STREQUAL "test_alloc.cpp" AND
       NOT filename STREQUAL "test_compile_fail.cpp")
        list(APPEND UNIT_TEST_SOURCES "${test_file}")
    endif()
endforeach()

add_executable(chat_tests ${UNIT_TEST_SOURCES})
target_link_libraries(chat_tests PRIVATE chat_infrastructure GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(chat_tests)

target_include_directories(chat_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/domain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application
    ${CMAKE_CURRENT_SOURCE_DIR}/src/infrastructure
)

# ── Compile-fail тест ─────────────────────────────────────────
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_compile_fail.cpp)
    add_executable(chat_compile_fail_test EXCLUDE_FROM_ALL
        tests/test_compile_fail.cpp)
    target_link_libraries(chat_compile_fail_test PRIVATE chat_infrastructure)
    add_test(NAME compile_fail_test
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
                --target chat_compile_fail_test)
    set_tests_properties(compile_fail_test PROPERTIES WILL_FAIL TRUE)
endif()