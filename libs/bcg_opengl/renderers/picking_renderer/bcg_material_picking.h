//
// Created by alex on 06.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_PICKING_H
#define BCG_GRAPHICS_BCG_MATERIAL_PICKING_H

#include "color/bcg_colors.h"
#include "entt/entt.hpp"
#include "renderers/bcg_attribute.h"

namespace bcg{

struct material_picking{
    material_picking() = default;

    explicit material_picking(entt::entity id) : picking_color(color::packed_int(static_cast<std::uint32_t>(id))){}

    std::vector<attribute> attributes = {
            {"position", "v_position", "v_position", 0, true}
    };

    ogl_vertex_array vao;

    VectorS<3> picking_color;
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_PICKING_H
