rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# Тесты (оба движка)
ctest --test-dir build --output-on-failure


# CLI с выбором движка
#./build/chat_cli --engine=std
./build/chat_cli --engine=hash