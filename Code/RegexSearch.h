#ifndef REGEX_SEARCH_H
#define REGEX_SEARCH_H

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <cstring>
#include <utility>

class RegexSearch {
public:
    static std::vector<std::pair<size_t, size_t>> search(
        const std::string& text, 
        const std::vector<std::string>& patterns) {
        
        std::vector<std::pair<size_t, size_t>> matches;
        
        for (size_t p = 0; p < patterns.size(); p++) {
            try {
                std::string escaped;
                for (char c : patterns[p]) {
                    if (std::strchr(".^*+?$()[]{}|\\", c)) {
                        escaped += '\\';
                    }
                    escaped += c;
                }
                
                std::regex pattern_regex(escaped);
                auto begin = std::sregex_iterator(text.begin(), text.end(), pattern_regex);
                auto end = std::sregex_iterator();
                
                for (std::sregex_iterator i = begin; i != end; ++i) {
                    matches.push_back({p, static_cast<size_t>((*i).position())});
                }
            } catch (const std::regex_error& e) {
                std::cerr << "Regex error for pattern " << p << ": " << e.what() << std::endl;
            }
        }
        
        return matches;
    }
};

#endif // REGEX_SEARCH_H
