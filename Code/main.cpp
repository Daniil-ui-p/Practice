#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "TestConfig.h"
#include "DataGenerator.h"
#include "AhoCorasick.h"
#include "NaiveSearch.h"
#include "Validator.h"
#include "Benchmark.h"

int main() {
    try {
        std::cout << "============================================================" << std::endl;
        std::cout << "  AHO-CORASICK ALGORITHM OPTIMIZATION & PERFORMANCE RESEARCH" << std::endl;
        std::cout << "============================================================" << std::endl;
        
        DataGenerator generator;
        Benchmark benchmark;
        
        // ============ ЭТАП 1: Генерация тестовых данных ============
        std::cout << "\n============================================================" << std::endl;
        std::cout << "STAGE 1: Generating test data files" << std::endl;
        std::cout << "============================================================" << std::endl;
        
        std::cout << "\nGenerating short patterns..." << std::endl;
        auto short_patterns_100 = generator.generateShortPatterns(100);
        generator.savePatternsToFile("patterns_short_100.txt", short_patterns_100);
        
        auto short_patterns_500 = generator.generateShortPatterns(500);
        generator.savePatternsToFile("patterns_short_500.txt", short_patterns_500);
        
        auto short_patterns_1000 = generator.generateShortPatterns(1000);
        generator.savePatternsToFile("patterns_short_1000.txt", short_patterns_1000);
        
        std::cout << "\nGenerating long patterns..." << std::endl;
        auto long_patterns_10 = generator.generateLongPatterns(10);
        generator.savePatternsToFile("patterns_long_10.txt", long_patterns_10);
        
        auto long_patterns_50 = generator.generateLongPatterns(50);
        generator.savePatternsToFile("patterns_long_50.txt", long_patterns_50);
        
        auto long_patterns_100 = generator.generateLongPatterns(100);
        generator.savePatternsToFile("patterns_long_100.txt", long_patterns_100);
        
        std::cout << "\nGenerating text files..." << std::endl;
        
        std::string text_1mb = generator.generateText(TestConfig::TEXT_SMALL);
        generator.saveTextToFile("text_1mb.txt", text_1mb);
        
        std::string text_10mb = generator.generateText(TestConfig::TEXT_MEDIUM);
        generator.saveTextToFile("text_10mb.txt", text_10mb);
        
        generator.saveLargeTextToFile("text_100mb.txt", TestConfig::TEXT_LARGE);
        
        std::cout << "\n[OK] All test data generated successfully!\n" << std::endl;
        
        // ============ ЭТАП 2: Валидация алгоритма ============
        std::cout << "============================================================" << std::endl;
        std::cout << "STAGE 2: Algorithm validation" << std::endl;
        std::cout << "============================================================" << std::endl;
        
        {
            std::cout << "\nValidating algorithm correctness..." << std::endl;
            
            std::string test_text = "abracadabra abacaba test pattern matching example";
            std::vector<std::string> test_patterns = {"abra", "aba", "cad", "test", "pattern", "match"};
            
            std::cout << "  Test text: \"" << test_text << "\"" << std::endl;
            std::cout << "  Test patterns: ";
            for (const auto& p : test_patterns) std::cout << "\"" << p << "\" ";
            std::cout << std::endl;
            
            AhoCorasick ac;
            for (const auto& p : test_patterns) ac.addPattern(p);
            ac.compile();
            
            auto ac_matches = ac.search(test_text);
            auto naive_matches = NaiveSearch::search(test_text, test_patterns);
            
            std::vector<std::pair<size_t, size_t>> ac_pairs;
            for (const auto& m : ac_matches) {
                ac_pairs.push_back({m.pattern_id, m.position});
            }
            
            std::cout << "  Aho-Corasick found: " << ac_matches.size() << " matches" << std::endl;
            std::cout << "  Naive search found: " << naive_matches.size() << " matches" << std::endl;
            
            Validator::validate(naive_matches, ac_pairs, test_patterns);
        }
        
        // ============ ЭТАП 3: Тестирование производительности ============
        std::cout << "\n============================================================" << std::endl;
        std::cout << "STAGE 3: Performance testing" << std::endl;
        std::cout << "============================================================" << std::endl;
        
        // Тесты с короткими паттернами
        benchmark.runTest("Short patterns (3-50 chars) in 1MB text", text_1mb, short_patterns_100);
        benchmark.runTest("Short patterns (3-50 chars) in 10MB text", text_10mb, short_patterns_100);
        
        // Тесты с длинными паттернами
        benchmark.runTest("Long patterns (1000+ chars) in 1MB text", text_1mb, long_patterns_10);
        benchmark.runTest("Long patterns (1000+ chars) in 10MB text", text_10mb, long_patterns_10);
        
        // Тесты с разными размерами словаря (короткие паттерны)
        std::vector<int> dict_sizes = {10, 50, 100, 200, 500};
        for (int size : dict_sizes) {
            std::vector<std::string> patterns = generator.generateShortPatterns(size);
            std::stringstream name;
            name << "Dictionary size: " << size << " short patterns";
            benchmark.runTest(name.str(), text_1mb, patterns);
        }
        
        // Тесты с разными размерами словаря (длинные паттерны)
        for (int size : {10, 50, 100}) {
            std::vector<std::string> patterns = generator.generateLongPatterns(size);
            std::stringstream name;
            name << "Dictionary size: " << size << " long patterns";
            benchmark.runTest(name.str(), text_1mb, patterns);
        }
        
        // ============ ЭТАП 4: Тест с большим текстом ============
        std::cout << "\n============================================================" << std::endl;
        std::cout << "STAGE 4: Large text test (100MB)" << std::endl;
        std::cout << "============================================================" << std::endl;
        
        std::cout << "\nLoading 100MB text file..." << std::endl;
        std::string large_text = generator.loadTextFromFile("text_100mb.txt");
        
        if (!large_text.empty()) {
            std::cout << "[OK] Loaded " << generator.formatFileSize(large_text.size()) << std::endl;
            
            benchmark.runTest("Short patterns in 100MB text", large_text, short_patterns_100);
            benchmark.runTest("Long patterns in 100MB text", large_text, long_patterns_10);
        } else {
            std::cout << "[WARNING] Large text file not found, skipping 100MB test" << std::endl;
        }
        
        // ============ ФИНАЛЬНЫЙ ОТЧЕТ ============
        benchmark.printFinalReport();
        benchmark.exportToCSV("aho_corasick_benchmark_results.csv");
        benchmark.generateVisualReport("aho_corasick_report.html");
        
        // ============ ДОПОЛНИТЕЛЬНАЯ АНАЛИТИКА ============
        std::cout << "\n============================================================" << std::endl;
        std::cout << "ADDITIONAL ANALYTICS" << std::endl;
        std::cout << "============================================================" << std::endl;
        
        {
            std::cout << "\nAutomaton size analysis:" << std::endl;
            
            AhoCorasick ac_short;
            for (const auto& p : short_patterns_100) ac_short.addPattern(p);
            ac_short.compile();
            
            std::cout << "  Short patterns (100): " << ac_short.getNodeCount() << " nodes" << std::endl;
            std::cout << "  Ratio: " << std::fixed << std::setprecision(2) 
                 << static_cast<double>(ac_short.getNodeCount()) / short_patterns_100.size() 
                 << " nodes per pattern" << std::endl;
            
            AhoCorasick ac_long;
            for (const auto& p : long_patterns_10) ac_long.addPattern(p);
            ac_long.compile();
            
            std::cout << "  Long patterns (10): " << ac_long.getNodeCount() << " nodes" << std::endl;
            std::cout << "  Ratio: " << std::fixed << std::setprecision(2) 
                 << static_cast<double>(ac_long.getNodeCount()) / long_patterns_10.size() 
                 << " nodes per pattern" << std::endl;
        }
        
        std::cout << "\nMemory estimation for 100M character text:" << std::endl;
        std::cout << "  Text: ~100 MB" << std::endl;
        std::cout << "  Automaton (1000 short patterns): ~10-50 MB" << std::endl;
        std::cout << "  Automaton (100 long patterns): ~50-200 MB" << std::endl;
        std::cout << "  Total estimate: ~150-350 MB" << std::endl;
        
        std::cout << "\n============================================================" << std::endl;
        std::cout << "RECOMMENDATIONS FOR SCALING" << std::endl;
        std::cout << "============================================================" << std::endl;
        std::cout << "\nFor production use with 100M+ character texts:" << std::endl;
        std::cout << "  1. Use mmap() for memory-mapped file I/O" << std::endl;
        std::cout << "  2. Process text in chunks of 10-50MB" << std::endl;
        std::cout << "  3. Parallelize with OpenMP or std::thread" << std::endl;
        std::cout << "  4. Compress automaton for large alphabets" << std::endl;
        std::cout << "  5. Consider double-array trie for memory efficiency" << std::endl;
        std::cout << "  6. Use cache-optimized transitions for hot paths" << std::endl;
        
        std::cout << "\n============================================================" << std::endl;
        std::cout << "  RESEARCH COMPLETED SUCCESSFULLY" << std::endl;
        std::cout << "============================================================" << std::endl;
        std::cout << "\n  Results saved to:" << std::endl;
        std::cout << "    - aho_corasick_benchmark_results.csv" << std::endl;
        std::cout << "    - aho_corasick_report.html (open in browser)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n[FATAL ERROR] " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
