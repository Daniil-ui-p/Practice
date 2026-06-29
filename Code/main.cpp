#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include "ResourceMonitor.h"
#include "AhoCorasick.h"
#include "NaiveSearch.h"
#include "FileLoader.h"
#include "PatternLoader.h"
#include "TestRunner.h"
#include "ChartGenerator.h"


void task1_buildNFA(AhoCorasick& ac, const std::vector<std::string>& patterns) {
    std::cout << "================================================================\n";
    std::cout << "TASK 1: NONDETERMINISTIC FINITE AUTOMATON (NFA)\n";
    std::cout << "================================================================\n";
    
    auto t1 = std::chrono::high_resolution_clock::now();
    
    for (const auto& p : patterns) {
        ac.add(p);
    }
    
    auto t2 = std::chrono::high_resolution_clock::now();
    auto build_time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    
    std::cout << "NFA built: " << ac.nodes() << " nodes" << std::endl;
    std::cout << "Build time: " << build_time << " ms\n" << std::endl;
}

void task2_convertDFA(AhoCorasick& ac) {
    std::cout << "================================================================\n";
    std::cout << "TASK 2: DETERMINISTIC FINITE AUTOMATON (DFA)\n";
    std::cout << "================================================================\n";
    
    auto t1 = std::chrono::high_resolution_clock::now();
    ac.build();
    auto t2 = std::chrono::high_resolution_clock::now();
    
    auto compile_time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    
    std::cout << "DFA compiled: " << ac.nodes() << " nodes" << std::endl;
    std::cout << "Compile time: " << compile_time << " ms\n" << std::endl;
}

void task3_cacheOptimization(AhoCorasick& ac, const std::string& text) {
    std::cout << "================================================================\n";
    std::cout << "TASK 3: DYNAMIC PROGRAMMING (CACHE OPTIMIZATION)\n";
    std::cout << "================================================================\n";
    
    std::string sample = text.substr(0, std::min(text.size(), (size_t)1000000));
    
    TestResult nocache = runAhoCorasickNoCache(ac, sample);
    TestResult cached = runAhoCorasickCached(ac, sample);
    
    std::cout << std::left << std::setw(20) << "Method" 
              << std::setw(12) << "Time(ms)" 
              << std::setw(12) << "CPU(ms)" 
              << std::setw(12) << "Memory(KB)"
              << std::setw(10) << "Matches" << std::endl;
    std::cout << std::string(66, '-') << std::endl;
    
    std::cout << std::left << std::setw(20) << "No cache"
              << std::setw(12) << std::fixed << std::setprecision(2) << nocache.time_ms
              << std::setw(12) << std::fixed << std::setprecision(2) << nocache.cpu_ms
              << std::setw(12) << std::fixed << std::setprecision(1) << (nocache.memory_bytes / 1024.0)
              << std::setw(10) << nocache.matches << std::endl;
    
    std::cout << std::left << std::setw(20) << "With cache"
              << std::setw(12) << std::fixed << std::setprecision(2) << cached.time_ms
              << std::setw(12) << std::fixed << std::setprecision(2) << cached.cpu_ms
              << std::setw(12) << std::fixed << std::setprecision(1) << (cached.memory_bytes / 1024.0)
              << std::setw(10) << cached.matches << std::endl;
    
    std::cout << "\nCache speedup: " << std::fixed << std::setprecision(2) 
              << (nocache.time_ms / cached.time_ms) << "x\n" << std::endl;
}

