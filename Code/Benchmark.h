#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "ResourceMonitor.h"
#include "AhoCorasick.h"
#include "NaiveSearch.h"
#include "RegexSearch.h"

struct BenchmarkResult {
    std::string algorithm_name;
    double time_ms;
    size_t memory_bytes;
    double cpu_time_ms;
    size_t matches_found;
    size_t automaton_nodes = 0;
};

class Benchmark {
private:
    struct TestGroup {
        std::string test_name;
        std::vector<BenchmarkResult> results;
    };
    
    std::vector<TestGroup> all_results;
    
    std::string formatSize(size_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB"};
        int unit = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024 && unit < 3) {
            size /= 1024;
            unit++;
        }
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << size << " " << units[unit];
        return ss.str();
    }
    
    BenchmarkResult runAhoCorasick(const std::string& text, 
                                   const std::vector<std::string>& patterns, 
                                   bool use_cache) {
        ResourceMonitor monitor;
        monitor.startMonitoring();
        
        AhoCorasick ac;
        auto start_build = std::chrono::high_resolution_clock::now();
        
        for (const auto& pattern : patterns) {
            ac.addPattern(pattern);
        }
        ac.compile();
        
        auto build_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start_build);
        
        auto start_search = std::chrono::high_resolution_clock::now();
        double start_cpu = monitor.getCPULoad();
        
        std::vector<AhoCorasick::Match> matches;
        if (use_cache) {
            matches = ac.search(text);
        } else {
            matches = ac.searchWithoutCache(text);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double end_cpu = monitor.getCPULoad();
        
        auto search_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_search);
        
        BenchmarkResult result;
        result.algorithm_name = use_cache ? "Aho-Corasick (cached)" : "Aho-Corasick (no cache)";
        result.time_ms = (build_time.count() + search_time.count()) / 1000.0;
        result.memory_bytes = monitor.getMemoryUsage();
        result.cpu_time_ms = end_cpu - start_cpu;
        result.matches_found = matches.size();
        result.automaton_nodes = ac.getNodeCount();
        
        return result;
    }
    
    BenchmarkResult runNaiveSearch(const std::string& text, 
                                   const std::vector<std::string>& patterns) {
        ResourceMonitor monitor;
        monitor.startMonitoring();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        double start_cpu = monitor.getCPULoad();
        
        auto matches = NaiveSearch::search(text, patterns);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double end_cpu = monitor.getCPULoad();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        BenchmarkResult result;
        result.algorithm_name = "Naive Search";
        result.time_ms = duration.count() / 1000.0;
        result.memory_bytes = monitor.getMemoryUsage();
        result.cpu_time_ms = end_cpu - start_cpu;
        result.matches_found = matches.size();
        
        return result;
    }
    
    BenchmarkResult runRegexSearch(const std::string& text, 
                                   const std::vector<std::string>& patterns) {
        ResourceMonitor monitor;
        monitor.startMonitoring();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        double start_cpu = monitor.getCPULoad();
        
        auto matches = RegexSearch::search(text, patterns);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double end_cpu = monitor.getCPULoad();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        BenchmarkResult result;
        result.algorithm_name = "Regex (std::regex)";
        result.time_ms = duration.count() / 1000.0;
        result.memory_bytes = monitor.getMemoryUsage();
        result.cpu_time_ms = end_cpu - start_cpu;
        result.matches_found = matches.size();
        
        return result;
    }
    
    void printGroupResults(const TestGroup& group) {
        std::cout << "\n  Results:" << std::endl;
        std::cout << "  " << std::string(70, '-') << std::endl;
        std::cout << "  " << std::left << std::setw(25) << "Algorithm" 
             << std::setw(12) << "Time(ms)" 
             << std::setw(12) << "CPU(ms)"
             << std::setw(15) << "Memory"
             << std::setw(10) << "Matches" << std::endl;
        std::cout << "  " << std::string(70, '-') << std::endl;
        
        for (const auto& res : group.results) {
            std::cout << "  " << std::left << std::setw(25) << res.algorithm_name
                 << std::setw(12) << std::fixed << std::setprecision(2) << res.time_ms
                 << std::setw(12) << std::fixed << std::setprecision(2) << res.cpu_time_ms
                 << std::setw(15) << formatSize(res.memory_bytes)
                 << std::setw(10) << res.matches_found;
            
            if (res.automaton_nodes > 0) {
                std::cout << " (nodes: " << res.automaton_nodes << ")";
            }
            std::cout << std::endl;
        }
    }
    
