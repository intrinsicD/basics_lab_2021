//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_STATISTICS_RUNNING_H
#define BCG_GRAPHICS_BCG_STATISTICS_RUNNING_H

#include <cstddef>

namespace bcg {

struct running_stats {
    running_stats();

    void clear();

    void push(double x);

    size_t size() const;

    double mean() const;

    double variance() const;

    double standard_deviation() const;

    double skewness() const;

    double kurtosis() const;

    double median() const;

    friend running_stats operator+(const running_stats a, const running_stats b);

    running_stats &operator+=(const running_stats &rhs);

private:
    size_t n;
    double M1, M2, M3, M4, M5;
};

}

#endif //BCG_GRAPHICS_BCG_STATISTICS_RUNNING_H
