#pragma once

#include <cstdio>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace chat {

class AppendOnlyLog {
public:
    explicit AppendOnlyLog(const std::filesystem::path& path)
        : file_(std::fopen(path.string().c_str(), "a"))
    {
        if (!file_) {
            throw std::runtime_error("Failed to open log file: " + path.string());
        }
    }

    ~AppendOnlyLog() noexcept {
        if (file_) {
            std::fflush(file_);
            std::fclose(file_);
        }
    }

    AppendOnlyLog(const AppendOnlyLog&) = delete;
    AppendOnlyLog& operator=(const AppendOnlyLog&) = delete;

    AppendOnlyLog(AppendOnlyLog&& other) noexcept
        : file_(std::exchange(other.file_, nullptr))
    {}

    AppendOnlyLog& operator=(AppendOnlyLog&& other) noexcept {
        if (this != &other) {
            if (file_) {
                std::fflush(file_);
                std::fclose(file_);
            }
            file_ = std::exchange(other.file_, nullptr);
        }
        return *this;
    }

    void append(std::string_view line) {
        if (!file_) {
            throw std::runtime_error("Log file is not open");
        }
        std::fwrite(line.data(), 1, line.size(), file_);
        std::fputc('\n', file_);
        std::fflush(file_);
    }

private:
    std::FILE* file_;
};

}