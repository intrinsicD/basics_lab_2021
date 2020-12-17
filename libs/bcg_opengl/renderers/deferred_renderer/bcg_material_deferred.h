//
// Created by alex on 15.12.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_DEFERRED_H
#define BCG_GRAPHICS_BCG_MATERIAL_DEFERRED_H

#include "color/bcg_colors.h"
#include "renderers/bcg_material.h"

namespace bcg {

struct material_deferred : public material {
    material_deferred() {
        attributes = {
                {"position", "v_position", "v_position", 0, true},
                {"normal",   "v_normal",   "v_normal",   1, true},
                {"color",    "",           "",           2, false},
        };
    }

    ogl_vertex_array vao;
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_DEFERRED_H
