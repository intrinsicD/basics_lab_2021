//
// Created by alex on 29.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_POINTS_H
#define BCG_GRAPHICS_BCG_MATERIAL_POINTS_H

#include "bcg_library/color/bcg_colors.h"
#include "renderers/bcg_attribute.h"
#include "bcg_opengl.h"
#include "renderers/bcg_material.h"

namespace bcg{

struct material_points : public material{
    material_points() {
        attributes = {
                {"position",   "v_position", "v_position", 0, true},
                {"color",      "",           "",           1, false},
                {"point_size", "",           "",           2, false}
        };
    }

    bool use_uniform_color = true;
    bool use_uniform_size = true;

    VectorS<3> uniform_color = color::random();
    bcg_scalar_t uniform_size = 1.0;
    bcg_scalar_t alpha = 1.0;

    void upload(const glsl_program &program) override {
        program.set_uniform_i("material.use_uniform_point_size", use_uniform_size);
        program.set_uniform_f("material.uniform_point_size", uniform_size);
        program.set_uniform_i("material.use_uniform_color", use_uniform_color);
        program.set_uniform_3f("material.uniform_color", 1, uniform_color.data());
        program.set_uniform_f("material.alpha", alpha);
        material::upload(program);
    }
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_POINTS_H
