//
// Created by alex on 29.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_POINTS_H
#define BCG_GRAPHICS_BCG_MATERIAL_POINTS_H

#include "color/bcg_colors.h"
#include "renderers/bcg_material.h"

namespace bcg{

struct material_points : public material{
    material_points() {
        attributes = {
                {"position",   "v_position", "v_position", 0, true},
                {"color",      "",           "",           1, false},
                {"point_size", "",           "",           2, false},
                {"normal", "v_normal", "v_normal", 3, false},
        };
    }

    bool use_uniform_color = true;
    bool use_uniform_size = true;
    bool has_normals = false;

    Vector<float, 3> uniform_color = color<float>::random();
    Vector<float, 4> viewport;
    float uniform_size = 1.0;
    float alpha = 1.0;

    void upload(const glsl_program &program) override {
        program.set_uniform_i("material.use_uniform_point_size", use_uniform_size);
        program.set_uniform_i("has_normals", has_normals);
        program.set_uniform_f("material.uniform_point_size", uniform_size);
        program.set_uniform_i("material.use_uniform_color", use_uniform_color);
        program.set_uniform_3f("material.uniform_color", 1, uniform_color.data());
        program.set_uniform_f("material.alpha", alpha);
        program.set_uniform_4f("viewport", 1, viewport.data());
        material::upload(program);
    }
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_POINTS_H
