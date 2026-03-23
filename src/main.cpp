#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>

#include "data_loader.h"
#include "strategy.h"
#include "backtester.h"
#include "metrics.h"
#include "tuner.h"

int main() {
    std::string filename = "../data/sample_prices.csv";
    std::vector<Bar> data = DataLoader::loadCSV(filename);

    if (data.empty()) {
        std::cout << "No data loaded.\n";
        return 1;
    }

    std::cout << "Loaded " << data.size() << " rows\n";

    std::string strategyType = std::getenv("STRATEGY_TYPE") ? std::getenv("STRATEGY_TYPE") : "MA";
    int shortWindow = std::getenv("SHORT_WINDOW") ? std::stoi(std::getenv("SHORT_WINDOW")) : 20;
    int longWindow = std::getenv("LONG_WINDOW") ? std::stoi(std::getenv("LONG_WINDOW")) : 50;
    int rsiPeriod = std::getenv("RSI_PERIOD") ? std::stoi(std::getenv("RSI_PERIOD")) : 14;
    double overbought = std::getenv("RSI_OVERBOUGHT") ? std::stod(std::getenv("RSI_OVERBOUGHT")) : 70.0;
    double oversold = std::getenv("RSI_OVERSOLD") ? std::stod(std::getenv("RSI_OVERSOLD")) : 30.0;
    double transactionCostRate = std::getenv("TRANSACTION_COST") ? std::stod(std::getenv("TRANSACTION_COST")) : 0.001;

    std::vector<int> signals;
    std::string strategyName;

    if (strategyType == "RSI") {
        RSIStrategy strategy(rsiPeriod, overbought, oversold);
        signals = strategy.generateSignals(data);
        strategyName = strategy.getName();
    } else {
        MovingAverageCrossoverStrategy strategy(shortWindow, longWindow);
        signals = strategy.generateSignals(data);
        strategyName = strategy.getName();
    }

    Backtester backtester(10000.0, transactionCostRate);
    backtester.run(data, signals);

    std::cout << "Strategy: " << strategyName << "\n";
    std::cout << "Transaction Cost Rate: " << transactionCostRate << "\n";
    std::cout << "Final Cash: " << backtester.getCash() << "\n";
    std::cout << "Final Position: " << backtester.getPosition() << "\n";
    std::cout << "Number of Trades: " << backtester.getTrades().size() << "\n";

    auto equity = backtester.getEquityCurve();

    std::cout << "\nPerformance:\n";
    std::cout << "Total Return: " << Metrics::computeTotalReturn(equity) << "\n";
    std::cout << "Max Drawdown: " << Metrics::computeMaxDrawdown(equity) << "\n";
    std::cout << "Sharpe Ratio: " << Metrics::computeSharpeRatio(equity) << "\n";
    std::cout << "Buy & Hold Return: " << Metrics::computeBuyAndHoldReturn(data) << "\n";

    backtester.saveTradesToCSV("../output/trades.csv");
    backtester.saveEquityCurveToCSV("../output/equity_curve.csv", data);

    std::vector<int> shortWindows = {5, 10, 15, 20};
    std::vector<int> longWindows = {30, 50, 100};

    auto tuningResults = Tuner::runMovingAverageGridSearch(data, 10000.0, shortWindows, longWindows);
    Tuner::saveResultsToCSV("../output/tuning_results.csv", tuningResults);

    return 0;
}