## Эксперимент с модулями C++20

**Цель:** Перевести `include/chat_core/concepts.hpp` в модуль C++20.

**Что было сделано:**
1. Создан файл `chat_core_concepts.cppm`:
   ```cpp
   export module chat_core.concepts;
   import <concepts>;
   import <string>;
   import <string_view>;
   import <vector>;
   import <byte>;

   export namespace chat {
       template <typename T>
       concept NickLike = requires(T t) {
           { std::string_view(t) } -> std::convertible_to<std::string_view>;
           requires std::equality_comparable<T>;
       };
   }