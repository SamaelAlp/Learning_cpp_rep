#include "chat_core/errors.hpp"

#include <iostream>

namespace chat {

void print_exception_chain(const std::exception& e, int level) {
    for (int i = 0; i < level; ++i) std::cout << "  ";
    std::cout << "└─ " << e.what() << "\n";

    if (auto nested = dynamic_cast<const std::nested_exception*>(&e)) {
        if (nested->nested_ptr()) {
            try {
                std::rethrow_exception(nested->nested_ptr());
            } catch (const std::exception& inner) {
                print_exception_chain(inner, level + 1);
            }
        }
    }
}

}