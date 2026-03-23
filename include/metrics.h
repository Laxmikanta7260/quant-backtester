#ifndef METRICS_H
#define METRICS_H

#include <vector>
#include "data_loader.h"

class Metrics {
public:
    static double computeTotalReturn(const std::vector<double>& equity);
    static double computeMaxDrawdown(const std::vector<double>& equity);
    static double computeSharpeRatio(const std::vector<double>& equity);
    static double computeBuyAndHoldReturn(const std::vector<Bar>& data);
};

#endif