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

    std::string pretty_report() {
        auto elapsed_time = measure<NANOSECONDS>();
        std::string message = "elapsed time: ";
        if(elapsed_time > H){
            size_t hours = elapsed_time / H;
            elapsed_time -= hours * H;
            size_t minutes = elapsed_time / MIN;
            elapsed_time -= minutes * MIN;
            double seconds = elapsed_time / (double)SEC;
            message += std::to_string(hours) + " h " + std::to_string(minutes) + " min " + std::to_string(seconds) + " s";
            value = clock_t::now();
            return message;
        }
        if(elapsed_time > MIN){
            size_t minutes = elapsed_time / MIN;
            elapsed_time -= minutes * MIN;
            double seconds = elapsed_time / (double)SEC;
            message += std::to_string(minutes) + " min " + std::to_string(seconds) + " s";
            value = clock_t::now();
            return message;
        }
        if(elapsed_time > SEC){
            double seconds = elapsed_time / (double)SEC;
            message += std::to_string(seconds) + " s";
            value = clock_t::now();
            return message;
        }
        if(elapsed_time > MILLIS){
            double millis = elapsed_time / (double)MILLIS;
            message += std::to_string(millis) + " milli s";
            value = clock_t::now();
            return message;
        }
        if(elapsed_time > MICROS){
            double micros = elapsed_time / (double)MICROS;
            message += std::to_string(micros) + " micro s";
            value = clock_t::now();
            return message;
        }else{
            message += std::to_string(elapsed_time) + " nano s";
            value = clock_t::now();
            return message;
        }
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

    size_t NANOS = 1;
    size_t MICROS = 1000 * NANOS;
    size_t MILLIS = 1000 * MICROS;
    size_t SEC = 1000 * MILLIS;
    size_t MIN = 60 * SEC;
    size_t H = 60 * MIN;
};

}
#endif //VIEWER_TIMER_H
