#ifndef NAIVE_SEARCH_H
#define NAIVE_SEARCH_H

#include <vector>
#include <string>
#include <utility>

class NaiveSearch {
public:
    static std::vector<std::pair<size_t, size_t>> search(
        const std::string& text, 
        const std::vector<std::string>& patterns);
};

std::vector<std::pair<size_t, size_t>> NaiveSearch::search(
    const std::string& text, 
    const std::vector<std::string>& patterns) {
    
    std::vector<std::pair<size_t, size_t>> m;
    
    for (size_t i = 0; i < patterns.size(); i++) {
        size_t p = 0;
        while ((p = text.find(patterns[i], p)) != std::string::npos) {
            m.push_back({i, p});
            p++;
        }
    }
    
    return m;
}

#endif
