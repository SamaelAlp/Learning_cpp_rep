// Бенчмарк: map vs unordered_map
// Профиль: вставка, поиск, обход в алфавитном порядке.

#include <chrono>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <unordered_map>

std::string random_string(std::mt19937& rng, int len) {
    static const char chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::string s(len, ' ');
    for (int i = 0; i < len; ++i) {
        s[i] = chars[rng() % (sizeof(chars) - 1)];
    }
    return s;
}

template <typename Map>
void bench_insert(Map& m, const std::vector<std::string>& keys) {
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& k : keys) {
        m[k] = k.size();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "  insert: " << ms << " ms\n";
}

template <typename Map>
void bench_lookup(Map& m, const std::vector<std::string>& keys) {
    auto start = std::chrono::high_resolution_clock::now();
    int sum = 0;
    for (const auto& k : keys) {
        auto it = m.find(k);
        if (it != m.end()) sum += it->second;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "  lookup: " << ms << " ms, sum=" << sum << "\n";
}

template <typename Map>
void bench_traverse(const Map& m) {
    auto start = std::chrono::high_resolution_clock::now();
    int sum = 0;
    for (const auto& [k, v] : m) {
        sum += static_cast<int>(k.size()) + v;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "  traverse: " << ms << " ms, sum=" << sum << "\n";
}

int main() {
    std::mt19937 rng(42);
    
    for (int N : {1000, 100000, 1000000}) {
        std::cout << "\n=== N = " << N << " ===\n";
        
        std::vector<std::string> keys;
        keys.reserve(N);
        for (int i = 0; i < N; ++i) {
            keys.push_back(random_string(rng, 10));
        }
        
        std::cout << "std::map:\n";
        std::map<std::string, int> m1;
        bench_insert(m1, keys);
        bench_lookup(m1, keys);
        bench_traverse(m1);
        
        std::cout << "std::unordered_map:\n";
        std::unordered_map<std::string, int> m2;
        bench_insert(m2, keys);
        bench_lookup(m2, keys);
        bench_traverse(m2);
    }
    
    // Худший случай для unordered_map: ключи с одинаковым хешем
    std::cout << "\n=== Худший случай для unordered_map (коллизии) ===\n";
    std::vector<std::string> collision_keys;
    for (int i = 0; i < 10000; ++i) {
        // Все ключи дают одинаковый хеш в стандартной реализации
        collision_keys.push_back(std::string(10, 'a') + std::to_string(i));
    }
    
    std::unordered_map<std::string, int> bad_map;
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& k : collision_keys) {
        bad_map[k] = 1;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "  unordered_map (collisions): " << ms << " ms\n";
    
    return 0;
}