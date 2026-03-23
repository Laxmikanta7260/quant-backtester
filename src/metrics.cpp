#include "metrics.h"

#include <cmath>

double Metrics::computeTotalReturn(const std::vector<double>& equity) {
    if (equity.empty()) return 0.0;
    return (equity.back() - equity.front()) / equity.front();
}

double Metrics::computeMaxDrawdown(const std::vector<double>& equity) {
    if (equity.empty()) return 0.0;

    double peak = equity[0];
    double maxDD = 0.0;

    for (double value : equity) {
        if (value > peak) peak = value;
        double drawdown = (peak - value) / peak;
        if (drawdown > maxDD) maxDD = drawdown;
    }

    return maxDD;
}

double Metrics::computeSharpeRatio(const std::vector<double>& equity) {
    if (equity.size() < 2) return 0.0;

    std::vector<double> returns;
    for (std::size_t i = 1; i < equity.size(); ++i) {
        returns.push_back((equity[i] - equity[i - 1]) / equity[i - 1]);
    }

    double mean = 0.0;
    for (double r : returns) mean += r;
    mean /= returns.size();

    double variance = 0.0;
    for (double r : returns) variance += (r - mean) * (r - mean);
    variance /= returns.size();

    double stddev = std::sqrt(variance);
    if (stddev == 0.0) return 0.0;

    return mean / stddev;
}

double Metrics::computeBuyAndHoldReturn(const std::vector<Bar>& data) {
    if (data.empty()) return 0.0;
    return (data.back().close - data.front().close) / data.front().close;
}