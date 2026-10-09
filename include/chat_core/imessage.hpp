#pragma once

#include <memory>
#include <string>

namespace chat {

class IMessage {
public:
    virtual ~IMessage() = default;

    [[nodiscard]] virtual std::string to_string() const = 0;

    [[nodiscard]] virtual std::string type_name() const = 0;

    [[nodiscard]] virtual std::unique_ptr<IMessage> clone() const = 0;
};

}