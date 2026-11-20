#include "chat_core/bad_hash.hpp"
#include "chat_core/fnv1a.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace chat;

template <typename Hash>
void print_distribution(const std::string& name, const std::vector<std::string>& keys) {
    Hash hash;
    std::unordered_map<uint64_t, int> hash_counts;
    
    for (const auto& key : keys) {
        hash_counts[hash(key)]++;
    }
    
    // Гистограмма длин цепочек
    std::vector<int> chain_lengths;
    for (const auto& [_, count] : hash_counts) {
        chain_lengths.push_back(count);
    }
    std::sort(chain_lengths.begin(), chain_lengths.end());
    
    std::cout << "\n=== " << name << " ===\n";
    std::cout << "Unique hashes: " << hash_counts.size() << " / " << keys.size() << "\n";
    std::cout << "Collisions: " << (keys.size() - hash_counts.size()) << "\n";
    
    if (!chain_lengths.empty()) {
        std::cout << "Chain length: min=" << chain_lengths.front()
                  << " max=" << chain_lengths.back()
                  << " avg=" << (chain_lengths.size() > 0 
                                 ? static_cast<double>(keys.size()) / hash_counts.size() 
                                 : 0.0) << "\n";
        
        // Топ-5 самых длинных цепочек
        std::cout << "Top chains: ";
        for (std::size_t i = 0; i < std::min<std::size_t>(5, chain_lengths.size()); ++i) {
            std::cout << chain_lengths[chain_lengths.size() - 1 - i] << " ";
        }
        std::cout << "\n";
    }
}

int main() {
    // Генерируем 100000 ников
    std::vector<std::string> nicks;
    nicks.reserve(100000);
    for (int i = 0; i < 100000; ++i) {
        nicks.push_back("user" + std::to_string(i));
    }
    
    print_distribution<Fnv1aHash>("FNV-1a", nicks);
    print_distribution<BadHash>("BadHash", nicks);
    
    return 0;
}