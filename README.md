## Команды чата

| Команда | Описание |
|---------|----------|
| `/join <room>` | Создать/войти в комнату |
| `/leave <room>` | Покинуть комнату |
| `/msg <room> <text>` | Отправить сообщение (text = остаток строки) |
| `/who` | Список комнат |
| `/history <room>` | История сообщений |
| `/type <room> <idx>` | Тип тела сообщения |
| `/react <room> <idx>` | Увеличить счётчик реакции |
| `/quit` | Выход |

## Сборка и запуск

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/chat_cli