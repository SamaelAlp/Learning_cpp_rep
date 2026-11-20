#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace chat {

template <typename T, std::size_t N>
class RingBuffer {
public:
    static_assert(N > 0, "RingBuffer capacity must be > 0");
    
    RingBuffer() : data_(std::make_unique<T[]>(N)) {}
    
    RingBuffer(const RingBuffer& other)
        : data_(std::make_unique<T[]>(N))
        , size_(other.size_)
        , head_(other.head_)
    {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }
    
    RingBuffer& operator=(const RingBuffer& other) {
        if (this != &other) {
            RingBuffer tmp(other);
            swap(*this, tmp);
        }
        return *this;
    }
    
    RingBuffer(RingBuffer&&) noexcept = default;
    RingBuffer& operator=(RingBuffer&&) noexcept = default;
    
    void push_back(T value) {
        std::size_t write_pos = (head_ + size_) % N;
        data_[write_pos] = std::move(value);
        
        if (size_ < N) {
            ++size_;
        } else {
            head_ = (head_ + 1) % N;
        }
    }
    
    template <typename... Args>
    T& emplace_back(Args&&... args) {
        std::size_t write_pos = (head_ + size_) % N;
        data_[write_pos] = T(std::forward<Args>(args)...);
        
        if (size_ < N) {
            ++size_;
        } else {
            head_ = (head_ + 1) % N;
        }
        return data_[write_pos];
    }
    
    [[nodiscard]] T& operator[](std::size_t i) {
        return data_[(head_ + i) % N];
    }
    
    [[nodiscard]] const T& operator[](std::size_t i) const {
        return data_[(head_ + i) % N];
    }
    
    [[nodiscard]] T& at(std::size_t i) {
        if (i >= size_) throw std::out_of_range("RingBuffer::at");
        return (*this)[i];
    }
    
    [[nodiscard]] const T& at(std::size_t i) const {
        if (i >= size_) throw std::out_of_range("RingBuffer::at");
        return (*this)[i];
    }
    
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] std::size_t capacity() const noexcept { return N; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] bool full() const noexcept { return size_ == N; }
    
    void clear() noexcept {
        size_ = 0;
        head_ = 0;
    }
    
    class iterator;
    class const_iterator;
    
    iterator begin() noexcept;
    iterator end() noexcept;
    [[nodiscard]] const_iterator begin() const noexcept;
    [[nodiscard]] const_iterator end() const noexcept;
    [[nodiscard]] const_iterator cbegin() const noexcept;
    [[nodiscard]] const_iterator cend() const noexcept;
    
    friend void swap(RingBuffer& a, RingBuffer& b) noexcept {
        using std::swap;
        swap(a.data_, b.data_);
        swap(a.size_, b.size_);
        swap(a.head_, b.head_);
    }

private:
    std::unique_ptr<T[]> data_;
    std::size_t size_ = 0;
    std::size_t head_ = 0;
};

}