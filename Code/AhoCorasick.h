#ifndef AHO_CORASICK_H
#define AHO_CORASICK_H

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>

class AhoCorasick {
private:
    struct Node {
        std::unordered_map<char, int> children;
        int fail = 0;
        std::vector<int> output;
        
        Node() = default;
    };
    
    std::vector<Node> automaton;
    std::vector<std::string> patterns;
    bool is_compiled = false;
    std::vector<std::unordered_map<char, int>> transition_cache;
    
    void initializeCache() {
        transition_cache.resize(automaton.size());
    }
    
    int getTransition(int state, char c) {
        auto cache_it = transition_cache[state].find(c);
        if (cache_it != transition_cache[state].end()) {
            return cache_it->second;
        }
        
        int original_state = state;
        
        while (state != 0 && automaton[state].children.find(c) == automaton[state].children.end()) {
            state = automaton[state].fail;
        }
        
        int result;
        auto it = automaton[state].children.find(c);
        if (it != automaton[state].children.end()) {
            result = it->second;
        } else {
            result = 0;
        }
        
        transition_cache[original_state][c] = result;
        return result;
    }
    
public:
    struct Match {
        int pattern_id;
        size_t position;
    };
    
    AhoCorasick() {
        automaton.emplace_back();
    }
    
    void addPattern(const std::string& pattern) {
        if (is_compiled) {
            throw std::runtime_error("Cannot add patterns after compilation");
        }
        
        int current = 0;
        for (char c : pattern) {
            auto it = automaton[current].children.find(c);
            if (it == automaton[current].children.end()) {
                automaton[current].children[c] = automaton.size();
                automaton.emplace_back();
                current = automaton.size() - 1;
            } else {
                current = it->second;
            }
        }
        automaton[current].output.push_back(patterns.size());
        patterns.push_back(pattern);
    }
    
    void compile() {
        if (is_compiled) return;
        
        std::queue<int> q;
        
        for (auto& pair : automaton[0].children) {
            int child = pair.second;
            automaton[child].fail = 0;
            q.push(child);
        }
        
        while (!q.empty()) {
            int current = q.front();
            q.pop();
            
            for (auto& pair : automaton[current].children) {
                char c = pair.first;
                int child = pair.second;
                int fail = automaton[current].fail;
                
                while (fail != 0 && automaton[fail].children.find(c) == automaton[fail].children.end()) {
                    fail = automaton[fail].fail;
                }
                
                auto it = automaton[fail].children.find(c);
                if (it != automaton[fail].children.end()) {
                    automaton[child].fail = it->second;
                } else {
                    automaton[child].fail = 0;
                }
                
                automaton[child].output.insert(
                    automaton[child].output.end(),
                    automaton[automaton[child].fail].output.begin(),
                    automaton[automaton[child].fail].output.end()
                );
                
                q.push(child);
            }
        }
        
        initializeCache();
        is_compiled = true;
    }
    
    std::vector<Match> search(const std::string& text) {
        if (!is_compiled) compile();
        
        std::vector<Match> matches;
        int state = 0;
        
        for (size_t i = 0; i < text.length(); i++) {
            state = getTransition(state, text[i]);
            
            if (!automaton[state].output.empty()) {
                for (int pattern_id : automaton[state].output) {
                    size_t pos = i + 1 - patterns[pattern_id].length();
                    matches.push_back({pattern_id, pos});
                }
            }
        }
        
        return matches;
    }
    
    std::vector<Match> searchWithoutCache(const std::string& text) {
        if (!is_compiled) compile();
        
        std::vector<Match> matches;
        int state = 0;
        
        for (size_t i = 0; i < text.length(); i++) {
            while (state != 0 && automaton[state].children.find(text[i]) == automaton[state].children.end()) {
                state = automaton[state].fail;
            }
            
            auto it = automaton[state].children.find(text[i]);
            if (it != automaton[state].children.end()) {
                state = it->second;
            } else {
                state = 0;
            }
            
            if (!automaton[state].output.empty()) {
                for (int pattern_id : automaton[state].output) {
                    size_t pos = i + 1 - patterns[pattern_id].length();
                    matches.push_back({pattern_id, pos});
                }
            }
        }
        
        return matches;
    }
    
    size_t getNodeCount() const { return automaton.size(); }
    size_t getPatternCount() const { return patterns.size(); }
    const std::vector<std::string>& getPatterns() const { return patterns; }
};

#endif // AHO_CORASICK_H
