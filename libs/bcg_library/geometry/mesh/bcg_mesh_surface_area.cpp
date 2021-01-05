//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_surface_area.h"
#include "bcg_mesh_vertex_area_barycentric.h"

namespace bcg{

bcg_scalar_t surface_area(const halfedge_mesh &mesh){
    bcg_scalar_t area = 0.0;
    for(const auto v : mesh.vertices){
        area += vertex_barycentric_area(mesh, v);
    }
    return area;
}

}