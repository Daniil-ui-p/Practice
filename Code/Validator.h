#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>

class Validator {
public:
    static bool validate(
        const std::vector<std::pair<size_t, size_t>>& expected,
        const std::vector<std::pair<size_t, size_t>>& actual,
        const std::vector<std::string>& patterns) {
        
        if (expected.size() != actual.size()) {
            std::cout << "    [FAIL] Size mismatch: expected " << expected.size() 
                 << " matches, got " << actual.size() << std::endl;
            return false;
        }
        
        auto sorted_expected = expected;
        auto sorted_actual = actual;
        
        std::sort(sorted_expected.begin(), sorted_expected.end());
        std::sort(sorted_actual.begin(), sorted_actual.end());
        
        bool match = sorted_expected == sorted_actual;
        
        if (match) {
            std::cout << "    [OK] Validation PASSED" << std::endl;
        } else {
            std::cout << "    [FAIL] Validation FAILED" << std::endl;
            std::cout << "    First 10 expected matches:" << std::endl;
            for (size_t i = 0; i < std::min(expected.size(), static_cast<size_t>(10)); i++) {
                std::cout << "      Pattern '" << patterns[expected[i].first] 
                     << "' at pos " << expected[i].second << std::endl;
            }
            std::cout << "    First 10 actual matches:" << std::endl;
            for (size_t i = 0; i < std::min(actual.size(), static_cast<size_t>(10)); i++) {
                std::cout << "      Pattern '" << patterns[actual[i].first] 
                     << "' at pos " << actual[i].second << std::endl;
            }
        }
        
        return match;
    }
};

#endif
