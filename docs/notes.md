#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <utility>

namespace chat {

// OwningBuffer на std::unique_ptr<char[]>.
//
// Сравнение с версией этапа 1 (правило пяти):
//   - Деструктор: = default (unique_ptr сам освобождает память)
//   - Перемещающие операции: = default (unique_ptr перемещается автоматически)
//   - Копирующие операции: остались, потому что unique_ptr не копируется
//   - swap: упрощён, можно использовать std::swap для полей
//
// Исчезло ~30 строк кода, и стали невозможны:
//   1. Утечка памяти при исключении в конструкторе
//   2. Double-free при неправильном перемещении
//   3. Забытый delete[] в деструкторе
//   4. Несоответствие new[]/delete[] (unique_ptr<char[]> всегда использует delete[])
//   5. Использование указателя после перемещения (unique_ptr обнуляется автоматически)
class OwningBuffer {
public:
    OwningBuffer() noexcept = default;

    explicit OwningBuffer(std::size_t size)
        : data_(std::make_unique<char[]>(size))
        , size_(size)
    {}

    // Деструктор: unique_ptr сам освободит память через delete[]
    ~OwningBuffer() noexcept = default;

    // Копирующий конструктор: unique_ptr не копируется, поэтому пишем вручную
    OwningBuffer(const OwningBuffer& other)
        : data_(std::make_unique<char[]>(other.size_))
        , size_(other.size_)
    {
        if (size_ > 0) {
            std::memcpy(data_.get(), other.data_.get(), size_);
        }
    }

    // Копирующее присваивание через copy-and-swap
    OwningBuffer& operator=(const OwningBuffer& other) {
        OwningBuffer tmp(other);
        swap(*this, tmp);
        return *this;
    }

    // Перемещающий конструктор: unique_ptr перемещается автоматически
    OwningBuffer(OwningBuffer&&) noexcept = default;

    // Перемещающее присваивание: unique_ptr перемещается автоматически
    OwningBuffer& operator=(OwningBuffer&&) noexcept = default;

    // swap: можно использовать std::swap для полей
    friend void swap(OwningBuffer& a, OwningBuffer& b) noexcept {
        using std::swap;
        swap(a.data_, b.data_);
        swap(a.size_, b.size_);
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] const char* data() const noexcept { return data_.get(); }
    [[nodiscard]] char* data() noexcept { return data_.get(); }

    void set(std::size_t i, char c) { data_[i] = c; }
    char get(std::size_t i) const   { return data_[i]; }

private:
    std::unique_ptr<char[]> data_;
    std::size_t size_ = 0;
};

} // namespace chat




Метод
Этап 1 (правило пяти)
Этап 4 (unique_ptr)
Разница
Поля
2 строки
2 строки
0
Конструктор по умолчанию
1 строка
1 строка
0
Конструктор с размером
2 строки
3 строки
+1
Деструктор
4 строки
1 строка
-3
Копирующий конструктор
6 строк
6 строк
0
Копирующее присваивание
5 строк
5 строк
0
Перемещающий конструктор
4 строки
1 строка
-3
Перемещающее присваивание
9 строк
1 строка
-8
swap
5 строк
5 строк
0
Геттеры/сеттеры
8 строк
8 строк
0
Итого
~80 строк
~50 строк
-30 строк
Исчезло ~30 строк кода.


## Домашнее задание этапа 4: OwningBuffer на unique_ptr

### Что изменилось

Перевели `OwningBuffer` с сырого указателя `char*` на `std::unique_ptr<char[]>`.

### Объём кода

- **Этап 1 (правило пяти):** ~80 строк
- **Этап 4 (unique_ptr):** ~50 строк
- **Исчезло:** ~30 строк

Основные сокращения:
- Деструктор: 4 строки → 1 строка (`= default`)
- Перемещающий конструктор: 4 строки → 1 строка (`= default`)
- Перемещающее присваивание: 9 строк → 1 строка (`= default`)

### Какие ошибки стали невозможны

1. **Утечка памяти при исключении в конструкторе**
   - Было: если конструктор бросил после `new[]`, память не освободится
   - Стало: `unique_ptr` уничтожается при раскрутке стека

2. **Double-free при неправильном перемещении**
   - Было: если забыли обнулить `other.data_` после перемещения
   - Стало: `unique_ptr` автоматически обнуляет источник

3. **Забытый `delete[]` в деструкторе**
   - Было: легко забыть написать `delete[] data_`
   - Стало: `= default` вызывает деструктор `unique_ptr` автоматически

4. **Несоответствие `new[]`/`delete[]`**
   - Было: можно случайно написать `delete data_` вместо `delete[] data_`
   - Стало: `unique_ptr<char[]>` всегда использует `delete[]`

5. **Использование указателя после перемещения**
   - Было: `data_` всё ещё указывает в старую память → UB
   - Стало: `data_` становится `nullptr` → краш вместо UB

### Вывод

`std::unique_ptr` — это **бесплатная страховка** от большинства ошибок ручного управления памятью. Код стал короче, безопаснее и понятнее. Единственное, что пришлось оставить вручную — копирующие операции, потому что `unique_ptr` не копируется (и это правильно: владение должно быть явным).