#ifndef TUNER_H
#define TUNER_H

#include <vector>
#include "data_loader.h"

struct TuningResult {
    int shortWindow;
    int longWindow;
    double totalReturn;
    double maxDrawdown;
    double sharpeRatio;
};

class Tuner {
public:
    static std::vector<TuningResult> runMovingAverageGridSearch(
        const std::vector<Bar>& data,
        double initialCash,
        const std::vector<int>& shortWindows,
        const std::vector<int>& longWindows
    );

    static void saveResultsToCSV(const std::string& filename, const std::vector<TuningResult>& results);
};

#endif