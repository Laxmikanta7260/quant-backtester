#include "strategy.h"

#include <vector>
#include <string>
#include <numeric>
#include <cstddef>

MovingAverageCrossoverStrategy::MovingAverageCrossoverStrategy(int shortW, int longW)
    : shortWindow(shortW), longWindow(longW) {}

std::string MovingAverageCrossoverStrategy::getName() const {
    return "MovingAverageCrossover";
}

std::vector<int> MovingAverageCrossoverStrategy::generateSignals(const std::vector<Bar>& data) {
    std::vector<int> signals(data.size(), 0);

    if (data.empty() || shortWindow <= 0 || longWindow <= 0 || shortWindow >= longWindow) {
        return signals;
    }

    std::vector<double> prefix(data.size() + 1, 0.0);
    for (size_t i = 0; i < data.size(); ++i) {
        prefix[i + 1] = prefix[i] + data[i].close;
    }

    auto getMA = [&](size_t endIdx, int window) {
        size_t startIdx = endIdx + 1 - window;
        double sum = prefix[endIdx + 1] - prefix[startIdx];
        return sum / window;
    };

    for (size_t i = longWindow; i < data.size(); ++i) {
        double shortMA_today = getMA(i, shortWindow);
        double longMA_today = getMA(i, longWindow);
        double shortMA_yesterday = getMA(i - 1, shortWindow);
        double longMA_yesterday = getMA(i - 1, longWindow);

        if (shortMA_yesterday <= longMA_yesterday && shortMA_today > longMA_today) {
            signals[i] = 1;
        } else if (shortMA_yesterday >= longMA_yesterday && shortMA_today < longMA_today) {
            signals[i] = -1;
        }
    }

    return signals;
}

RSIStrategy::RSIStrategy(int p, double ob, double os)
    : period(p), overbought(ob), oversold(os) {}

std::string RSIStrategy::getName() const {
    return "RSI";
}

std::vector<int> RSIStrategy::generateSignals(const std::vector<Bar>& data) {
    std::vector<int> signals(data.size(), 0);

    if (data.size() <= static_cast<size_t>(period) || period <= 0) {
        return signals;
    }

    for (size_t i = period; i < data.size(); ++i) {
        double gains = 0.0;
        double losses = 0.0;

        for (size_t j = i - period + 1; j <= i; ++j) {
            double change = data[j].close - data[j - 1].close;
            if (change > 0) {
                gains += change;
            } else {
                losses -= change;
            }
        }

        double avgGain = gains / period;
        double avgLoss = losses / period;

        double rsi;
        if (avgLoss == 0.0) {
            rsi = 100.0;
        } else {
            double rs = avgGain / avgLoss;
            rsi = 100.0 - (100.0 / (1.0 + rs));
        }

        if (rsi < oversold) {
            signals[i] = 1;
        } else if (rsi > overbought) {
            signals[i] = -1;
        }
    }

    return signals;
}