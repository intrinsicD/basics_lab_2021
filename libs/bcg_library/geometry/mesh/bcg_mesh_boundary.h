//
// Created by alex on 19.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_BOUNDARY_H
#define BCG_GRAPHICS_BCG_MESH_BOUNDARY_H

#include "bcg_mesh.h"

namespace bcg {

struct boundary_elements{
    std::vector<vertex_handle> boundary_vertices;
    std::vector<edge_handle> boundary_edges;
    std::vector<face_handle> boundary_faces;
};

boundary_elements mesh_boundary(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

void mesh_clear_boundary(halfedge_mesh &mesh);

}
#endif //BCG_GRAPHICS_BCG_MESH_BOUNDARY_H
