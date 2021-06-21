//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_CURVE_H
#define BCG_GRAPHICS_BCG_MATERIAL_CURVE_H

#include "color/bcg_colors.h"
#include "components/bcg_component_material.h"

namespace bcg {

struct material_curve : public material {
    material_curve() {
        attributes = {
                {"position", "v_position", "v_position", 0, true},
        };
    }

    ~material_curve() override = default;

    bool show_bezier = false, show_hermite = true;

    Vector<float, 3> uniform_color = color<float>::random();
    float uniform_alpha = 1;
    int tesselation_level = 16;

    void use_hermite(const glsl_program &program) {
        program.set_uniform_i("show_hermite", show_hermite);
        program.set_uniform_i("show_bezier", !show_hermite);
    }

    void use_bezier(const glsl_program &program) {
        program.set_uniform_i("show_bezier", show_bezier);
        program.set_uniform_i("show_hermite", !show_bezier);
    }

    void upload(const glsl_program &program) override {
        program.set_uniform_3f("material.uniform_color", 1, uniform_color.data());
        program.set_uniform_f("material.alpha", uniform_alpha);
        program.set_uniform_i("tesselation_level", tesselation_level);
        material::upload(program);
    }
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_CURVE_H
