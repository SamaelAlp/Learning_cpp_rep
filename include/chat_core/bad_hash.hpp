#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace chat {

struct BadHash {
    using hash_type = std::uint64_t;
    
    hash_type operator()(std::string_view sv) const noexcept {
        hash_type sum = 0;
        for (unsigned char c : sv) {
            sum += static_cast<hash_type>(c);
        }
        return sum;
    }
};

}