#ifndef PATTERN_LOADER_H
#define PATTERN_LOADER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

std::vector<std::string> loadPatternsFromFile(const std::string& filename) {
    std::vector<std::string> patterns;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Warning: Pattern file '" << filename << "' not found." << std::endl;
        return patterns;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        while (!line.empty() && std::isspace(line.front())) line.erase(0, 1);
        while (!line.empty() && std::isspace(line.back())) line.pop_back();
        if (!line.empty()) patterns.push_back(line);
    }
    
    file.close();
    std::cout << "Loaded " << patterns.size() << " patterns from " << filename << std::endl;
    return patterns;
}

#endif
