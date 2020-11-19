//
// Created by alex on 19.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FEATURES_H
#define BCG_GRAPHICS_BCG_MESH_FEATURES_H

#include "bcg_mesh.h"

namespace bcg{

void mesh_features_boundary(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

void mesh_features_dihedral_angle(halfedge_mesh &mesh, bcg_scalar_t threshold_degrees = 45, size_t parallel_grain_size = 1024);

void mesh_features(halfedge_mesh &mesh, bool boundary, bool angle, bcg_scalar_t threshold_degrees = 45, size_t parallel_grain_size = 1024);

void mesh_clear_features(halfedge_mesh &mesh);

}

#endif //BCG_GRAPHICS_BCG_MESH_FEATURES_H
