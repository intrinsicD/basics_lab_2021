//
// Created by alex on 01.04.21.
//

#include "bcg_mesh_split_path.h"
#include "bcg_mesh_connected_components.h"
#include "utils/bcg_timer.h"

namespace bcg{

void mesh_remove_path(halfedge_mesh &mesh, property<bcg_scalar_t, 1> path){
    Timer timer;
    for (const auto &v : mesh.vertices) {
        if (path[v] > 0) {
            mesh.delete_vertex(v);
        }
    }
    std::cout << "delete vertices " << timer.pretty_report() << "\n";
    mesh.garbage_collection();
    std::cout << "garbage collection " << timer.pretty_report() << "\n";
}

std::vector<halfedge_mesh> mesh_split_path(halfedge_mesh &mesh, property<bcg_scalar_t, 1> path){
    auto *original = new halfedge_mesh();
    original->assign(mesh);
    mesh_remove_path(*original, path);
    auto components = mesh_connected_components_split(*original);
    delete original;
    return components;
}

}