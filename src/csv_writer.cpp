#include "../include/csv_writer.h"
#include <fstream>
#include <iostream>

void CSVWriter::writeTuningResults(
    const std::string& filename,
    const std::vector<TuningResult>& results
) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: could not open file " << filename << "\n";
        return;
    }

    file << "shortWindow,longWindow,totalReturn,maxDrawdown,sharpeRatio\n";

    for (const auto& r : results) {
        file << r.shortWindow << ","
             << r.longWindow << ","
             << r.totalReturn << ","
             << r.maxDrawdown << ","
             << r.sharpeRatio << "\n";
    }

    file.close();
    std::cout << "Tuning results saved to " << filename << "\n";
}