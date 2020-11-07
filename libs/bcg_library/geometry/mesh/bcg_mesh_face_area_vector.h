//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FACE_AREA_VECTOR_H
#define BCG_GRAPHICS_BCG_MESH_FACE_AREA_VECTOR_H

#include "bcg_mesh.h"

namespace bcg{

VectorS<3> face_area_vector(const halfedge_mesh &mesh, face_handle f);

}

#endif //BCG_GRAPHICS_BCG_MESH_FACE_AREA_VECTOR_H
