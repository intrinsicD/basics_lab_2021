//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_RENDER_SYSTEM_H
#define BCG_GRAPHICS_BCG_RENDER_SYSTEM_H

#include "systems/bcg_systems.h"

namespace bcg{

struct render_system : public system{
    explicit render_system(viewer_state *state);

private:
    std::unordered_map<std::string, renderer> renderers;
};

}

#endif //BCG_GRAPHICS_BCG_RENDER_SYSTEM_H
