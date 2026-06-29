#ifndef AHO_CORASICK_H
#define AHO_CORASICK_H

#include <vector>
#include <string>
#include <queue>
#include <unordered_map>

class AhoCorasick {
public:
    struct Match { 
        int id; 
        size_t pos; 
    };

private:
    struct Node {
        std::unordered_map<char, int> next;
        int fail;
        std::vector<int> out;
        Node() : fail(0) {}
    };
    
    std::vector<Node> t;
    std::vector<std::string> pat;
    std::vector<std::unordered_map<char, int>> cache;
    bool built;
    
    void initCache();
    int go(int s, char c);
    
public:
    AhoCorasick();
    void add(const std::string& p);
    void build();
    std::vector<Match> search(const std::string& text);
    std::vector<Match> searchNoCache(const std::string& text);
    size_t nodes() const;
};

AhoCorasick::AhoCorasick() : built(false) {
    t.emplace_back();
}

void AhoCorasick::add(const std::string& p) {
    int v = 0;
    for (char c : p) {
        if (!t[v].next.count(c)) {
            t[v].next[c] = static_cast<int>(t.size());
            t.emplace_back();
        }
        v = t[v].next[c];
    }
    t[v].out.push_back(static_cast<int>(pat.size()));
    pat.push_back(p);
}

void AhoCorasick::initCache() {
    cache.resize(t.size());
}

void AhoCorasick::build() {
    if (built) return;
    
    std::queue<int> q;
    
    for (auto& pair : t[0].next) {
        int v = pair.second;
        t[v].fail = 0;
        q.push(v);
    }
    
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        
        for (auto& pair : t[v].next) {
            char c = pair.first;
            int u = pair.second;
            int f = t[v].fail;
            
            while (f != 0 && t[f].next.find(c) == t[f].next.end()) {
                f = t[f].fail;
            }
            
            t[u].fail = t[f].next.count(c) ? t[f].next[c] : 0;
            t[u].out.insert(t[u].out.end(), t[t[u].fail].out.begin(), t[t[u].fail].out.end());
            q.push(u);
        }
    }
    
    initCache();
    built = true;
}

int AhoCorasick::go(int s, char c) {
    auto it = cache[s].find(c);
    if (it != cache[s].end()) return it->second;
    
    int orig = s;
    while (s != 0 && t[s].next.find(c) == t[s].next.end()) {
        s = t[s].fail;
    }
    
    int res = t[s].next.count(c) ? t[s].next[c] : 0;
    cache[orig][c] = res;
    return res;
}

std::vector<AhoCorasick::Match> AhoCorasick::search(const std::string& text) {
    if (!built) build();
    
    std::vector<Match> m;
    int s = 0;
    
    for (size_t i = 0; i < text.size(); i++) {
        s = go(s, text[i]);
        for (int id : t[s].out) {
            m.push_back({id, i + 1 - pat[id].size()});
        }
    }
    
    return m;
}

std::vector<AhoCorasick::Match> AhoCorasick::searchNoCache(const std::string& text) {
    if (!built) build();
    
    std::vector<Match> m;
    int s = 0;
    
    for (size_t i = 0; i < text.size(); i++) {
        while (s != 0 && t[s].next.find(text[i]) == t[s].next.end()) {
            s = t[s].fail;
        }
        
        s = t[s].next.count(text[i]) ? t[s].next[text[i]] : 0;
        
        for (int id : t[s].out) {
            m.push_back({id, i + 1 - pat[id].size()});
        }
    }
    
    return m;
}

size_t AhoCorasick::nodes() const {
    return t.size();
}

#endif
