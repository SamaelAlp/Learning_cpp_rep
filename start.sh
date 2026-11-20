# 1. Сборка
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# 2. Проверка, что domain собирается изолированно
# (должно пройти без ошибок — domain не зависит от Kafka)
cmake --build build --target chat_domain

# 3. Тесты проходят при выключенном брокере
lab-stand down
ctest --test-dir build --output-on-failure

# 4. Проверка, что domain/application не знают про Kafka
grep -rn "kafka\|Kafka\|lab::" src/domain src/application
# Ожидаем: пусто

# 5. Запуск с Kafka
lab-stand up
./build/chat_tail &        # в фоне
./build/chat_cli           # в другом терминале
# Выполняем команды: /nick alice, /join #general, /msg #general hello
# В окне chat_tail должны появиться события

# 6. Запуск без Kafka (fallback)
./build/chat_cli --no-kafka