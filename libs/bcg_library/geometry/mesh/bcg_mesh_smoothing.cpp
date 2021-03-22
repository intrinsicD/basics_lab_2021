//
// Created by alex on 23.11.20.
//
#include "bcg_mesh_smoothing.h"
#include "bcg_mesh_edge_cotan.h"
#include "bcg_property_map_eigen.h"
#include "bcg_mesh_surface_area.h"
#include "tbb/tbb.h"

namespace bcg {

void rescale(halfedge_mesh &mesh, const VectorS<3> &center_before, bcg_scalar_t area_before) {
    // restore original surface area
    bcg_scalar_t area_after = surface_area(mesh);
    bcg_scalar_t scale = std::sqrt(area_before / area_after);
    Map(mesh.positions) *= scale;

    // restore original center
    VectorS<3> center_after = Map(mesh.positions).colwise().mean();
    VectorS<3> trans = center_before - center_after;

    Map(mesh.positions).rowwise() += trans.transpose();
}

void
explicit_smoothing(halfedge_mesh &mesh, const mesh_laplacian &laplacian, int smoothing_steps, bcg_scalar_t timestep,
                   size_t parallel_grain_size) {
    VectorS<3> center_before = Map(mesh.positions).colwise().mean();
    bcg_scalar_t area_before = surface_area(mesh);

    explicit_smoothing(mesh, laplacian, mesh.positions, smoothing_steps, timestep, parallel_grain_size);

    rescale(mesh, center_before, area_before);
}

void explicit_smoothing(halfedge_mesh &mesh, const mesh_laplacian &, property<bcg_scalar_t, 1> &property,
                        int smoothing_steps, bcg_scalar_t timestep, size_t parallel_grain_size) {
    auto positions = mesh.vertices.get<VectorS<3>, 3>("v_position");
    auto v_feature = mesh.vertices.get_or_add<bool, 1>("v_feature");
    auto e_weight = mesh.edges.get<bcg_scalar_t, 1>("e_laplacian_weight");
    auto v_weight = mesh.vertices.get<bcg_scalar_t, 1>("v_laplacian_weight");
    auto property_values = property.vector();
    for (int i = 0; i < smoothing_steps; ++i) {
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        if ((v_feature && v_feature[v]) || mesh.is_boundary(v)) {

                        } else {
                            bcg_scalar_t smooth_value = 0.0;
                            bcg_scalar_t weight_sum = 0.0;
                            for (const auto vh : mesh.halfedge_graph::get_halfedges(v)) {
                                auto tv = mesh.get_to_vertex(vh);
                                bcg_scalar_t weight = e_weight[mesh.get_edge(vh)];
                                smooth_value += weight * (property_values[tv] - property_values[v]);
                                weight_sum += weight;
                            }

                            if (weight_sum != 0) {
                                smooth_value /= weight_sum;
                                property[v] += timestep * smooth_value;
                            }
                        }
                    }
                }
        );
    }
    property.set_dirty();
}

void explicit_smoothing(halfedge_mesh &mesh, const mesh_laplacian &, property<VectorS<3>, 3> &property,
                        int smoothing_steps, bcg_scalar_t timestep, size_t parallel_grain_size) {
    auto v_feature = mesh.vertices.get_or_add<bool, 1>("v_feature");
    auto e_weight = mesh.edges.get<bcg_scalar_t, 1>("e_laplacian_weight");
    auto v_weight = mesh.vertices.get<bcg_scalar_t, 1>("v_laplacian_weight");
    auto property_values = property.vector();
    for (int i = 0; i < smoothing_steps; ++i) {
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        if ((v_feature && v_feature[v]) || mesh.is_boundary(v)) {

                        } else {
                            VectorS<3> smooth_value = zero3s;
                            bcg_scalar_t sum_weight = 0;
                            for (const auto vh : mesh.halfedge_graph::get_halfedges(v)) {
                                auto tv = mesh.get_to_vertex(vh);
                                bcg_scalar_t weight = e_weight[mesh.get_edge(vh)];
                                smooth_value += weight * (property_values[tv] - property_values[v]);
                                sum_weight += weight;
                            }

                            if (sum_weight != 0) {
                                smooth_value /= sum_weight;
                                property[v] += timestep * smooth_value;
                            }
                        }
                    }
                }
        );
    }
    property.set_dirty();
}

void taubin_smoothing(halfedge_mesh &mesh, const mesh_laplacian &laplacian, bcg_scalar_t lambda, bcg_scalar_t mu,
                      int smoothing_steps) {
    if(lambda > 0 && -mu > lambda){
        size_t N = mesh.positions.size();
        MatrixS<-1, -1> Id = MatrixS<-1, -1>::Identity(N, N);
        MatrixS<-1, -1> K = Id - laplacian.normalize();
        for(int i = 0; i < smoothing_steps; ++i){
            std::cout << "iter: " << i << "/" << smoothing_steps << "\n";
            Map(mesh.positions) = (Id - mu * K) * ((Id - lambda * K) * MapConst(mesh.positions));
        }
        mesh.positions.set_dirty();
    }else{
        std::cout << "lambda: " << lambda << " must be > 0\n";
        std::cout << "mu: " << -mu << " must be -mu > lambda\n";
    }
}

void implicit_smoothing(halfedge_mesh &mesh, const mesh_laplacian &laplacian, bcg_scalar_t timestep) {
    VectorS<3> center_before = Map(mesh.positions).colwise().mean();
    bcg_scalar_t area_before = surface_area(mesh);

    implicit_smoothing<VectorS<3>, 3>(mesh, laplacian, mesh.positions, timestep);

    rescale(mesh, center_before, area_before);
}


}