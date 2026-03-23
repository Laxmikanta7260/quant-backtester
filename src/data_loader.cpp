#include "data_loader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::vector<Bar> DataLoader::loadCSV(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return {};
    }

    std::vector<Bar> bars;
    std::string line;

    // Skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;
        Bar bar;

        std::getline(ss, bar.date, ',');

        std::getline(ss, token, ',');
        bar.open = std::stod(token);

        std::getline(ss, token, ',');
        bar.high = std::stod(token);

        std::getline(ss, token, ',');
        bar.low = std::stod(token);

        std::getline(ss, token, ',');
        bar.close = std::stod(token);

        std::getline(ss, token, ',');
        bar.volume = std::stod(token);

        bars.push_back(bar);
    }

    return bars;
}