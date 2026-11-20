#pragma once

#include "chat_core/message.hpp"

#include <map>
#include <vector>

namespace chat {

class TimeIndex {
public:
    void insert(Timestamp ts, MessagePtr msg) {
        index_[ts].push_back(std::move(msg));
    }
    
    std::vector<MessagePtr> since(Timestamp since_ts) const {
        std::vector<MessagePtr> result;
        for (auto it = index_.lower_bound(since_ts); it != index_.end(); ++it) {
            for (const auto& msg : it->second) {
                result.push_back(msg);
            }
        }
        return result;
    }
    
    std::vector<MessagePtr> until(Timestamp until_ts) const {
        std::vector<MessagePtr> result;
        for (auto it = index_.begin(); it != index_.end() && it->first <= until_ts; ++it) {
            for (const auto& msg : it->second) {
                result.push_back(msg);
            }
        }
        return result;
    }
    
    [[nodiscard]] std::size_t size() const noexcept {
        std::size_t count = 0;
        for (const auto& [_, msgs] : index_) {
            count += msgs.size();
        }
        return count;
    }

private:
    std::map<Timestamp, std::vector<MessagePtr>> index_;
};

}