#include "backtester.h"

#include <fstream>

Backtester::Backtester(double initialCash, double transactionCostRate)
    : initialCash(initialCash),
      cash(initialCash),
      position(0),
      transactionCostRate(transactionCostRate) {}

void Backtester::run(const std::vector<Bar>& data, const std::vector<int>& signals) {
    equityCurve.clear();
    trades.clear();
    cash = initialCash;
    position = 0;

    for (size_t i = 0; i < data.size(); ++i) {
        double price = data[i].close;

        if (signals[i] == 1 && position == 0) {
            int sharesToBuy = static_cast<int>(cash / (price * (1.0 + transactionCostRate)));
            if (sharesToBuy > 0) {
                double grossCost = sharesToBuy * price;
                double fee = grossCost * transactionCostRate;
                double totalCost = grossCost + fee;

                cash -= totalCost;
                position = sharesToBuy;

                trades.push_back({data[i].date, "BUY", price, sharesToBuy, cash});
            }
        }
        else if (signals[i] == -1 && position > 0) {
            double grossProceeds = position * price;
            double fee = grossProceeds * transactionCostRate;
            double netProceeds = grossProceeds - fee;

            cash += netProceeds;
            trades.push_back({data[i].date, "SELL", price, position, cash});
            position = 0;
        }

        double equity = cash + position * price;
        equityCurve.push_back(equity);
    }
}

double Backtester::getCash() const {
    return cash;
}

int Backtester::getPosition() const {
    return position;
}

double Backtester::getTransactionCostRate() const {
    return transactionCostRate;
}

const std::vector<double>& Backtester::getEquityCurve() const {
    return equityCurve;
}

const std::vector<Trade>& Backtester::getTrades() const {
    return trades;
}

void Backtester::saveTradesToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    file << "Date,Action,Price,Shares,CashAfter\n";

    for (const auto& t : trades) {
        file << t.date << ","
             << t.action << ","
             << t.price << ","
             << t.shares << ","
             << t.cashAfter << "\n";
    }
}

void Backtester::saveEquityCurveToCSV(const std::string& filename, const std::vector<Bar>& data) const {
    std::ofstream file(filename);
    file << "Date,Equity\n";

    for (size_t i = 0; i < equityCurve.size() && i < data.size(); ++i) {
        file << data[i].date << "," << equityCurve[i] << "\n";
    }
}