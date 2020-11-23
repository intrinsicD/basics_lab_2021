//
// Created by alex on 19.11.20.
//

#include "bcg_mesh_boundary.h"
#include "tbb/tbb.h"

namespace bcg{

boundary_elements mesh_boundary(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto v_boundary = mesh.vertices.get_or_add<bool, 1>("v_boundary");
    auto e_boundary = mesh.edges.get_or_add<bool, 1>("e_boundary");
    auto f_boundary = mesh.faces.get_or_add<bool, 1>("f_boundary");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    f_boundary[f] = mesh.is_boundary(f);

                    for(auto h : mesh.get_halfedges(f)){
                        auto e = mesh.get_edge(h);
                        e_boundary[e] = mesh.is_boundary(e);
                        auto v = mesh.get_vertex(e,  0);
                        v_boundary[v] = mesh.is_boundary(v);
                    }
                }
            }
    );

    boundary_elements boundary;
    for(const auto f : mesh.faces){
        if(f_boundary[f]){
            boundary.boundary_faces.emplace_back(f);
        }
    }
    for(const auto e : mesh.edges){
        if(e_boundary[e]){
            boundary.boundary_edges.emplace_back(e);
        }
    }
    for(const auto v : mesh.vertices){
        if(v_boundary[v]){
            boundary.boundary_vertices.emplace_back(v);
        }
    }
    v_boundary.set_dirty();
    e_boundary.set_dirty();
    f_boundary.set_dirty();
    return boundary;
}

void mesh_clear_boundary(halfedge_mesh &mesh){
    auto v_boundary = mesh.vertices.get_or_add<bool, 1>("v_boundary");
    auto e_boundary = mesh.edges.get_or_add<bool, 1>("e_boundary");
    auto f_boundary = mesh.faces.get_or_add<bool, 1>("f_boundary");

    mesh.vertices.remove(v_boundary);
    mesh.edges.remove(e_boundary);
    mesh.faces.remove(f_boundary);
}

}