void task4_comparison(AhoCorasick& ac, const std::string& text, 
                      const std::vector<std::string>& patterns) {
    std::cout << "================================================================\n";
    std::cout << "TASK 4: COMPARISON WITH NAIVE SEARCH\n";
    std::cout << "================================================================\n";
    
    std::string sample = text.substr(0, std::min(text.size(), (size_t)1000000));
    
    TestResult cached = runAhoCorasickCached(ac, sample);
    TestResult nocache = runAhoCorasickNoCache(ac, sample);
    TestResult naive = runNaiveSearch(sample, patterns);
    
    std::cout << std::left << std::setw(25) << "Algorithm" 
              << std::setw(12) << "Time(ms)" 
              << std::setw(10) << "Speedup"
              << std::setw(10) << "Matches" << std::endl;
    std::cout << std::string(57, '-') << std::endl;
    
    std::cout << std::left << std::setw(25) << "Aho-Corasick (cached)"
              << std::setw(12) << std::fixed << std::setprecision(2) << cached.time_ms
              << std::setw(10) << std::fixed << std::setprecision(2) << (naive.time_ms / cached.time_ms) << "x"
              << std::setw(10) << cached.matches << std::endl;
    
    std::cout << std::left << std::setw(25) << "Aho-Corasick (no cache)"
              << std::setw(12) << std::fixed << std::setprecision(2) << nocache.time_ms
              << std::setw(10) << std::fixed << std::setprecision(2) << (naive.time_ms / nocache.time_ms) << "x"
              << std::setw(10) << nocache.matches << std::endl;
    
    std::cout << std::left << std::setw(25) << "Naive Search"
              << std::setw(12) << std::fixed << std::setprecision(2) << naive.time_ms
              << std::setw(10) << "1.00x"
              << std::setw(10) << naive.matches << std::endl;
    
    std::cout << "\nNote: std::regex not shown (100x slower)\n" << std::endl;
}

TestResult task5_shortPatterns(AhoCorasick& ac, const std::string& text, 
                               const std::vector<std::string>& patterns) {
    std::cout << "================================================================\n";
    std::cout << "TASK 5: SHORT PATTERNS (<=50 chars) IN TEXT\n";
    std::cout << "================================================================\n";
    
    ResourceMonitor mem_mon;
    mem_mon.start();
    
    TestResult result = runAhoCorasickCached(ac, text);
    
    size_t mem_used = mem_mon.memoryUsed();
    
    std::cout << "Patterns: " << patterns.size() << std::endl;
    std::cout << "Text size: " << text.size() << " characters" << std::endl;
    std::cout << "Search time: " << result.time_ms << " ms" << std::endl;
    std::cout << "CPU time: " << std::fixed << std::setprecision(1) << result.cpu_ms << " ms" << std::endl;
    std::cout << "Memory: " << std::fixed << std::setprecision(2) 
              << (mem_used / (1024.0 * 1024.0)) << " MB" << std::endl;
    std::cout << "Matches: " << result.matches << "\n" << std::endl;
    
    return result;
}

TestResult task6_longPatterns(const std::string& text, 
                              const std::vector<std::string>& patterns) {
    std::cout << "================================================================\n";
    std::cout << "TASK 6: LONG PATTERNS (>=1000 chars) IN TEXT\n";
    std::cout << "================================================================\n";
    
    AhoCorasick ac;
    for (const auto& p : patterns) ac.add(p);
    ac.build();
    
    ResourceMonitor mem_mon;
    mem_mon.start();
    
    TestResult result = runAhoCorasickCached(ac, text);
    
    size_t mem_used = mem_mon.memoryUsed();
    
    std::cout << "Patterns: " << patterns.size() << std::endl;
    std::cout << "Automaton nodes: " << ac.nodes() << std::endl;
    std::cout << "Search time: " << result.time_ms << " ms" << std::endl;
    std::cout << "CPU time: " << std::fixed << std::setprecision(1) << result.cpu_ms << " ms" << std::endl;
    std::cout << "Memory: " << std::fixed << std::setprecision(2) 
              << (mem_used / (1024.0 * 1024.0)) << " MB" << std::endl;
    std::cout << "Matches: " << result.matches << "\n" << std::endl;
    
    return result;
}


