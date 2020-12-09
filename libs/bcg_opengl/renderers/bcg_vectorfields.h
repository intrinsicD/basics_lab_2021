//
// Created by alex on 11.11.20.
//

#ifndef BCG_GRAPHICS_BCG_VECTORFIELDS_H
#define BCG_GRAPHICS_BCG_VECTORFIELDS_H

#include <string>
#include <unordered_map>
#include <vector>

#include "vectorfield_renderer/bcg_material_vectorfield.h"

namespace bcg{

struct vectorfields{
    std::string current_vertex_vectorfield_name = "";
    std::string current_edge_vectorfield_name = "";
    std::string current_face_vectorfield_name = "";
    std::unordered_map<std::string, material_vectorfield> vertex_vectorfields;
    std::unordered_map<std::string, material_vectorfield> edge_vectorfields;
    std::unordered_map<std::string, material_vectorfield> face_vectorfields;

    void set_clean() {
        for(auto &item : vertex_vectorfields){
            for(auto &attribute : item.second.attributes){
                attribute.update = false;
            }
        }
        for(auto &item : edge_vectorfields){
            for(auto &attribute : item.second.attributes){
                attribute.update = false;
            }
        }
        for(auto &item : face_vectorfields){
            for(auto &attribute : item.second.attributes){
                attribute.update = false;
            }
        }
    }
};

}

#endif //BCG_GRAPHICS_BCG_VECTORFIELDS_H
