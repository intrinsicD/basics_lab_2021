//
// Created by alex on 29.03.21.
//

#include "bcg_mesh_vertex_extremal_values.h"
#include "tbb/tbb.h"

namespace bcg{

std::vector<std::string> mesh_vertex_extremal_type_names(){
    std::vector<std::string> names(static_cast<int>(MeshVertexExtremalType::__last__));
    names[static_cast<int>(MeshVertexExtremalType::minimum)] = "minimum";
    names[static_cast<int>(MeshVertexExtremalType::maximum)] = "maximum";
    names[static_cast<int>(MeshVertexExtremalType::extremal)] = "extremal";
    return names;
}

void
mesh_vertex_extremal_values(halfedge_mesh &mesh, property<bcg_scalar_t, 1> property, size_t parallel_grain_size){
    auto extremal_values = mesh.vertices.get_or_add<bool, 1>("v_extremal_values");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    extremal_values[v] = true;
                    for(const auto &vv : mesh.halfedge_graph::get_vertices(v)){
                        if(std::abs(property[vv]) > std::abs(property[v])){
                            extremal_values[v] = false;
                            break;
                        }
                    }
                }
            }
    );
    extremal_values.set_dirty();
}

void
mesh_vertex_minimal_values(halfedge_mesh &mesh, property<bcg_scalar_t, 1> property, size_t parallel_grain_size){
    auto minimal_values = mesh.vertices.get_or_add<bool, 1>("v_minimal_values");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    minimal_values[v] = true;
                    for(const auto &vv : mesh.halfedge_graph::get_vertices(v)){
                        if(property[vv] < property[v]){
                            minimal_values[v] = false;
                            break;
                        }
                    }
                }
            }
    );
    minimal_values.set_dirty();
}

void
mesh_vertex_maximal_values(halfedge_mesh &mesh, property<bcg_scalar_t, 1> property, size_t parallel_grain_size){
    auto maximal_values = mesh.vertices.get_or_add<bool, 1>("v_maximal_values");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    maximal_values[v] = true;
                    for(const auto &vv : mesh.halfedge_graph::get_vertices(v)){
                        if(property[vv] > property[v]){
                            maximal_values[v] = false;
                            break;
                        }
                    }
                }
            }
    );
    maximal_values.set_dirty();
}

}