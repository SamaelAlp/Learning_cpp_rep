#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <utility>

namespace chat {

class OwningBuffer {
public:
    OwningBuffer() noexcept = default;

    explicit OwningBuffer(std::size_t size)
        : data_(new char[size]()), size_(size)
    {}

    ~OwningBuffer() noexcept {
        delete[] data_;
    }

    OwningBuffer(const OwningBuffer& other)
        : data_(new char[other.size_]), size_(other.size_)
    {
        if (size_ > 0) {
            std::memcpy(data_, other.data_, size_);
        }
    }

    OwningBuffer& operator=(const OwningBuffer& other) {
        OwningBuffer tmp(other);
        swap(*this, tmp);
        return *this;
    }

    OwningBuffer(OwningBuffer&& other) noexcept
        : data_(std::exchange(other.data_, nullptr))
        , size_(std::exchange(other.size_, 0))
    {}

    OwningBuffer& operator=(OwningBuffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = std::exchange(other.data_, nullptr);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }

    friend void swap(OwningBuffer& a, OwningBuffer& b) noexcept {
        using std::swap;
        swap(a.data_, b.data_);
        swap(a.size_, b.size_);
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] const char* data() const noexcept { return data_; }
    [[nodiscard]] char* data() noexcept { return data_; }

    void set(std::size_t i, char c) { data_[i] = c; }
    char get(std::size_t i) const   { return data_[i]; }

private:
    char*       data_ = nullptr;
    std::size_t size_ = 0;
};

}