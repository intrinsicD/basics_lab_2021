//
// Created by alex on 28.11.20.
//

#include "bcg_mesh_statistics.h"
#include "graph/bcg_graph_edge_lengths.h"
#include "bcg_mesh_vertex_valences.h"
#include "bcg_property_map_eigen.h"
#include "bcg_mesh_face_areas.h"


namespace bcg{

mesh_stats mesh_statistics(halfedge_mesh &mesh, size_t parallel_grain_size){
    mesh_stats stats{};

    vertex_valences(mesh, parallel_grain_size);
    auto valence = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_valence");
    stats.vertices.min_valence = MapConst(valence).minCoeff();
    stats.vertices.max_valence = MapConst(valence).maxCoeff();
    stats.vertices.avg_valence = MapConst(valence).mean();
    edge_lengths(mesh, parallel_grain_size);
    auto lengths = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_length");
    stats.edges.min_length = MapConst(lengths).minCoeff();
    stats.edges.max_length = MapConst(lengths).maxCoeff();
    stats.edges.avg_length = MapConst(lengths).mean();
    face_areas(mesh, parallel_grain_size);
    auto areas = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_area");
    stats.faces.min_area = MapConst(areas).minCoeff();
    stats.faces.max_area = MapConst(areas).maxCoeff();
    stats.faces.avg_area = MapConst(areas).mean();
    return stats;
}

}