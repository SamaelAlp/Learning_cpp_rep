#include "chat_core/chat_store.hpp"

namespace chat {

const RingBuffer<MessagePtr, Room::HISTORY_CAPACITY> ChatStore::empty_messages_{};

}