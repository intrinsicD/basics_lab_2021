//
// Created by alex on 24.02.21.
//

#include "bcg_point_cloud_normal_filtering_robust_statistics.h"
#include "bcg_point_cloud_vertex_pca.h"
#include "quadric/bcg_quadric.h"
#include "tbb/tbb.h"
#include "math/matrix/bcg_matrix_map_eigen.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "bcg_property_map_eigen.h"

namespace bcg {

std::vector<std::string> point_cloud_normal_filtering_type_names() {
    std::vector<std::string> names(static_cast<int>(PointCloudNormalFilteringType::__last__));
    names[static_cast<int>(PointCloudNormalFilteringType::averaging_knn)] = "averaging_knn";
    names[static_cast<int>(PointCloudNormalFilteringType::averaging_radius)] = "averaging_radius";
    return names;
}

void point_cloud_normal_filtering_robust_statistics_knn(vertex_container *vertices,
                                                        kdtree_property<bcg_scalar_t> &index, int num_closest,
                                                        bcg_scalar_t sigma_g, bcg_scalar_t sigma_p,
                                                        bcg_scalar_t sigma_n, bcg_scalar_t lambda,
                                                        bool use_repulsion_tangential, bool use_attraction_normal,
                                                        size_t parallel_grain_size) {
    /* auto positions = vertices->get<VectorS<3>, 3>("v_position");
     auto ref_positions = vertices->get<VectorS<3>, 3>("v_ref_position");
     auto new_positions = vertices->get_or_add<VectorS<3>, 3>("v_new_position");
     auto normals = vertices->get_or_add<VectorS<3>, 3>("v_normal");
     auto new_normals = vertices->get_or_add<VectorS<3>, 3>("v_new_normal");
     auto repulsion_force = vertices->get_or_add<VectorS<3>, 3>("v_repulsion_force");
     auto attraction_force = vertices->get_or_add<VectorS<3>, 3>("v_attraction_force");
     auto pcas = vertices->get_or_add<Pca<3>, 1>("v_pca");
     auto quadrics = vertices->get_or_add<quadric, 1>("v_quadrics");
     auto new_quadrics = vertices->get_or_add<quadric, 1>("v_new_quadrics");

     if (!ref_positions) {
         ref_positions = vertices->get_or_add<VectorS<3>, 3>("v_ref_position");
         Map(ref_positions) = MapConst(positions);

         tbb::parallel_for(
                 tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
                 [&](const tbb::blocked_range<uint32_t> &range) {
                     for (uint32_t i = range.begin(); i != range.end(); ++i) {
                         auto v = vertex_handle(i);
                         auto result = index.query_knn(ref_positions[v], num_closest);
                         std::vector<VectorS<3>> V;
                         for (size_t i = 0; i < result.indices.size(); ++i) {
                             V.push_back(ref_positions[result.indices[i]]);
                         }
                         pcas[v] = point_cloud_vertex_pca_least_squares_svd(MapConst(V), ref_positions[v], false);
                         normals[v] = pcas[v].directions.col(2);
                         quadrics[v].probabilistic_plane_quadric(ref_positions[v], normals[v], sigma_p, sigma_n);
                     }
                 }
         );
     }

     tbb::parallel_for(
             tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
             [&](const tbb::blocked_range<uint32_t> &range) {
                 for (uint32_t i = range.begin(); i != range.end(); ++i) {
                     auto v = vertex_handle(i);
                     quadric Q_total;

                     bcg_scalar_t sum_weights = 0;
                     repulsion_force[v].setZero();

                     for (size_t j = 0; j < positions.size(); ++j) {
                         VectorS<3> diff = positions[v] - positions[j];
                         bcg_scalar_t distance = diff.norm();
                         bcg_scalar_t normalizer = std::max(distance, scalar_eps);
                         bcg_scalar_t weight = std::exp(-distance * distance / (sigma_p * sigma_n / 16)) / (normalizer);
                         if (j != v.idx) {
                             sum_weights += weight;
                             repulsion_force[v] += weight * diff;
                         }
                     }
                     repulsion_force[v] /= sum_weights;
                     new_positions[v] = positions[v];

                     sum_weights = 0;
                     new_normals[v] = normals[v];
                     auto result = index.query_knn(new_positions[v], num_closest);
                     for (size_t j = 0; j < result.indices.size(); ++j) {
                         VectorS<3> diff = new_positions[v] - ref_positions[result.indices[j]];
                         bcg_scalar_t distance = diff.norm();
                         bcg_scalar_t normalizer = std::max(distance, scalar_eps);
                         bcg_scalar_t weight = std::exp(-distance * distance / (sigma_p * sigma_n / 16)) / (normalizer);
                         if (result.indices[j] != v.idx) {
                             sum_weights += weight;
                             Q_total += quadrics[result.indices[j]] * weight;
                         }
                     }
                     Eigen::JacobiSVD<MatrixS<3, 3>> svd(Q_total.A());
                     new_normals[v] = svd.matrixU().col(0);
                     Q_total /= sum_weights;
                     repulsion_force[v] = repulsion_force[v] - Q_total.A() * repulsion_force[v];
                     new_positions[v] += lambda * repulsion_force[v];

                     attraction_force[v] = Q_total.A() * (Q_total.minimizer() - new_positions[v]);
                     new_positions[v] += attraction_force[v];
                     new_quadrics[v] = Q_total;
                 }
             }
     );
     Map(positions) = MapConst(new_positions);
     Map(normals) = MapConst(new_normals);
     Map(quadrics) = MapConst(new_quadrics);
     positions.set_dirty();
     new_positions.set_dirty();
     normals.set_dirty();
     repulsion_force.set_dirty();
     attraction_force.set_dirty();
     index.build(positions);
     vertices->remove(pcas);*/

    auto v_normals_filtered = vertices->get_or_add<VectorS<3>, 3>("v_normal_filtered", VectorS<3>::Zero());
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto normals = vertices->get<VectorS<3>, 3>("v_normal");

    if (!normals) {
        normals = vertices->get_or_add<VectorS<3>, 3>("v_normal");

        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        auto result = index.query_knn(positions[v], num_closest);
                        std::vector<VectorS<3>> V;
                        for (size_t i = 0; i < result.indices.size(); ++i) {
                            V.push_back(positions[result.indices[i]]);
                        }
                        auto pca = point_cloud_vertex_pca_least_squares_svd(MapConst(V), positions[v], false);
                        normals[v] = pca.directions.col(2);
                    }
                }
        );
    }

    auto repulsion_force = vertices->get_or_add<VectorS<3>, 3>("v_repulsion_force");
    auto repulsion_force_tangential = vertices->get_or_add<VectorS<3>, 3>("v_repulsion_force_tangential");
    auto attraction_force = vertices->get_or_add<VectorS<3>, 3>("v_attraction_force");
    auto attraction_force_normal = vertices->get_or_add<VectorS<3>, 3>("v_attraction_force_normal");

    bcg_scalar_t sigma_g_squared = sigma_g * sigma_g;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    VectorS<3> n_i = normals[v];
                    VectorS<3> c_i = positions[v];
                    v_normals_filtered[v] = n_i;
                    auto result = index.query_knn(positions[v], num_closest);
                    for (const auto &idx : result.indices) {
                        if (idx == v.idx) continue;
                        VectorS<3> n_j = normals[idx];
                        bcg_scalar_t sign = n_i.dot(n_j) > 0 ? 1 : -1;
                        bcg_scalar_t x = acos(1.0 - (n_i - sign * n_j).squaredNorm() / 2.0) /
                                         (c_i - positions[idx]).norm();
                        bcg_scalar_t weight = std::exp(-x * x / sigma_g_squared);
                        v_normals_filtered[v] += weight * sign * n_j;
                    }
                    v_normals_filtered[v].normalize();
                }
            }
    );

    auto new_positions = vertices->get_or_add<VectorS<3>, 3>("v_new_position");
    if (true) {
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);

                        quadric Q_total;
                        //Q_total = quadric::probabilistic_plane_quadric(positions[v], v_normals_filtered[v], sigma_p, sigma_n);
                        auto result = index.query_knn(positions[v], num_closest);
                        bcg_scalar_t radius = MapConst(result.distances).maxCoeff() * 10;
                        bcg_scalar_t sum_weights = 0;
                        for (const auto &idx : result.indices) {
                            if (idx != v.idx) {
                                VectorS<3> diff = positions[v] - positions[idx];
                                bcg_scalar_t distance = diff.norm();
                                bcg_scalar_t normalizer = std::max(distance, scalar_eps);
                                bcg_scalar_t weight =
                                        std::exp(-distance * distance / (radius * radius / 16)) / normalizer;
                                quadric Q = quadric::probabilistic_plane_quadric(positions[idx], v_normals_filtered[idx], sigma_p,
                                                              sigma_n);
                                Q_total += Q * weight;
                                sum_weights += weight;
                            }
                        }
                        Q_total /= sum_weights;

                        sum_weights = 0;
                        VectorS<3> delta = VectorS<3>::Zero();
                        for (const auto &idx : result.indices) {
                            if (idx != v.idx) {
                                VectorS<3> diff = positions[v] - positions[idx];
                                bcg_scalar_t distance = diff.norm();
                                bcg_scalar_t normalizer = std::max(distance, scalar_eps);
                                bcg_scalar_t weight =
                                        std::exp(-distance * distance / (radius * radius / 16)) / normalizer;
                                sum_weights += weight;
                                delta += weight * diff;
                            }
                        }
                        delta /= sum_weights;

                        repulsion_force[v] = delta * lambda;
                        repulsion_force_tangential[v] = repulsion_force[v] -
                                                        v_normals_filtered[v].dot(repulsion_force[v]) *
                                                        v_normals_filtered[v];