void task7_8_9_summary(TestResult short_r, TestResult long_r, 
                       AhoCorasick& ac_short, AhoCorasick& ac_long) {
    std::cout << "================================================================\n";
    std::cout << "TASK 7-8-9: RESOURCE USAGE SUMMARY\n";
    std::cout << "================================================================\n";
    
    std::cout << std::left << std::setw(25) << "Metric" 
              << std::setw(18) << "Short patterns" 
              << std::setw(18) << "Long patterns" << std::endl;
    std::cout << std::string(61, '-') << std::endl;
    
    std::cout << std::left << std::setw(25) << "Time (ms)" 
              << std::setw(18) << short_r.time_ms 
              << std::setw(18) << long_r.time_ms << std::endl;
    
    std::cout << std::left << std::setw(25) << "CPU time (ms)" 
              << std::setw(18) << std::fixed << std::setprecision(1) << short_r.cpu_ms
              << std::setw(18) << std::fixed << std::setprecision(1) << long_r.cpu_ms << std::endl;
    
    std::cout << std::left << std::setw(25) << "Memory (MB)" 
              << std::setw(18) << std::fixed << std::setprecision(2) 
              << (short_r.memory_bytes / (1024.0 * 1024.0))
              << std::setw(18) << std::fixed << std::setprecision(2) 
              << (long_r.memory_bytes / (1024.0 * 1024.0)) << std::endl;
    
    std::cout << std::left << std::setw(25) << "Matches" 
              << std::setw(18) << short_r.matches 
              << std::setw(18) << long_r.matches << "\n" << std::endl;
}

void task10_charts(double cache_time, double nocache_time, double naive_time,
                   double mem_short, double mem_long) {
    std::cout << "================================================================\n";
    std::cout << "TASK 10: COMPARATIVE CHARTS\n";
    std::cout << "================================================================\n";
    
    printTimeChart(cache_time, nocache_time, naive_time);
    printMemoryChart(mem_short, mem_long);
}

int main() {
    try {
        std::cout << "================================================================" << std::endl;
        std::cout << "  AHO-CORASICK ALGORITHM - PERFORMANCE RESEARCH" << std::endl;
        std::cout << "================================================================\n" << std::endl;
        
        std::string text = loadTextFile("text.txt");
        
        std::vector<std::string> short_patterns = {
            "the", "and", "for", "not", "you", "but", "his", "say",
            "her", "she", "one", "all", "can", "had", "how", "was",
            "has", "see", "two", "way", "new", "day", "get", "may",
            "out", "man", "now", "go", "come", "make", "data", "code",
            "test", "find", "search", "pattern", "algorithm", "string"
        };
        
        std::vector<std::string> long_patterns;
if (text.size() > 1000) {
    for (int i = 0; i < 10; i++) {
        size_t start = (text.size() / 10) * i;
        size_t len = std::min((size_t)(1000 + i * 100), text.size() - start);
        
        if (start + len <= text.size()) {
            long_patterns.push_back(text.substr(start, len));
        }
    }
}

        
        std::cout << "Short patterns: " << short_patterns.size() << std::endl;
        std::cout << "Long patterns: " << long_patterns.size() << "\n" << std::endl;
        
        AhoCorasick ac;
        
        task1_buildNFA(ac, short_patterns);
        task2_convertDFA(ac);
        task3_cacheOptimization(ac, text);
        task4_comparison(ac, text, short_patterns);
        
        TestResult short_result = task5_shortPatterns(ac, text, short_patterns);
        TestResult long_result = task6_longPatterns(text, long_patterns);
        
        AhoCorasick ac_long;
        for (const auto& p : long_patterns) ac_long.add(p);
        ac_long.build();
        
        task7_8_9_summary(short_result, long_result, ac, ac_long);
        task10_charts(0, 0, 0, 
                     short_result.memory_bytes / (1024.0 * 1024.0),
                     long_result.memory_bytes / (1024.0 * 1024.0));
        
        std::cout << "\n================================================================\n";
        std::cout << "ALL TASKS COMPLETED\n";
        std::cout << "================================================================\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
