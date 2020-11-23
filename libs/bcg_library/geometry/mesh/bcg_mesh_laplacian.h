//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_LAPLACIAN_H
#define BCG_GRAPHICS_BCG_MESH_LAPLACIAN_H

#include "bcg_mesh.h"
#include "math/laplacian/bcg_laplacian_matrix.h"

namespace bcg {

enum class MeshLaplacianStiffness : unsigned int{
    uniform,
    cotan,
    fujiwara,
    __last__
};

enum class MeshLaplacianMass : unsigned int{
    uniform,
    cotan,
    fujiwara,
    voronoi,
    barycentric,
    __last__
};

struct mesh_laplacian : public laplacian_matrix{
    MeshLaplacianStiffness s_type;
    MeshLaplacianMass m_type;
};

std::vector<std::string> mesh_laplacian_stiffness();

std::vector<std::string> mesh_laplacian_mass();

mesh_laplacian
build_laplacian(halfedge_mesh &mesh, MeshLaplacianStiffness s_type, MeshLaplacianMass m_type,
                size_t parallel_grain_size = 1024,
                std::string edge_scaling_property_name = "");

}

#endif //BCG_GRAPHICS_BCG_GUI_MESH_LAPLACIAN_H
