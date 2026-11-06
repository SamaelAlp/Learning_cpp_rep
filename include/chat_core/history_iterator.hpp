#pragma once

#include "chat_core/ring_buffer.hpp"

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace chat {

template <typename T, std::size_t N>
class RingBuffer<T, N>::iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;
    
    iterator() noexcept = default;
    
    iterator(RingBuffer* buf, std::size_t pos) noexcept
        : buf_(buf), pos_(pos) {}
    
    [[nodiscard]] reference operator*() const {
        return (*buf_)[pos_];
    }
    
    [[nodiscard]] pointer operator->() const {
        return &(*buf_)[pos_];
    }
    
    iterator& operator++() noexcept {
        ++pos_;
        return *this;
    }
    
    iterator operator++(int) noexcept {
        iterator tmp = *this;
        ++pos_;
        return tmp;
    }
    
    [[nodiscard]] bool operator==(const iterator& other) const noexcept {
        return buf_ == other.buf_ && pos_ == other.pos_;
    }
    
    [[nodiscard]] bool operator!=(const iterator& other) const noexcept {
        return !(*this == other);
    }

private:
    RingBuffer* buf_ = nullptr;
    std::size_t pos_ = 0;
};

template <typename T, std::size_t N>
class RingBuffer<T, N>::const_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;
    
    const_iterator() noexcept = default;
    
    const_iterator(const RingBuffer* buf, std::size_t pos) noexcept
        : buf_(buf), pos_(pos) {}
    
    const_iterator(const iterator& it) noexcept
        : buf_(it.buf_), pos_(it.pos_) {}
    
    [[nodiscard]] reference operator*() const {
        return (*buf_)[pos_];
    }
    
    [[nodiscard]] pointer operator->() const {
        return &(*buf_)[pos_];
    }
    
    const_iterator& operator++() noexcept {
        ++pos_;
        return *this;
    }
    
    const_iterator operator++(int) noexcept {
        const_iterator tmp = *this;
        ++pos_;
        return tmp;
    }
    
    [[nodiscard]] bool operator==(const const_iterator& other) const noexcept {
        return buf_ == other.buf_ && pos_ == other.pos_;
    }
    
    [[nodiscard]] bool operator!=(const const_iterator& other) const noexcept {
        return !(*this == other);
    }

private:
    const RingBuffer* buf_ = nullptr;
    std::size_t pos_ = 0;
};

template <typename T, std::size_t N>
typename RingBuffer<T, N>::iterator RingBuffer<T, N>::begin() noexcept {
    return iterator(this, 0);
}

template <typename T, std::size_t N>
typename RingBuffer<T, N>::iterator RingBuffer<T, N>::end() noexcept {
    return iterator(this, size_);
}

template <typename T, std::size_t N>
typename RingBuffer<T, N>::const_iterator RingBuffer<T, N>::begin() const noexcept {
    return const_iterator(this, 0);
}

template <typename T, std::size_t N>
typename RingBuffer<T, N>::const_iterator RingBuffer<T, N>::end() const noexcept {
    return const_iterator(this, size_);
}

template <typename T, std::size_t N>
typename RingBuffer<T, N>::const_iterator RingBuffer<T, N>::cbegin() const noexcept {
    return const_iterator(this, 0);
}

template <typename T, std::size_t N>
typename RingBuffer<T, N>::const_iterator RingBuffer<T, N>::cend() const noexcept {
    return const_iterator(this, size_);
}

}