/*                        repulsion_force_tangential[v] = repulsion_force[v] -
                                Q_total.A() * repulsion_force[v];*/
                        bcg_scalar_t rep_len = repulsion_force[v].norm();
                        bcg_scalar_t rep_tan_len = repulsion_force_tangential[v].norm();
                        if (rep_tan_len > rep_len) {
                            repulsion_force_tangential[v] *= rep_len / rep_tan_len;
                        }

                        new_positions[v] = positions[v];
                        if(use_repulsion_tangential){
                            new_positions[v] += repulsion_force_tangential[v];
                        }else{
                            new_positions[v] += repulsion_force[v];
                        }

                        attraction_force[v] = Q_total.minimizer() - positions[v];
                        attraction_force_normal[v] =
                                v_normals_filtered[v].dot(attraction_force[v]) * v_normals_filtered[v];
/*                        attraction_force_normal[v] = Q_total.A() * attraction_force[v];*/

                        bcg_scalar_t attr_len = attraction_force[v].norm();
                        bcg_scalar_t attr_normal_len = attraction_force_normal[v].norm();
                        if (attr_normal_len > attr_len) {
                            attraction_force_normal[v] *= attr_len / attr_normal_len;
                        }

                        if(use_attraction_normal){
                            new_positions[v] += attraction_force_normal[v];
                        }else{
                            new_positions[v] += attraction_force[v];
                        }

                        if (false) {
                            VectorS<3> delta = Q_total.minimizer() - positions[v];
                            positions[v] += Q_total.A() * delta;
                        }
                        if(false) {
/*                            repulsion_force[v] = lambda * (delta -  v_normals_filtered[v].dot(delta) * v_normals_filtered[v]);
                            new_positions[v] = positions[v] + repulsion_force[v];
                            attraction_force[v] = v_normals_filtered[v].dot(Q_total.minimizer() - new_positions[v]) * v_normals_filtered[v];
                            new_positions[v] += attraction_force[v];*/
                            repulsion_force[v] =
                                    lambda * (delta - v_normals_filtered[v].dot(delta) * v_normals_filtered[v]);
                            // new_positions[v] = positions[v] + repulsion_force[v];
                            new_positions[v] = positions[v];
                            attraction_force[v] = v_normals_filtered[v].dot(Q_total.minimizer() - new_positions[v]) *
                                                  v_normals_filtered[v];
                            new_positions[v] += attraction_force[v]; //+ repulsion_force[v];
                            //new_positions[v] = positions[v] + attraction_force[v] + repulsion_force[v];
                        }

                        normals[v] = v_normals_filtered[v];
                    }
                }
        );
    } else {
        /*auto new_positions = mesh.vertices.get_or_add<VectorS<3>, 3>("v_position_new");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);

                        VectorS<3> delta = VectorS<3>::Zero();
                        bcg_scalar_t sum_weights = 0;
                        for (const auto &fv : mesh.get_faces(v)) {
                            VectorS<3> PC = face_center(mesh, fv) - positions[v];
                            bcg_scalar_t weight = face_area(mesh, fv);
                            sum_weights += weight;
                            delta += weight * PC.dot(f_normals_filtered[fv]) * f_normals_filtered[fv];
                        }
                        delta /= sum_weights;
                        new_positions[v] = positions[v] + delta;
                    }
                }
        );
        Map(positions) = MapConst(new_positions);*/
    }
    Map(positions) = MapConst(new_positions);
    index.build(positions);
    positions.set_dirty();
    normals.set_dirty();
}

