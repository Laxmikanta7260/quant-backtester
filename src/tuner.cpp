#include "tuner.h"

#include <fstream>
#include "strategy.h"
#include "backtester.h"
#include "metrics.h"

std::vector<TuningResult> Tuner::runMovingAverageGridSearch(
    const std::vector<Bar>& data,
    double initialCash,
    const std::vector<int>& shortWindows,
    const std::vector<int>& longWindows
) {
    std::vector<TuningResult> results;

    for (int shortW : shortWindows) {
        for (int longW : longWindows) {
            if (shortW >= longW) continue;

            MovingAverageCrossoverStrategy strategy(shortW, longW);
            std::vector<int> signals = strategy.generateSignals(data);

            Backtester backtester(initialCash, 0.001);
            backtester.run(data, signals);

            const auto& equity = backtester.getEquityCurve();

            TuningResult result;
            result.shortWindow = shortW;
            result.longWindow = longW;
            result.totalReturn = Metrics::computeTotalReturn(equity);
            result.maxDrawdown = Metrics::computeMaxDrawdown(equity);
            result.sharpeRatio = Metrics::computeSharpeRatio(equity);

            results.push_back(result);
        }
    }

    return results;
}

void Tuner::saveResultsToCSV(const std::string& filename, const std::vector<TuningResult>& results) {
    std::ofstream file(filename);
    file << "ShortWindow,LongWindow,TotalReturn,MaxDrawdown,SharpeRatio\n";

    for (const auto& r : results) {
        file << r.shortWindow << ","
             << r.longWindow << ","
             << r.totalReturn << ","
             << r.maxDrawdown << ","
             << r.sharpeRatio << "\n";
    }
}