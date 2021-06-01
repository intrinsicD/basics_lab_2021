//
// Created by alex on 31.05.21.
//

#include "bcg_mesh_dual.h"
#include "bcg_mesh_face_centers.h"

namespace bcg{

halfedge_mesh mesh_make_dual(const halfedge_mesh &mesh){
    halfedge_mesh tmp;
    tmp.assign(mesh);
    mesh_dual(tmp);
    return tmp;
}

void mesh_dual(halfedge_mesh &mesh){
    halfedge_mesh tmp;

    // a property to remember new vertices per face
    auto fvertex = mesh.faces.get_or_add<vertex_handle, 1>("f_vertex");

    // for each face add the centroid to the dual mesh
    for (auto f : mesh.faces){
        fvertex[f] = tmp.add_vertex(face_center(mesh, f));
    }

    // add new face for each vertex
    for (auto v : mesh.vertices) {
        std::vector<vertex_handle> vertices;
        for (auto f : mesh.get_faces(v)){
            vertices.push_back(fvertex[f]);
        }

        tmp.add_face(vertices);
    }

    // swap old and new meshes, don't copy properties
    mesh.assign(tmp);
}

}