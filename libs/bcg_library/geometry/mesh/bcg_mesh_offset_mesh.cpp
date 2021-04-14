//
// Created by alex on 13.04.21.
//

#include "bcg_mesh_offset_mesh.h"
#include "bcg_property_map_eigen.h"

namespace bcg{

void mesh_offset_mesh(halfedge_mesh &mesh, bcg_scalar_t offset){
    auto positions = mesh.positions;
    auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");

    Map(positions).array() += MapConst(normals).array() * offset;
    positions.set_dirty();
}

}