public:
    void runTest(const std::string& test_name, 
                const std::string& text, 
                const std::vector<std::string>& patterns) {
        TestGroup group;
        group.test_name = test_name;
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "Test: " << test_name << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        std::cout << "  Text size: " << formatSize(text.size()) << std::endl;
        std::cout << "  Patterns: " << patterns.size() << std::endl;
        
        if (!patterns.empty()) {
            size_t min_len = patterns[0].length();
            size_t max_len = patterns[0].length();
            for (const auto& p : patterns) {
                min_len = std::min(min_len, p.length());
                max_len = std::max(max_len, p.length());
            }
            std::cout << "  Pattern lengths: " << min_len << " - " << max_len << " chars" << std::endl;
        }
        
        std::cout << "\n  Running Aho-Corasick with cache..." << std::flush;
        group.results.push_back(runAhoCorasick(text, patterns, true));
        std::cout << " Done" << std::endl;
        
        std::cout << "  Running Aho-Corasick without cache..." << std::flush;
        group.results.push_back(runAhoCorasick(text, patterns, false));
        std::cout << " Done" << std::endl;
        
        std::cout << "  Running Naive search..." << std::flush;
        group.results.push_back(runNaiveSearch(text, patterns));
        std::cout << " Done" << std::endl;
        
        if (patterns.size() <= 20 && patterns[0].length() <= 100) {
            std::cout << "  Running Regex search..." << std::flush;
            group.results.push_back(runRegexSearch(text, patterns));
            std::cout << " Done" << std::endl;
        }
        
        all_results.push_back(group);
        printGroupResults(group);
    }
    
    void printFinalReport() {
        std::cout << "\n\n" << std::string(80, '=') << std::endl;
        std::cout << "FINAL COMPARATIVE REPORT" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        for (const auto& group : all_results) {
            std::cout << "\n" << group.test_name << std::endl;
            std::cout << std::string(70, '-') << std::endl;
            std::cout << std::left << std::setw(25) << "Algorithm" 
                 << std::setw(12) << "Time(ms)" 
                 << std::setw(12) << "CPU(ms)"
                 << std::setw(15) << "Memory"
                 << std::setw(10) << "Matches" << std::endl;
            std::cout << std::string(70, '-') << std::endl;
            
            for (const auto& res : group.results) {
                std::cout << std::left << std::setw(25) << res.algorithm_name
                     << std::setw(12) << std::fixed << std::setprecision(2) << res.time_ms
                     << std::setw(12) << std::fixed << std::setprecision(2) << res.cpu_time_ms
                     << std::setw(15) << formatSize(res.memory_bytes)
                     << std::setw(10) << res.matches_found << std::endl;
            }
        }
    }
    
    void exportToCSV(const std::string& filename) {
        std::ofstream file(filename);
        file << "Test Group,Algorithm,Time(ms),CPU(ms),Memory(bytes),Matches,Automaton Nodes\n";
        
        for (const auto& group : all_results) {
            for (const auto& res : group.results) {
                file << "\"" << group.test_name << "\","
                     << "\"" << res.algorithm_name << "\","
                     << res.time_ms << ","
                     << res.cpu_time_ms << ","
                     << res.memory_bytes << ","
                     << res.matches_found << ","
                     << res.automaton_nodes << "\n";
            }
        }
        
        file.close();
        std::cout << "\n[OK] Results exported to: " << filename << std::endl;
    }
    
    void generateVisualReport(const std::string& filename) {
        std::ofstream html(filename);
        
        html << "<!DOCTYPE html><html><head><meta charset='UTF-8'>\n";
        html << "<title>Aho-Corasick Benchmark Report</title>\n";
        html << "<style>\n";
        html << "  body { font-family: Arial; margin: 20px; background: #f5f5f5; }\n";
        html << "  h1 { color: #333; text-align: center; }\n";
        html << "  h2 { color: #666; margin-top: 30px; }\n";
        html << "  table { border-collapse: collapse; width: 100%; background: white; margin: 10px 0; }\n";
        html << "  th, td { border: 1px solid #ddd; padding: 12px; text-align: left; }\n";
        html << "  th { background-color: #4CAF50; color: white; }\n";
        html << "  tr:nth-child(even) { background-color: #f2f2f2; }\n";
        html << "  .chart { background: white; margin: 20px 0; padding: 20px; border-radius: 5px; }\n";
        html << "  .bar { height: 30px; margin: 5px 0; padding: 5px; color: white; font-size: 14px; ";
        html << "border-radius: 3px; transition: width 0.3s; }\n";
        html << "</style></head><body>\n";
        
        html << "<h1>📊 Aho-Corasick Algorithm Benchmark Report</h1>\n";
        html << "<p style='text-align:center'>Generated: " << __DATE__ << " " << __TIME__ << "</p>\n";
        
        for (const auto& group : all_results) {
            html << "<h2>" << group.test_name << "</h2>\n";
            
            html << "<table><tr><th>Algorithm</th><th>Time (ms)</th>";
            html << "<th>CPU (ms)</th><th>Memory</th><th>Matches</th></tr>\n";
            
            double max_time = 0;
            for (const auto& r : group.results) {
                max_time = std::max(max_time, r.time_ms);
            }
            
            for (const auto& r : group.results) {
                html << "<tr>";
                html << "<td>" << r.algorithm_name << "</td>";
                html << "<td>" << std::fixed << std::setprecision(2) << r.time_ms << "</td>";
                html << "<td>" << std::fixed << std::setprecision(2) << r.cpu_time_ms << "</td>";
                html << "<td>" << formatSize(r.memory_bytes) << "</td>";
                html << "<td>" << r.matches_found << "</td>";
                html << "</tr>\n";
            }
            html << "</table>\n";
            
            html << "<div class='chart'><h3>Performance Comparison</h3>\n";
            for (const auto& r : group.results) {
                int width = std::max(50, static_cast<int>((r.time_ms / max_time) * 600));
                std::string color;
                if (r.algorithm_name.find("cached") != std::string::npos) color = "#4CAF50";
                else if (r.algorithm_name.find("no cache") != std::string::npos) color = "#2196F3";
                else if (r.algorithm_name.find("Naive") != std::string::npos) color = "#FF9800";
                else color = "#F44336";
                
                html << "<div class='bar' style='width:" << width << "px;background:" << color << "'>";
                html << r.algorithm_name << ": " << std::fixed << std::setprecision(1) << r.time_ms << "ms";
                html << "</div>\n";
            }
            html << "</div>\n";
        }
        
        html << "</body></html>\n";
        html.close();
        
        std::cout << "[OK] Visual report saved to: " << filename << std::endl;
    }
};

#endif // BENCHMARK_H
