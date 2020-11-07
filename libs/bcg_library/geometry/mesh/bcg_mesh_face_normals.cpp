//
// Created by alex on 06.11.20.
//

#include "bcg_mesh_face_normals.h"
#include "bcg_mesh_face_area_vector.h"

namespace bcg{

VectorS<3> face_normal(const halfedge_mesh &mesh, face_handle f) {
    return face_area_vector(mesh, f).normalized();
}

}