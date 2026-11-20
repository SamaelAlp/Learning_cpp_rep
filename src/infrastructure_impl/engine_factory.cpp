#include "hash_engine.hpp"
#include "ichat_store.hpp"
#include "std_engine.hpp"

#include <stdexcept>

namespace chat {

std::unique_ptr<IChatStore> make_engine(const std::string& name) {
    if (name == "std" || name == "map") {
        return std::make_unique<StdEngine>();
    }
    if (name == "hash" || name == "hashtable") {
        return std::make_unique<HashEngine>();
    }
    throw std::invalid_argument("Unknown engine: " + name + ". Use 'std' or 'hash'.");
}

}