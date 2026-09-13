#pragma once

#include <cstddef>

namespace chat {

struct Spy {
    static std::size_t copy_ctor_count;
    static std::size_t move_ctor_count;
    static std::size_t copy_assign_count;
    static std::size_t move_assign_count;

    Spy() noexcept = default;

    Spy(const Spy&) noexcept {
        ++copy_ctor_count;
    }

    Spy(Spy&&) noexcept {
        ++move_ctor_count;
    }

    Spy& operator=(const Spy&) noexcept {
        ++copy_assign_count;
        return *this;
    }

    Spy& operator=(Spy&&) noexcept {
        ++move_assign_count;
        return *this;
    }

    static void reset() noexcept {
        copy_ctor_count = 0;
        move_ctor_count = 0;
        copy_assign_count = 0;
        move_assign_count = 0;
    }
};

}