//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_VERTEX_NORMALS_H
#define BCG_GRAPHICS_BCG_MESH_VERTEX_NORMALS_H

#include "bcg_mesh.h"

namespace bcg {

VectorS<3> vertex_normal_uniform(halfedge_mesh &mesh, vertex_handle v);

VectorS<3> vertex_normal_area(halfedge_mesh &mesh, vertex_handle v);

VectorS<3> vertex_normal_angle(halfedge_mesh &mesh, vertex_handle v);

VectorS<3> vertex_normal_area_angle(halfedge_mesh &mesh, vertex_handle v);

void vertex_normals(halfedge_mesh &mesh, std::function<VectorS<3>(halfedge_mesh &, vertex_handle)> method,
                    size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_VERTEX_NORMALS_H
