//
// Created by alex on 20.01.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_COVMESH_H
#define BCG_GRAPHICS_BCG_MESH_COVMESH_H

#include "bcg_mesh.h"

namespace bcg {

void mesh_convert_vertex_based_covmesh(halfedge_mesh &mesh, bcg_scalar_t kernel_sigma_0 = 0.01);

void mesh_convert_face_based_covmesh(halfedge_mesh &mesh, bcg_scalar_t kernel_sigma_0 = 0.01);

void mesh_convert_gaussian_mixture_covmesh(halfedge_mesh &mesh, size_t num_gaussians);

void mesh_convert_hierarchical_gaussian_mixture_covmesh(halfedge_mesh &mesh, size_t levels);

}


#endif //BCG_GRAPHICS_BCG_MESH_COVMESH_H
