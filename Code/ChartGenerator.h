#ifndef CHART_GENERATOR_H
#define CHART_GENERATOR_H

#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>

void printBar(const std::string& name, double value, double max, const std::string& unit) {
    int len = static_cast<int>((value / max) * 50);
    std::cout << std::left << std::setw(25) << name << " |";
    for (int i = 0; i < len; i++) std::cout << '#';
    std::cout << " " << std::fixed << std::setprecision(1) << value << " " << unit << std::endl;
}

void printTimeChart(double cache, double nocache, double naive) {
    std::cout << "\nTime comparison (ms):\n" << std::endl;
    double max_time = std::max({cache, nocache, naive});
    
    printBar("Aho-Corasick (cached)", cache, max_time, "ms");
    printBar("Aho-Corasick (no cache)", nocache, max_time, "ms");
    printBar("Naive Search", naive, max_time, "ms");
}

void printMemoryChart(double mem_short, double mem_long) {
    std::cout << "\nMemory comparison (MB):\n" << std::endl;
    double max_mem = std::max(mem_short, mem_long);
    
    printBar("Short patterns", mem_short, max_mem, "MB");
    printBar("Long patterns", mem_long, max_mem, "MB");
}

#endif
