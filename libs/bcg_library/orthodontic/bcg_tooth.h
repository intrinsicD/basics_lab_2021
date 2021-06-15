//
// Created by alex on 15.06.21.
//

#ifndef BCG_GRAPHICS_BCG_TOOTH_H
#define BCG_GRAPHICS_BCG_TOOTH_H

#include "mesh/bcg_mesh.h"

namespace bcg{

halfedge_graph mesh_incisal_edges(const halfedge_mesh &mesh);

VectorS<3> mesh_canine_cusp(const halfedge_mesh &mesh);

point_cloud mesh_premolar_cusps(const halfedge_mesh &mesh);

point_cloud mesh_molar_cusps(const halfedge_mesh &mesh);

halfedge_graph mesh_grove(const halfedge_mesh &mesh);

std::vector<halfedge_graph> mesh_ridges(const halfedge_mesh &mesh);

halfedge_graph mesh_occlusal_surface_boundary(const std::vector<halfedge_graph> &ridges);

}


#endif //BCG_GRAPHICS_BCG_TOOTH_H
