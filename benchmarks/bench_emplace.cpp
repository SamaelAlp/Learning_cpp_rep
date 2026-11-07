// Бенчмарк: push_back vs emplace_back, insert vs try_emplace

#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct Instrumented {
    std::string a;
    std::string b;
    int c;
    
    static int ctor_count;
    static int copy_count;
    static int move_count;
    
    Instrumented(std::string a_, std::string b_, int c_)
        : a(std::move(a_)), b(std::move(b_)), c(c_) {
        ++ctor_count;
    }
    
    Instrumented(const Instrumented& other)
        : a(other.a), b(other.b), c(other.c) {
        ++copy_count;
    }
    
    Instrumented(Instrumented&& other) noexcept
        : a(std::move(other.a)), b(std::move(other.b)), c(other.c) {
        ++move_count;
    }
    
    Instrumented& operator=(const Instrumented&) = default;
    Instrumented& operator=(Instrumented&&) noexcept = default;
    
    static void reset() {
        ctor_count = copy_count = move_count = 0;
    }
};

int Instrumented::ctor_count = 0;
int Instrumented::copy_count = 0;
int Instrumented::move_count = 0;

int main() {
    constexpr int N = 100000;
    
    // ── push_back vs emplace_back ─────────────────────────────
    std::cout << "=== push_back vs emplace_back ===\n";
    
    Instrumented::reset();
    {
        std::vector<Instrumented> v;
        v.reserve(N);
        for (int i = 0; i < N; ++i) {
            v.push_back(Instrumented("a", "b", i));  // ctor + move
        }
    }
    std::cout << "push_back:    ctor=" << Instrumented::ctor_count
              << " copy=" << Instrumented::copy_count
              << " move=" << Instrumented::move_count << "\n";
    
    Instrumented::reset();
    {
        std::vector<Instrumented> v;
        v.reserve(N);
        for (int i = 0; i < N; ++i) {
            v.emplace_back("a", "b", i);  // только ctor
        }
    }
    std::cout << "emplace_back: ctor=" << Instrumented::ctor_count
              << " copy=" << Instrumented::copy_count
              << " move=" << Instrumented::move_count << "\n";
    
    // ── insert vs try_emplace для map ─────────────────────────
    std::cout << "\n=== insert vs try_emplace (map) ===\n";
    
    Instrumented::reset();
    {
        std::map<int, Instrumented> m;
        for (int i = 0; i < N; ++i) {
            m.insert({i, Instrumented("a", "b", i)});  // ctor + move
        }
    }
    std::cout << "insert:       ctor=" << Instrumented::ctor_count
              << " copy=" << Instrumented::copy_count
              << " move=" << Instrumented::move_count << "\n";
    
    Instrumented::reset();
    {
        std::map<int, Instrumented> m;
        for (int i = 0; i < N; ++i) {
            m.try_emplace(i, "a", "b", i);  // только ctor
        }
    }
    std::cout << "try_emplace:  ctor=" << Instrumented::ctor_count
              << " copy=" << Instrumented::copy_count
              << " move=" << Instrumented::move_count << "\n";
    
    return 0;
}