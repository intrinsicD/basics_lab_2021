//
// Created by alex on 29.04.21.
//

#ifndef BCG_GRAPHICS_BCG_HQ_PBR_UFSI_MATERIAL_H
#define BCG_GRAPHICS_BCG_HQ_PBR_UFSI_MATERIAL_H

#include "color/bcg_colors.h"
#include "bcg_opengl/renderers/bcg_material.h"

namespace bcg{

struct hq_pbr_ufsi_material : public material{
    hq_pbr_ufsi_material() {
        attributes = {
                {"position",   "v_position", "v_position", 0, true},
                {"color",      "",           "",           1, false},
                {"normal", "", "", 2, false},
        };
    }

    float field_of_view_radians;
    int screen_height_in_pixels;
    bool use_uniform_color = true;
    Vector<float, 3> uniform_color = color<float>::random();
    float alpha = 1.0;
    float uniform_size = 1.0;

    void upload(const glsl_program &program) override {
        program.set_uniform_3f("material.uniform_color", 1, uniform_color.data());
        program.set_uniform_i("material.use_uniform_color", use_uniform_color);
        program.set_uniform_f("material.alpha", alpha);
        program.set_uniform_f("material.uniform_size", uniform_size);
        program.set_uniform_f("field_of_view_radians", field_of_view_radians);
        program.set_uniform_i("screen_height_in_pixels", screen_height_in_pixels);
        material::upload(program);
    }
};

}

#endif //BCG_GRAPHICS_BCG_HQ_PBR_UFSI_MATERIAL_H
