//
// Created by alex on 04.12.20.
//

#include <cmath>
#include <algorithm>
#include "bcg_statistics_running.h"

namespace bcg {

running_stats::running_stats() : n(0), M1(0), M2(0), M3(0), M4(0), M5(0),
                                 MIN(std::numeric_limits<double>::max()),
                                 MAX(std::numeric_limits<double>::min()) {
    clear();
}

void running_stats::clear() {
    n = 0;
    M1 = M2 = M3 = M4 = M5 = 0.0;
    MIN = std::numeric_limits<double>::max();
    MAX = std::numeric_limits<double>::min();
}

void running_stats::push(double x) {
    double delta, delta_n, delta_n2, term1;

    size_t n1 = n;
    n++;
    delta = x - M1;
    delta_n = delta / n;
    delta_n2 = delta_n * delta_n;
    term1 = delta * delta_n * n1;
    M1 += delta_n;
    M4 += term1 * delta_n2 * double(n * n - 3 * n + 3) + 6 * delta_n2 * M2 - 4 * delta_n * M3;
    M3 += term1 * delta_n * double(n - 2) - 3 * delta_n * M2;
    M2 += term1;
    //Jeff McClintock running median approximation
    term1 = x - M5;
    M5 += copysign(std::max(M1 * 0.01, std::abs(term1)), term1);
    MIN = fmin(x, MIN);
    MAX = fmax(x, MAX);
}

size_t running_stats::size() const {
    return n;
}

double running_stats::mean() const {
    return M1;
}

double running_stats::variance() const {
    return M2 / (n - 1.0);
}

double running_stats::standard_deviation() const {
    return std::sqrt(variance());
}

double running_stats::skewness() const {
    return sqrt(double(n)) * M3 / std::pow(M2, 1.5);
}

double running_stats::kurtosis() const {
    return double(n) * M4 / (M2 * M2) - 3.0;
}

double running_stats::median() const {
    //Jeff McClintock running median approximation
    return M5;
}

double running_stats::min() const {
    return MIN;
}

double running_stats::max() const {
    return MAX;
}

running_stats operator+(const running_stats a, const running_stats b) {
    running_stats combined;

    combined.n = a.n + b.n;

    double delta = b.M1 - a.M1;
    double delta2 = delta * delta;
    double delta3 = delta * delta2;
    double delta4 = delta2 * delta2;

    combined.M1 = (a.n * a.M1 + b.n * b.M1) / combined.n;

    combined.M2 = a.M2 + b.M2 +
                  delta2 * a.n * b.n / combined.n;

    combined.M3 = a.M3 + b.M3 +
                  delta3 * a.n * b.n * double(a.n - b.n) / double(combined.n * combined.n);
    combined.M3 += 3.0 * delta * (a.n * b.M2 - b.n * a.M2) / combined.n;

    combined.M4 = a.M4 + b.M4 + delta4 * a.n * b.n * double(a.n * a.n - a.n * b.n + b.n * b.n) /
                                double(combined.n * combined.n * combined.n);
    combined.M4 +=
            6.0 * delta2 * (double(a.n * a.n) * b.M2 + double(b.n * b.n) * a.M2) / double(combined.n * combined.n) +
            4.0 * delta * (a.n * b.M3 - b.n * a.M3) / combined.n;

    return combined;
}

running_stats &running_stats::operator+=(const running_stats &rhs) {
    running_stats combined = *this + rhs;
    *this = combined;
    return *this;
}

}