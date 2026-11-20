#pragma once

#include "chat_core/command_handler.hpp"
#include "chat_core/convert.hpp"
#include "chat_core/function_traits.hpp"

#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace chat {


class CommandRegistry {
public:
    using Handler = std::function<std::string(std::string_view, const std::vector<std::string_view>&)>;
    
    template <typename F>
    void register_command(std::string name, F&& handler) {
        using Traits = function_traits<std::decay_t<F>>;
        constexpr auto arity = Traits::arity;
        
        static_assert(CommandHandler<std::decay_t<F>>,
            "Handler must be callable and return string-convertible type");
        
        handlers_[name] = [handler = std::forward<F>(handler)](
            [[maybe_unused]] std::string_view input,
            const std::vector<std::string_view>& tokens
        ) -> std::string {

            if (tokens.size() - 1 != arity) {
                return "Wrong number of arguments. Expected " + std::to_string(arity);
            }
            
            return invoke_with_args(handler, tokens, std::make_index_sequence<arity>{});
        };
    }
    
    std::string execute(std::string_view input, const std::vector<std::string_view>& tokens) {
        if (tokens.empty()) {
            return "Empty command";
        }
        
        std::string cmd_name(tokens[0]);
        auto it = handlers_.find(cmd_name);
        if (it == handlers_.end()) {
            return "Unknown command: " + cmd_name;
        }
        
        try {
            return it->second(input, tokens);
        } catch (const std::exception& e) {
            return std::string("Error: ") + e.what();
        }
    }
    
    bool has_command(const std::string& name) const {
        return handlers_.contains(name);
    }

private:
    std::map<std::string, Handler> handlers_;
    
    template <typename F, std::size_t... Is>
    static std::string invoke_with_args(
        F& handler,
        const std::vector<std::string_view>& tokens,
        std::index_sequence<Is...>
    ) {
        using Traits = function_traits<std::decay_t<F>>;
        
        return handler(convert<typename Traits::template arg_type<Is>>(tokens[Is + 1])...);
    }
};

}