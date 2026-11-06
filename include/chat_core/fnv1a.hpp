#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace chat {

struct Fnv1aHash {
    using hash_type = std::uint64_t;
    
    static constexpr hash_type offset_basis = 14695981039346656037ULL;
    static constexpr hash_type prime        = 1099511628211ULL;
    
    hash_type operator()(std::string_view sv) const noexcept {
        hash_type hash = offset_basis;
        for (unsigned char c : sv) {
            hash ^= static_cast<hash_type>(c);
            hash *= prime;
        }
        return hash;
    }
};

}