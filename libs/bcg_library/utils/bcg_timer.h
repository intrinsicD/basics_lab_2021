//
// Created by alex on 28.02.19.
//

#ifndef BCG_TIMER_H
#define BCG_TIMER_H

#include <chrono>
#include <string>

namespace bcg {

using NANOSECONDS = std::chrono::nanoseconds;
using MICROSECONDS = std::chrono::microseconds;
using MILLISECONDS = std::chrono::milliseconds;
using SECONDS = std::chrono::seconds;
using MINUTES = std::chrono::minutes;
using HOURS = std::chrono::hours;

struct Timer {
public:
    using clock_t = std::chrono::high_resolution_clock;

    clock_t::time_point value;

    Timer() : value(clock_t::now()) {};

    template<typename Duration>
    size_t measure() const {
        return std::chrono::duration_cast<Duration>(clock_t::now() - value).count();
    }

    template<typename Duration>
    std::string report() {
        std::string message = "elapsed time: " + std::to_string(measure<Duration>()) + unit<Duration>();
        value = clock_t::now();
        return message;
    }

private:
    template<typename Duration>
    static std::string unit() {
        if (std::is_same<Duration, NANOSECONDS>::value) {
            return " nano s";
        }
        if (std::is_same<Duration, MICROSECONDS>::value) {
            return " micro s";
        }
        if (std::is_same<Duration, MILLISECONDS>::value) {
            return " milli s";
        }
        if (std::is_same<Duration, SECONDS>::value) {
            return " s";
        }
        if (std::is_same<Duration, MINUTES>::value) {
            return " min";
        }
        if (std::is_same<Duration, HOURS>::value) {
            return " h";
        }
        return " ";
    }
};

}
#endif //VIEWER_TIMER_H
