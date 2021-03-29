//
// Created by alex on 29.03.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_VERTEX_EXTREMAL_VALUES_H
#define BCG_GRAPHICS_BCG_MESH_VERTEX_EXTREMAL_VALUES_H

#include "bcg_mesh.h"

namespace bcg {

enum class MeshVertexExtremalType{
    minimum,
    maximum,
    extremal,
    __last__
};

std::vector<std::string> mesh_vertex_extremal_type_names();

void
mesh_vertex_extremal_values(halfedge_mesh &mesh, property<bcg_scalar_t, 1> property, size_t parallel_grain_size = 1024);

void
mesh_vertex_minimal_values(halfedge_mesh &mesh, property<bcg_scalar_t, 1> property, size_t parallel_grain_size = 1024);

void
mesh_vertex_maximal_values(halfedge_mesh &mesh, property<bcg_scalar_t, 1> property, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_VERTEX_EXTREMAL_VALUES_H
