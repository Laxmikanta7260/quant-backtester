#ifndef BACKTESTER_H
#define BACKTESTER_H

#include <string>
#include <vector>
#include "data_loader.h"

struct Trade {
    std::string date;
    std::string action;
    double price;
    int shares;
    double cashAfter;
};

class Backtester {
private:
    double initialCash;
    double cash;
    int position;
    double transactionCostRate;
    std::vector<double> equityCurve;
    std::vector<Trade> trades;

public:
    Backtester(double initialCash, double transactionCostRate = 0.0);

    void run(const std::vector<Bar>& data, const std::vector<int>& signals);

    double getCash() const;
    int getPosition() const;
    double getTransactionCostRate() const;
    const std::vector<double>& getEquityCurve() const;
    const std::vector<Trade>& getTrades() const;

    void saveTradesToCSV(const std::string& filename) const;
    void saveEquityCurveToCSV(const std::string& filename, const std::vector<Bar>& data) const;
};

#endif