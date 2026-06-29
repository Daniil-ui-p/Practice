#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <iostream>
#include <iomanip>
#include <chrono>
#include "AhoCorasick.h"
#include "NaiveSearch.h"
#include "ResourceMonitor.h"

struct TestResult {
    double time_ms;
    double cpu_ms;
    size_t memory_bytes;
    size_t matches;
};

TestResult runAhoCorasickCached(AhoCorasick& ac, const std::string& text) {
    ResourceMonitor mon;
    mon.start();
    
    auto t1 = std::chrono::high_resolution_clock::now();
    double cpu1 = mon.cpuTime();
    
    auto matches = ac.search(text);
    
    auto t2 = std::chrono::high_resolution_clock::now();
    double cpu2 = mon.cpuTime();
    
    TestResult r;
    r.time_ms = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1000.0;
    r.cpu_ms = cpu2 - cpu1;
    r.memory_bytes = mon.memoryUsed();
    r.matches = matches.size();
    
    return r;
}

TestResult runAhoCorasickNoCache(AhoCorasick& ac, const std::string& text) {
    ResourceMonitor mon;
    mon.start();
    
    auto t1 = std::chrono::high_resolution_clock::now();
    double cpu1 = mon.cpuTime();
    
    auto matches = ac.searchNoCache(text);
    
    auto t2 = std::chrono::high_resolution_clock::now();
    double cpu2 = mon.cpuTime();
    
    TestResult r;
    r.time_ms = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1000.0;
    r.cpu_ms = cpu2 - cpu1;
    r.memory_bytes = mon.memoryUsed();
    r.matches = matches.size();
    
    return r;
}

TestResult runNaiveSearch(const std::string& text, const std::vector<std::string>& patterns) {
    auto t1 = std::chrono::high_resolution_clock::now();
    
    auto matches = NaiveSearch::search(text, patterns);
    
    auto t2 = std::chrono::high_resolution_clock::now();
    
    TestResult r;
    r.time_ms = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1000.0;
    r.cpu_ms = 0;
    r.memory_bytes = 0;
    r.matches = matches.size();
    
    return r;
}

#endif
