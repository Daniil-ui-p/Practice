#ifndef NAIVE_SEARCH_H
#define NAIVE_SEARCH_H

#include <vector>
#include <string>
#include <utility>

class NaiveSearch {
public:
    static std::vector<std::pair<size_t, size_t>> search(
        const std::string& text, 
        const std::vector<std::string>& patterns) {
        
        std::vector<std::pair<size_t, size_t>> matches;
        
        for (size_t p = 0; p < patterns.size(); p++) {
            const std::string& pattern = patterns[p];
            if (pattern.empty()) continue;
            
            size_t pos = 0;
            while ((pos = text.find(pattern, pos)) != std::string::npos) {
                matches.push_back({p, pos});
                pos += 1;
            }
        }
        
        return matches;
    }
};

#endif
