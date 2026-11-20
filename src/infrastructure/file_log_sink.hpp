#pragma once

#include "ilog_sink.hpp"

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

namespace chat {

class FileLogSink : public ILogSink {
public:
    explicit FileLogSink(const std::string& path)
        : path_(path)
        , file_(std::make_unique<std::ofstream>(path, std::ios::app))
    {
        if (!file_->is_open()) {
            throw std::runtime_error("Cannot open log file: " + path);
        }
    }

    FileLogSink(const FileLogSink&) = delete;
    FileLogSink& operator=(const FileLogSink&) = delete;
    FileLogSink(FileLogSink&&) noexcept = default;
    FileLogSink& operator=(FileLogSink&&) noexcept = default;

    ~FileLogSink() noexcept override {
        if (file_ && file_->is_open()) {
            file_->flush();
            file_->close();
        }
    }

    void append(const std::string& line) override {
        if (file_ && file_->is_open()) {
            *file_ << line << '\n';
        }
    }

    [[nodiscard]] const std::string& path() const noexcept { return path_; }

private:
    std::string path_;
    std::unique_ptr<std::ofstream> file_;
};

}