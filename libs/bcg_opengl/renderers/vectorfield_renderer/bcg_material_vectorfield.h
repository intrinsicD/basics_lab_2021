//
// Created by alex on 11.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_VECTORFIELD_H
#define BCG_GRAPHICS_BCG_MATERIAL_VECTORFIELD_H

#include "color/bcg_colors.h"
#include "renderers/bcg_material.h"

namespace bcg{

struct material_vectorfield : public material{
    material_vectorfield() {
        attributes = {
                {"position", "v_position","v_position", 0, true, false},
                {"vectors", "", "", 1, false, false},
                {"color", "", "", 2, false, false},
        };
    }


    bool enabled = false;

    ogl_vertex_array vao;

    bool use_uniform_color = true;
    bool use_uniform_size = true;

    VectorS<3> uniform_color = color::random();
    bcg_scalar_t uniform_size = 1.0;
    bcg_scalar_t uniform_alpha = 1.0;
    size_t num_vectors = 0;

    void upload(const glsl_program &program) override {
        program.set_uniform_i("material.use_uniform_vector_length", use_uniform_size);
        program.set_uniform_f("material.uniform_vector_length", uniform_size);
        program.set_uniform_i("material.use_uniform_color", use_uniform_color);
        program.set_uniform_3f("material.uniform_color", 1, uniform_color.data());
        program.set_uniform_f("material.alpha", uniform_alpha);
        material::upload(program);
    }
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_VECTORFIELD_H
