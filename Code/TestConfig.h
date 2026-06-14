#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#include <cstddef>

struct TestConfig {
    static constexpr size_t TEXT_SMALL = 1'000'000;
    static constexpr size_t TEXT_MEDIUM = 10'000'000;
    static constexpr size_t TEXT_LARGE = 100'000'000;
    
    static constexpr int PATTERNS_SMALL = 10;
    static constexpr int PATTERNS_MEDIUM = 100;
    static constexpr int PATTERNS_LARGE = 1000;
    
    static constexpr int SHORT_MIN = 3;
    static constexpr int SHORT_MAX = 50;
    static constexpr int LONG_MIN = 1000;
    static constexpr int LONG_MAX = 5000;
};

#endif
