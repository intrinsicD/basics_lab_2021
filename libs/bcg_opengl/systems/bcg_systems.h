//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_SYSTEMS_H
#define BCG_GRAPHICS_BCG_SYSTEMS_H

#include <string>
#include <utility>

#include "../events/bcg_events.h"

namespace bcg {

struct viewer_state;

struct system {
    system() = default;

    system(std::string name, viewer_state *state) : name(std::move(name)), state(state) {}

    virtual ~system() = default;

    std::string name;
    viewer_state *state = nullptr;
};

}

#endif //BCG_GRAPHICS_BCG_SYSTEMS_H
