#ifndef STRATEGY_H
#define STRATEGY_H

#include <string>
#include <vector>
#include "data_loader.h"

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual std::string getName() const = 0;
    virtual std::vector<int> generateSignals(const std::vector<Bar>& data) = 0;
};

class MovingAverageCrossoverStrategy : public Strategy {
private:
    int shortWindow;
    int longWindow;

public:
    MovingAverageCrossoverStrategy(int shortWindow, int longWindow);
    std::string getName() const override;
    std::vector<int> generateSignals(const std::vector<Bar>& data) override;
};

class RSIStrategy : public Strategy {
private:
    int period;
    double overbought;
    double oversold;

public:
    RSIStrategy(int period, double overbought, double oversold);
    std::string getName() const override;
    std::vector<int> generateSignals(const std::vector<Bar>& data) override;
};

#endif