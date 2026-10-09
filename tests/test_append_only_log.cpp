#include "chat_core/append_only_log.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

using namespace chat;

namespace fs = std::filesystem;

class AppendOnlyLogTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_file_ = fs::temp_directory_path() / "test_log.txt";
        fs::remove(test_file_);
    }

    void TearDown() override {
        fs::remove(test_file_);
    }

    fs::path test_file_;
};

TEST_F(AppendOnlyLogTest, CreatesFileOnConstruction) {
    {
        AppendOnlyLog log(test_file_);
    }
    EXPECT_TRUE(fs::exists(test_file_));
}

TEST_F(AppendOnlyLogTest, ThrowsIfFileCannotBeOpened) {
    fs::path invalid_path = "/nonexistent/directory/log.txt";
    EXPECT_THROW(AppendOnlyLog log(invalid_path), std::runtime_error);
}

TEST_F(AppendOnlyLogTest, AppendsLines) {
    {
        AppendOnlyLog log(test_file_);
        log.append("line 1");
        log.append("line 2");
    }

    std::ifstream file(test_file_);
    std::string line1, line2;
    std::getline(file, line1);
    std::getline(file, line2);

    EXPECT_EQ(line1, "line 1");
    EXPECT_EQ(line2, "line 2");
}

TEST_F(AppendOnlyLogTest, ClosesOnDestruction) {
    {
        AppendOnlyLog log(test_file_);
        log.append("test");
    }
    std::ifstream file(test_file_);
    EXPECT_TRUE(file.good());
}

TEST_F(AppendOnlyLogTest, ClosesOnException) {
    try {
        AppendOnlyLog log(test_file_);
        log.append("before exception");
        throw std::runtime_error("test exception");
    } catch (...) {

    }

    std::ifstream file(test_file_);
    std::string line;
    std::getline(file, line);
    EXPECT_EQ(line, "before exception");
}

TEST_F(AppendOnlyLogTest, MoveTransfersOwnership) {
    AppendOnlyLog log1(test_file_);
    log1.append("from log1");

    AppendOnlyLog log2 = std::move(log1);
    log2.append("from log2");

    std::ifstream file(test_file_);
    std::string line1, line2;
    std::getline(file, line1);
    std::getline(file, line2);

    EXPECT_EQ(line1, "from log1");
    EXPECT_EQ(line2, "from log2");
}