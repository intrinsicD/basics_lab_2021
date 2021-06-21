//
// Created by alex on 17.11.20.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_MATERIAL_H
#define BCG_GRAPHICS_BCG_COMPONENT_MATERIAL_H

#include <vector>
#include "renderers/bcg_attribute.h"
#include "viewer/bcg_opengl.h"

namespace bcg{

struct material{
    virtual ~material() = default;

    std::string name;

    std::vector<attribute> attributes;
    std::vector<ogl_texture> textures;

    ogl_vertex_array vao;
    size_t num_elements;

    virtual void upload(const glsl_program &){
        for(auto &texture : textures){
            if(texture.is_valid()){
                texture.activate();
            }
        }
    }
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_MATERIAL_H
