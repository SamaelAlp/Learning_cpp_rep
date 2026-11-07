// Бенчмарк: сравнение vector, deque, list для истории с вытеснением
// Профиль нагрузки: постоянное добавление в конец, вытеснение из начала,
// чтение последних N элементов.

#include <chrono>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

using MessagePtr = std::unique_ptr<std::string>;

MessagePtr make_msg(int i) {
    return std::make_unique<std::string>("Message #" + std::to_string(i));
}

// ── Вариант 1: std::vector с ручным вытеснением ───────────────
void bench_vector(int N, int iterations) {
    std::vector<MessagePtr> history;
    history.reserve(N);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        history.push_back(make_msg(i));
        if (history.size() > static_cast<std::size_t>(N)) {
            history.erase(history.begin());  // O(N) — дорого!
        }
    }
    
    // Чтение последних 100
    int sum = 0;
    for (std::size_t i = history.size() - 100; i < history.size(); ++i) {
        sum += static_cast<int>(history[i]->size());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "vector:  " << ms << " ms, sum=" << sum << "\n";
}

// ── Вариант 2: std::deque ─────────────────────────────────────
void bench_deque(int N, int iterations) {
    std::deque<MessagePtr> history;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        history.push_back(make_msg(i));
        if (history.size() > static_cast<std::size_t>(N)) {
            history.pop_front();  // O(1)
        }
    }
    
    int sum = 0;
    for (std::size_t i = history.size() - 100; i < history.size(); ++i) {
        sum += static_cast<int>(history[i]->size());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "deque:   " << ms << " ms, sum=" << sum << "\n";
}

// ── Вариант 3: std::list ──────────────────────────────────────
void bench_list(int N, int iterations) {
    std::list<MessagePtr> history;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        history.push_back(make_msg(i));
        if (history.size() > static_cast<std::size_t>(N)) {
            history.pop_front();  // O(1)
        }
    }
    
    // Чтение последних 100 — нужно пройти с начала, т.к. list не даёт random access
    int sum = 0;
    auto it = history.begin();
    std::advance(it, history.size() - 100);
    for (; it != history.end(); ++it) {
        sum += static_cast<int>((*it)->size());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "list:    " << ms << " ms, sum=" << sum << "\n";
}

int main() {
    std::cout << "=== История с вытеснением (capacity=1000, iterations=100000) ===\n";
    bench_vector(1000, 100000);
    bench_deque(1000, 100000);
    bench_list(1000, 100000);
    
    std::cout << "\n=== История с вытеснением (capacity=10000, iterations=1000000) ===\n";
    bench_vector(10000, 1000000);
    bench_deque(10000, 1000000);
    bench_list(10000, 1000000);
    
    return 0;
}