void point_cloud_normal_filtering_robust_statistics_radius(vertex_container *vertices,
                                                           kdtree_property<bcg_scalar_t> &index,
                                                           bcg_scalar_t radius, bcg_scalar_t sigma_p,
                                                           bcg_scalar_t sigma_n, size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto normals = vertices->get_or_add<VectorS<3>, 3>("v_normal");
    auto pcas = vertices->get_or_add<Pca<3>, 1>("v_pca");
    auto quadrics = vertices->get_or_add<quadric, 1>("v_quadrics");
    auto neighbors = vertices->get_or_add<neighbors_query, 1>("v_neighbors");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    neighbors[v] = index.query_radius(positions[v], radius);
                    std::vector<VectorS<3>> V;
                    for (size_t i = 0; i < neighbors[v].indices.size(); ++i) {
                        V.push_back(positions[neighbors[v].indices[i]]);
                    }
                    pcas[v] = point_cloud_vertex_pca_least_squares_svd(MapConst(V), positions[v], false);
                    normals[v] = pcas[v].directions.col(2);
                    quadrics[v] = quadric::probabilistic_plane_quadric(positions[v], normals[v], sigma_p, sigma_n);
                }
            }
    );

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadric Q_total = quadrics[v];
                    for (size_t i = 0; i < neighbors[v].indices.size(); ++i) {
                        Q_total += quadrics[neighbors[v].indices[i]];
                    }
                    positions[v] = Q_total.minimizer();
                }
            }
    );
    positions.set_dirty();
    normals.set_dirty();
    index.build(positions);
    vertices->remove(pcas);
    vertices->remove(neighbors);
}


}