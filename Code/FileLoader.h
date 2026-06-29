#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

std::string loadTextFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        std::cerr << "\nERROR: File '" << filename << "' not found!" << std::endl;
        std::cerr << "Create this file with your text (100M+ characters)." << std::endl;
        std::exit(1);
    }
    
    size_t size = static_cast<size_t>(file.tellg());
    file.seekg(0);
    std::string text(size, '\0');
    file.read(&text[0], static_cast<std::streamsize>(size));
    file.close();
    
    std::cout << "Loaded: " << filename << " (" << size << " chars, " 
              << std::fixed << std::setprecision(2) 
              << (size / (1024.0 * 1024.0)) << " MB)" << std::endl;
    
    return text;
}

#endif
