//
// Created by alex on 06.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_PICKING_H
#define BCG_GRAPHICS_BCG_MATERIAL_PICKING_H

#include "entt/entt.hpp"
#include "color/bcg_colors.h"
#include "components/bcg_component_material.h"

namespace bcg{

struct material_picking : public material{
    material_picking() = default;

    explicit material_picking(entt::entity id) : picking_color(color<float>::packed_int(static_cast<std::uint32_t>(id))){
        attributes = {
                {"position", "v_position", "v_position", 0, true}
        };
    }

    ogl_vertex_array vao;

    Vector<float, 3> picking_color;

    void upload(const glsl_program &program) override {
        program.set_uniform_3f("material.picking_color", 1, picking_color.data());
        program.set_uniform_f("uniform_point_size", 20.0f);
        material::upload(program);
    }
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_PICKING_H
