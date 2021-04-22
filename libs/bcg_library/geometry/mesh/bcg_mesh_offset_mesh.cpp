//
// Created by alex on 13.04.21.
//

#include "bcg_mesh_offset_mesh.h"

#include "bcg_property_map_eigen.h"
#include "math/bcg_probabilistic_quadric.h"
#include "bcg_mesh_face_quadric.h"
#include "bcg_mesh_vertex_quadric.h"
#include "bcg_mesh_face_normals.h"
#include "bcg_mesh_face_centers.h"
#include "tbb/tbb.h"

namespace bcg{

using quadric = pq::quadric<pq::math<bcg_scalar_t, VectorS<3>, VectorS<3>, MatrixS<3, 3>>>;

void mesh_normal_offset_mesh(halfedge_mesh &mesh, bcg_scalar_t offset){
    auto positions = mesh.positions;
    auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");

    Map(positions).array() += MapConst(normals).array() * offset;
    positions.set_dirty();
}

void mesh_face_normal_offset_mesh(halfedge_mesh &mesh, bcg_scalar_t offset, bcg_scalar_t sigma_p, size_t parallel_grain_size){
    auto positions = mesh.positions;
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    std::vector<VectorS<3>> V;
                    VectorS<3> n = face_normal(mesh, f);
                    for (const auto &vf : mesh.get_vertices(f)) {
                        V.emplace_back(mesh.positions[vf] + n * offset);
                    }
                    quadrics[f] = quadric::probabilistic_triangle_quadric(V[0], V[1], V[2], sigma_p);
                    //quadrics[f] = quadric::probabilistic_plane_quadric(face_center(mesh, f) + offset * n, n, sigma_p, sigma_p);
                }
            }
    );

    Params params;
    params.use_area_weighting = false;
    mesh_face_quadric_neighbors_avg_to_vertices(mesh, quadrics, params, parallel_grain_size);
    auto quadric_avg = mesh.vertices.get<quadric, 1>("v_quadric_avg");
    mesh_vertex_quadric_minimizer(mesh, quadric_avg, parallel_grain_size);
    auto old_positions = mesh.vertices.get<VectorS<3>, 3>("v_position_old");
    auto quadric_minimizer = mesh.vertices.get<VectorS<3>, 3>("v_quadric_minimizer");
    auto offset_resolve = mesh.vertices.get_or_add<VectorS<3>, 3>("v_offset_resolve");
    auto diff_norm = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_diff_norm");
    MatrixS<-1, 3> diff = MapConst(old_positions) - MapConst(positions);
    Map(diff_norm) = (diff.rowwise().norm().array() - std::abs(offset)).cwiseMax(0);
    Map(offset_resolve) = (MapConst(diff_norm).transpose().array()).array() * diff.rowwise().normalized().array();
    Map(positions) = MapConst(quadric_minimizer) + MapConst(offset_resolve);
    positions.set_dirty();
    offset_resolve.set_dirty();
    diff_norm.set_dirty();
}

}