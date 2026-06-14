#ifndef DATA_GENERATOR_H
#define DATA_GENERATOR_H

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "TestConfig.h"

class DataGenerator {
private:
    std::mt19937 gen;
    const std::string ALPHABET = "abcdefghijklmnopqrstuvwxyz";
    
public:
    DataGenerator() : gen(std::random_device{}()) {}
    
    std::string generateText(size_t length) {
        std::string text;
        text.reserve(length);
        
        std::uniform_int_distribution<> dis(0, ALPHABET.size() - 1);
        
        for (size_t i = 0; i < length; i++) {
            text += ALPHABET[dis(gen)];
        }
        
        return text;
    }
    
    std::vector<std::string> generateShortPatterns(int count) {
        std::vector<std::string> patterns;
        patterns.reserve(count);
        
        std::uniform_int_distribution<> len_dis(TestConfig::SHORT_MIN, TestConfig::SHORT_MAX);
        std::uniform_int_distribution<> char_dis(0, ALPHABET.size() - 1);
        
        for (int i = 0; i < count; i++) {
            int len = len_dis(gen);
            std::string pattern;
            pattern.reserve(len);
            for (int j = 0; j < len; j++) {
                pattern += ALPHABET[char_dis(gen)];
            }
            patterns.push_back(pattern);
        }
        
        return patterns;
    }
    
    std::vector<std::string> generateLongPatterns(int count) {
        std::vector<std::string> patterns;
        patterns.reserve(count);
        
        std::uniform_int_distribution<> len_dis(TestConfig::LONG_MIN, TestConfig::LONG_MAX);
        std::uniform_int_distribution<> char_dis(0, ALPHABET.size() - 1);
        
        for (int i = 0; i < count; i++) {
            int len = len_dis(gen);
            std::string pattern;
            pattern.reserve(len);
            for (int j = 0; j < len; j++) {
                pattern += ALPHABET[char_dis(gen)];
            }
            patterns.push_back(pattern);
        }
        
        return patterns;
    }
    
    void saveTextToFile(const std::string& filename, const std::string& text) {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.write(text.c_str(), text.size());
            file.close();
            std::cout << "  [OK] Saved text file: " << filename 
                 << " (" << formatFileSize(text.size()) << ")" << std::endl;
        } else {
            std::cerr << "  [ERROR] Error saving text file: " << filename << std::endl;
        }
    }
    
    void savePatternsToFile(const std::string& filename, const std::vector<std::string>& patterns) {
        std::ofstream file(filename);
        if (file.is_open()) {
            for (const auto& pattern : patterns) {
                file << pattern << "\n";
            }
            file.close();
            std::cout << "  [OK] Saved patterns file: " << filename 
                 << " (" << patterns.size() << " patterns)" << std::endl;
        } else {
            std::cerr << "  [ERROR] Error saving patterns file: " << filename << std::endl;
        }
    }
    
    void saveLargeTextToFile(const std::string& filename, size_t totalSize) {
        const size_t CHUNK_SIZE = 10 * 1024 * 1024; // 10 MB chunks
        std::ofstream file(filename, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "  [ERROR] Error creating large text file: " << filename << std::endl;
            return;
        }
        
        size_t written = 0;
        while (written < totalSize) {
            size_t chunkSize = std::min(CHUNK_SIZE, totalSize - written);
            std::string chunk = generateText(chunkSize);
            file.write(chunk.c_str(), chunk.size());
            written += chunkSize;
            
            if (written % (50 * 1024 * 1024) == 0 || written == totalSize) {
                std::cout << "\r  Generating large text: " << (written * 100 / totalSize) 
                     << "% complete..." << std::flush;
            }
        }
        
        file.close();
        std::cout << "\r  [OK] Saved large text file: " << filename 
             << " (" << formatFileSize(totalSize) << ")           " << std::endl;
    }
    
    std::string loadTextFromFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            size_t size = file.tellg();
            file.seekg(0, std::ios::beg);
            
            std::string content;
            content.resize(size);
            file.read(&content[0], size);
            file.close();
            return content;
        }
        return "";
    }
    
    std::vector<std::string> loadPatternsFromFile(const std::string& filename) {
        std::vector<std::string> patterns;
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                if (!line.empty()) {
                    patterns.push_back(line);
                }
            }
            file.close();
        }
        return patterns;
    }
    
    std::string formatFileSize(size_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB"};
        int unit = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024 && unit < 3) {
            size /= 1024;
            unit++;
        }
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << size << " " << units[unit];
        return ss.str();
    }
};

#endif // DATA_GENERATOR_H
