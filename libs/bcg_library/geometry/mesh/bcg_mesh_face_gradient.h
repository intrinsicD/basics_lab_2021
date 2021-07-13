//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FACE_GRADIENT_H
#define BCG_GRAPHICS_BCG_MESH_FACE_GRADIENT_H

#include "bcg_mesh.h"

namespace bcg{

VectorS<3> face_gradient(const halfedge_mesh &mesh, face_handle f, property<bcg_scalar_t , 1> vproperty);

property<VectorS<3>, 3> face_gradients(halfedge_mesh &mesh, property<bcg_scalar_t , 1> vproperty,size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_FACE_GRADIENT_H
