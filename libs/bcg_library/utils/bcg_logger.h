//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_LOGGER_H
#define BCG_GRAPHICS_BCG_LOGGER_H

#include <string>
#include <cassert>
#include <iostream>

namespace bcg {

struct logger {
    enum Verboosity {
        Quiet = 0,
        Info,
        Warning,
        Error,
        All
    };

    static Verboosity mode;

    static void info(const std::string &message);

    static void warn(const std::string &message);

    static void error(const std::string &message);

    static void error_if(bool condition, std::string message = "");
};
}


namespace bcg {

void logger::info(const std::string &message) {
    if (mode >= Verboosity::Info) std::cout << "BCG::INFO: " << message << "\n";
}

void logger::warn(const std::string &message) {
    if (mode >= Verboosity::Warning) std::cout << "BCG::WARN: " << message << "\n";
}

void logger::error(const std::string &message) {
    if (mode >= Verboosity::Error) std::cerr << "BCG::ERROR: " << message << "\n";
}

void logger::error_if(bool condition, std::string message) {
    if (!condition){
        std::cerr << "BCG::ERROR_IF: " << message << "\n";
        assert(false);
    }
}
}

#endif //BCG_GRAPHICS_BCG_LOGGER_H
