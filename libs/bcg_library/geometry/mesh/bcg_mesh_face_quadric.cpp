//
// Created by alex on 19.04.21.
//

#include "bcg_mesh_face_quadric.h"
#include "bcg_mesh_face_centers.h"
#include "bcg_mesh_face_areas.h"
#include "bcg_mesh_face_normals.h"
#include "bcg_property_map_eigen.h"
#include "tbb/tbb.h"

namespace bcg {


std::vector<std::string> mesh_face_quadric_type_names() {
    std::vector<std::string> names(static_cast<int>(MeshFaceQuadricType::__last__));
    names[static_cast<int>(MeshFaceQuadricType::point)] = "point";
    names[static_cast<int>(MeshFaceQuadricType::plane)] = "plane";
    names[static_cast<int>(MeshFaceQuadricType::triangle)] = "triangle";
    names[static_cast<int>(MeshFaceQuadricType::global_isotropic_probabilistic_plane)] = "global_isotropic_probabilistic_plane";
    names[static_cast<int>(MeshFaceQuadricType::local_isotropic_probabilistic_plane)] = "local_isotropic_probabilistic_plane";
    names[static_cast<int>(MeshFaceQuadricType::global_isotropic_pobabilistic_triangle)] = "global_isotropic_pobabilistic_triangle";
    names[static_cast<int>(MeshFaceQuadricType::local_isotropic_pobabilistic_triangle)] = "local_isotropic_pobabilistic_triangle";
    names[static_cast<int>(MeshFaceQuadricType::global_anisotropic_probabilistic_plane)] = "global_anisotropic_probabilistic_plane";
    names[static_cast<int>(MeshFaceQuadricType::local_anisotropic_probabilistic_plane)] = "local_anisotropic_probabilistic_plane";
    names[static_cast<int>(MeshFaceQuadricType::global_anisotropic_pobabilistic_triangle)] = "global_anisotropic_pobabilistic_triangle";
    names[static_cast<int>(MeshFaceQuadricType::local_anisotropic_pobabilistic_triangle)] = "local_anisotropic_pobabilistic_triangle";
    return names;
}

void mesh_face_point_quadric(halfedge_mesh &mesh, size_t parallel_grain_size) {
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    quadrics[f] = quadric::point_quadric(face_center(mesh, f));
                }
            }
    );
}

void mesh_face_plane_quadric(halfedge_mesh &mesh, size_t parallel_grain_size) {
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    quadrics[f] = quadric::plane_quadric(face_center(mesh, f), face_normal(mesh, f));
                }
            }
    );
}

void
mesh_face_triangle_quadric(halfedge_mesh &mesh, size_t parallel_grain_size) {
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    std::vector<VectorS<3>> V;
                    for (const auto vf : mesh.get_vertices(f)) {
                        V.push_back(mesh.positions[vf]);
                    }
                    quadrics[f] = quadric::triangle_quadric(V[0], V[1], V[2]);
                }
            }
    );
}

//global isotropic sigmas
void mesh_face_probabilistic_plane_quadric_isotropic(halfedge_mesh &mesh, bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                                     size_t parallel_grain_size) {
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    if (mesh.is_boundary(f)) {
                        VectorS<3> boundary_mean = VectorS<3>::Zero();
                        bcg_scalar_t count = 0.0;
                        for (const auto &vf : mesh.get_vertices(f)) {
                            boundary_mean += mesh.positions[vf];
                            count += 1.0;
                        }
                        boundary_mean /= count;
                        quadrics[f] = quadric::probabilistic_plane_quadric(boundary_mean, face_normal(mesh, f), sigma_p,
                                                                           sigma_n);
                    } else {
                        quadrics[f] = quadric::probabilistic_plane_quadric(face_center(mesh, f), face_normal(mesh, f),
                                                                           sigma_p, sigma_n);
                    }
                }
            }
    );
}

//global anisotropic sigmas
void mesh_face_probabilistic_plane_quadric_anisotropic(halfedge_mesh &mesh,
                                                       const MatrixS<3, 3> &sigma_p, const MatrixS<3, 3> &sigma_n,
                                                       size_t parallel_grain_size) {
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    if (mesh.is_boundary(f)) {
                        VectorS<3> boundary_mean = VectorS<3>::Zero();
                        bcg_scalar_t count = 0.0;
                        for (const auto &vf : mesh.get_vertices(f)) {
                            boundary_mean += mesh.positions[vf];
                            count += 1.0;
                        }
                        boundary_mean /= count;
                        quadrics[f] = quadric::probabilistic_plane_quadric(boundary_mean, face_normal(mesh, f), sigma_p,
                                                                           sigma_n);
                    } else {
                        quadrics[f] = quadric::probabilistic_plane_quadric(face_center(mesh, f), face_normal(mesh, f),
                                                                           sigma_p, sigma_n);
                    }
                }
            }
    );
}

//global isotropic sigmas
void mesh_face_probabilistic_triangle_quadric_isotropic(halfedge_mesh &mesh,
                                                        bcg_scalar_t sigma_p, size_t parallel_grain_size) {
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    std::vector<VectorS<3>> V;
                    for (const auto &vf : mesh.get_vertices(f)) {
                        V.push_back(mesh.positions[vf]);
                    }
                    quadrics[f] = quadric::probabilistic_triangle_quadric(V[0], V[1], V[2], sigma_p);
                }
            }
    );
}

//global anisotropic sigmas
void mesh_face_probabilistic_triangle_quadric_anisotropic(halfedge_mesh &mesh,
                                                          const MatrixS<3, 3> &sigma_p, const MatrixS<3, 3> &sigma_q,
                                                          const MatrixS<3, 3> &sigma_r,
                                                          size_t parallel_grain_size) {
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    std::vector<VectorS<3>> V;
                    for (const auto &vf : mesh.get_vertices(f)) {
                        V.push_back(mesh.positions[vf]);
                    }
                    quadrics[f] = quadric::probabilistic_triangle_quadric(V[0], V[1], V[2], sigma_p, sigma_q, sigma_r);
                }
            }
    );
}

//local isotropic sigmas
void mesh_face_probabilistic_plane_quadric_isotropic(halfedge_mesh &mesh,
                                                     property<bcg_scalar_t, 1> sigma_p,
                                                     property<bcg_scalar_t, 1> sigma_n,
                                                     size_t parallel_grain_size) {
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    if (!sigma_p) {
        sigma_p = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_position_isotropic_variance");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto f = face_handle(i);
                        MatrixS<3, 3> P = MatrixS<3, 3>::Zero();
                        bcg_scalar_t sum_weights = 0;
                        for (const auto &vf : mesh.get_vertices(f)) {
                            VectorS<3> diff = mesh.positions[vf] - face_center(mesh, f);
                            P += diff * diff.transpose();
                            sum_weights += 1.0;
                        }
                        P /= sum_weights;
                        Eigen::JacobiSVD<MatrixS<3, 3>> svd(P, Eigen::ComputeFullV);
                        sigma_p[f] = svd.singularValues().mean();
                    }
                }
        );
        sigma_p.set_dirty();
        std::cout << "estimated sigma_p from face positions!\n";
    }
    if (!sigma_n) {
        auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
        sigma_n = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_normal_isotropic_variance");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto f = face_handle(i);
                        VectorS<3> fnormal = face_normal(mesh, f);
                        MatrixS<3, 3>N = MatrixS<3, 3>::Zero();
                        bcg_scalar_t sum_weights = 1;
                        for (const auto &vf : mesh.get_vertices(f)) {
                            VectorS<3> diff = normals[vf] - fnormal;
                            N += diff * diff.transpose();
                            sum_weights += 1.0;
                        }
                        N /= sum_weights;
                        Eigen::JacobiSVD<MatrixS<3, 3>> svd(N, Eigen::ComputeFullV);
                        sigma_n[f] = svd.singularValues().mean();
                    }
                }
        );
        sigma_n.set_dirty();
        std::cout << "estimated sigma_n from neighborhood normals!\n";
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    if (mesh.is_boundary(f)) {
                        VectorS<3> boundary_mean = VectorS<3>::Zero();
                        bcg_scalar_t count = 0.0;
                        for (const auto &vf : mesh.get_vertices(f)) {
                            boundary_mean += mesh.positions[vf];
                            count += 1.0;
                        }
                        boundary_mean /= count;
                        quadrics[f] = quadric::probabilistic_plane_quadric(boundary_mean, face_normal(mesh, f),
                                                                           sigma_p[f], sigma_n[f]);
                    } else {
                        quadrics[f] = quadric::probabilistic_plane_quadric(face_center(mesh, f), face_normal(mesh, f),
                                                                           sigma_p[f], sigma_n[f]);
                    }
                }
            }
    );
}

//local isotropic sigmas
void mesh_face_probabilistic_triangle_quadric_isotropic(halfedge_mesh &mesh,
                                                        property<bcg_scalar_t, 1> sigma_p,
                                                        size_t parallel_grain_size) {
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    if (!sigma_p) {
        sigma_p = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_position_isotropic_variance");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto f = face_handle(i);
                        MatrixS<3, 3> P = MatrixS<3, 3>::Zero();
                        bcg_scalar_t sum_weights = 0;
                        for (const auto &vf : mesh.get_vertices(f)) {
                            VectorS<3> diff = mesh.positions[vf] - face_center(mesh, f);
                            P += diff * diff.transpose();
                            sum_weights += 1.0;
                        }
                        P /= sum_weights;
                        Eigen::JacobiSVD<MatrixS<3, 3>> svd(P, Eigen::ComputeFullV);
                        sigma_p[f] = svd.singularValues().mean();
                    }
                }
        );
        sigma_p.set_dirty();
        std::cout << "estimated sigma_p from face positions!\n";
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    std::vector<VectorS<3>> V;
                    for (const auto &vf : mesh.get_vertices(f)) {
                        V.push_back(mesh.positions[vf]);
                    }
                    quadrics[f] = quadric::probabilistic_triangle_quadric(V[0], V[1], V[2], sigma_p[f]);
                }
            }
    );
}

//local anisotropic sigmas
void mesh_face_probabilistic_plane_quadric_anisotropic(halfedge_mesh &mesh,
                                                       property<MatrixS<3, 3>, 1> sigma_p,
                                                       property<MatrixS<3, 3>, 1> sigma_n,
                                                       size_t parallel_grain_size) {
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    if (!sigma_p) {
        sigma_p = mesh.faces.get_or_add<MatrixS<3, 3>, 1>("f_position_anisotropic_variance");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto f = face_handle(i);
                        MatrixS<3, 3> P = MatrixS<3, 3>::Zero();
                        bcg_scalar_t sum_weights = 0;
                        for (const auto &vf : mesh.get_vertices(f)) {
                            VectorS<3> diff = mesh.positions[vf] - face_center(mesh, f);
                            P += diff * diff.transpose();
                            sum_weights += 1.0;
                        }
                        sigma_p[f] = P / sum_weights;
                    }
                }
        );
        sigma_p.set_dirty();
        std::cout << "estimated sigma_p from face positions!\n";
    }
    if (!sigma_n) {
        auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
        sigma_n = mesh.faces.get_or_add<MatrixS<3, 3>, 1>("f_normal_anisotropic_variance");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto f = face_handle(i);
                        VectorS<3> fnormal = face_normal(mesh, f);
                        MatrixS<3, 3>N = MatrixS<3, 3>::Zero();
                        bcg_scalar_t sum_weights = 1;
                        for (const auto &vf : mesh.get_vertices(f)) {
                            VectorS<3> diff = normals[vf] - fnormal;
                            N += diff * diff.transpose();
                            sum_weights += 1.0;
                        }
                        sigma_n[f] = N / sum_weights;
                    }
                }
        );
        sigma_n.set_dirty();
        std::cout << "estimated sigma_n from neighborhood normals!\n";
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    if (mesh.is_boundary(f)) {
                        VectorS<3> boundary_mean = VectorS<3>::Zero();
                        bcg_scalar_t count = 0.0;
                        for (const auto &vf : mesh.get_vertices(f)) {
                            boundary_mean += mesh.positions[vf];
                            count += 1.0;
                        }
                        boundary_mean /= count;
                        quadrics[f] = quadric::probabilistic_plane_quadric(boundary_mean, face_normal(mesh, f),
                                                                           sigma_p[f], sigma_n[f]);
                    } else {
                        quadrics[f] = quadric::probabilistic_plane_quadric(face_center(mesh, f), face_normal(mesh, f),
                                                                           sigma_p[f], sigma_n[f]);
                    }
                }
            }
    );
}

//local anisotropic sigmas
void mesh_face_probabilistic_triangle_quadric_anisotropic(halfedge_mesh &mesh,
                                                          property<MatrixS<3, 3>, 1> sigma_p,
                                                          size_t parallel_grain_size) {
    auto quadrics = mesh.faces.get_or_add<quadric, 1>("f_quadric");
    if (!sigma_p) {
        sigma_p = mesh.faces.get_or_add<MatrixS<3, 3>, 1>("f_position_anisotropic_variance");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto f = face_handle(i);
                        MatrixS<3, 3> P = MatrixS<3, 3>::Zero();
                        bcg_scalar_t sum_weights = 0;
                        for (const auto &vf : mesh.get_vertices(f)) {
                            VectorS<3> diff = mesh.positions[vf] - face_center(mesh, f);
                            P += diff * diff.transpose();
                            sum_weights += 1.0;
                        }
                        sigma_p[f] = P / sum_weights;
                    }
                }
        );
        sigma_p.set_dirty();
        std::cout << "estimated sigma_p from face positions!\n";
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    std::vector<VectorS<3>> V;
                    std::vector<vertex_handle> Vv;
                    for (const auto &vf : mesh.get_vertices(f)) {
                        V.push_back(mesh.positions[vf]);
                        Vv.push_back(vf);
                    }
                    quadrics[f] = quadric::probabilistic_triangle_quadric(V[0], V[1], V[2], sigma_p[Vv[0]],
                                                                          sigma_p[Vv[1]], sigma_p[Vv[2]]);
                }
            }
    );
}

//----------------------------------------------------------------------------------------------------------------------

void mesh_face_quadric_error(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                             size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute face quadrics first!" << "\n";
        return;
    }
    auto quadric_error = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_quadric_error");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    quadric_error[f] = quadrics[f](face_center(mesh, f));
                }
            }
    );
    quadric_error.set_dirty();
}

void mesh_face_quadric_minimizer(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                                 size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute face quadrics first!" << "\n";
        return;
    }
    auto quadric_minimizer = mesh.faces.get_or_add<VectorS<3>, 3>("f_quadric_minimizer");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    quadric_minimizer[f] = quadrics[f].minimizer();
                }
            }
    );
    quadric_minimizer.set_dirty();
}

void mesh_face_quadric_neighbors_sum(halfedge_mesh &mesh, property<quadric, 1> quadrics, Params params,
                                     size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute face quadrics first!" << "\n";
        return;
    }
    auto quadric_last = mesh.faces.get_or_add<quadric, 1>("f_quadric_last");
    Map(quadric_last) = MapConst(quadrics);
    auto quadric_sum = mesh.faces.get_or_add<quadric, 1>("f_quadric_sum");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    bcg_scalar_t weight = 1.0;
                    if (params.use_area_weighting) {
                        weight *= face_area(mesh, f);
                    }
                    VectorS<3> n_i = face_normal(mesh, f);
                    VectorS<3> c_i = face_center(mesh, f);
                    quadric_sum[f] = quadric_last[f] * weight;
                    for (const auto &hf : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(hf);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            weight = 1.0;
                            if (params.use_area_weighting) {
                                weight *= face_area(mesh, ff);
                            }
                            if (params.use_normal_weighting) {
                                weight *= std::exp(-(face_normal(mesh, ff) - n_i).norm());
                            }
                            if (params.use_distance_weighting) {
                                weight *= std::exp(-(face_center(mesh, ff) - c_i).norm());
                            }
                            quadric_sum[f] += quadric_last[f] * weight;
                        }
                    }
                }
            }
    );
    quadric_sum.set_dirty();
}

void mesh_face_quadric_neighbors_avg(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                                     Params params,
                                     size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute face quadrics first!" << "\n";
        return;
    }

    auto quadric_last = mesh.faces.get_or_add<quadric, 1>("f_quadric_last");
    Map(quadric_last) = MapConst(quadrics);
    auto quadric_avg = mesh.faces.get_or_add<quadric, 1>("f_quadric_avg");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    bcg_scalar_t weight = 1.0;
                    if (params.use_area_weighting) {
                        weight *= face_area(mesh, f);
                    }
                    VectorS<3> n_i = face_normal(mesh, f);
                    VectorS<3> c_i = face_center(mesh, f);
                    bcg_scalar_t sum_weight = weight;
                    quadric_avg[f] = quadric_last[f] * weight;
                    for (const auto &hf : mesh.get_halfedges(f)) {
                        auto oh = mesh.get_opposite(hf);
                        if (!mesh.is_boundary(oh)) {
                            auto ff = mesh.get_face(oh);
                            weight = 1.0;
                            if (params.use_area_weighting) {
                                weight *= face_area(mesh, ff);
                            }
                            if (params.use_normal_weighting) {
                                weight *= std::exp(-(face_normal(mesh, ff) - n_i).norm());
                            }
                            if (params.use_distance_weighting) {
                                weight *= std::exp(-(face_center(mesh, ff) - c_i).norm());
                            }
                            quadric_avg[f] += quadric_last[f] * weight;
                            sum_weight += weight;
                        }
                    }
                    quadric_avg[f] /= sum_weight;
                }
            }
    );
    quadric_avg.set_dirty();
}

void mesh_face_quadric_neighbors_sum_to_vertices(halfedge_mesh &mesh, property<quadric, 1> quadrics, Params params,
                                                 size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute face quadrics first!" << "\n";
        return;
    }
    if(quadrics.name() == "v_quadric_sum"){
        std::cout << "Face Quadrics name should not be equal to: v_quadric_sum" << "\n";
        return;
    }
    auto quadric_sum = mesh.vertices.get_or_add<quadric, 1>("v_quadric_sum");
    auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadric_sum[v] = quadric();
                    for (const auto fv : mesh.get_faces(v)) {
                        bcg_scalar_t weight = 1.0;
                        if (params.use_area_weighting) {
                            weight *= face_area(mesh, fv);
                        }
                        if (params.use_normal_weighting) {
                            weight *= std::exp(-(face_normal(mesh, fv) - normals[v]).norm());
                        }
                        if (params.use_distance_weighting) {
                            weight *= std::exp(-(face_center(mesh, fv) - mesh.positions[v]).norm());
                        }
                        quadric_sum[v] += quadrics[fv] * weight;
                    }

                }
            }
    );
    quadric_sum.set_dirty();
}

void mesh_face_quadric_neighbors_avg_to_vertices(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                                                 Params params,
                                                 size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute face quadrics first!" << "\n";
        return;
    }
    if(quadrics.name() == "v_quadric_sum"){
        std::cout << "Face Quadrics name should not be equal to: v_quadric_avg" << "\n";
        return;
    }
    auto quadric_avg= mesh.vertices.get_or_add<quadric, 1>("v_quadric_avg");
    auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadric_avg[v] = quadric();
                    bcg_scalar_t sum_weights = 0.0;
                    for (const auto fv : mesh.get_faces(v)) {
                        bcg_scalar_t weight = 1.0;
                        if (params.use_area_weighting) {
                            weight *= face_area(mesh, fv);
                        }
                        if (params.use_normal_weighting) {
                            weight *= std::exp(-(face_normal(mesh, fv) - normals[v]).norm());
                        }
                        if (params.use_distance_weighting) {
                            weight *= std::exp(-(face_center(mesh, fv) - mesh.positions[v]).norm());
                        }
                        quadric_avg[v] += quadrics[fv] * weight;
                        sum_weights += weight;
                    }
                    quadric_avg[v] /= sum_weights;
                }
            }
    );
    quadric_avg.set_dirty();
}

void mesh_face_quadric_extract_normals(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                                       size_t parallel_grain_size) {
    if (!quadrics) {
        std::cout << "Please compute face quadrics first!" << "\n";
        return;
    }
    auto quadric_normals = mesh.faces.get_or_add<VectorS<3>, 3>("f_quadric_normal");
    auto quadric_normals_dir1 = mesh.faces.get_or_add<VectorS<3>, 3>("f_quadric_normals_dir1");
    auto quadric_normals_dir2 = mesh.faces.get_or_add<VectorS<3>, 3>("f_quadric_normals_dir2");
    auto quadric_normals_loadings = mesh.faces.get_or_add<VectorS<3>, 3>("f_quadric_normals_loading");
    auto quadric_normals_loadings_0 = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_quadric_normals_loading_0");
    auto quadric_normals_loadings_1 = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_quadric_normals_loading_1");
    auto quadric_normals_loadings_2 = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_quadric_normals_loading_2");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    Eigen::JacobiSVD<MatrixS<3, 3>> svd(quadrics[f].A(), Eigen::ComputeFullV);
                    quadric_normals[f] = svd.matrixV().col(0);
                    quadric_normals_dir1[f] = svd.matrixV().col(1);
                    quadric_normals_dir2[f] = svd.matrixV().col(2);
                    quadric_normals_loadings[f] = svd.singularValues();
                    quadric_normals_loadings_0[f] = svd.singularValues()[0];
                    quadric_normals_loadings_1[f] = svd.singularValues()[1];
                    quadric_normals_loadings_2[f] = svd.singularValues()[2];
                }
            }
    );
    quadric_normals.set_dirty();
    quadric_normals_dir1.set_dirty();
    quadric_normals_dir2.set_dirty();
    quadric_normals_loadings.set_dirty();
    quadric_normals_loadings_0.set_dirty();
    quadric_normals_loadings_1.set_dirty();
    quadric_normals_loadings_2.set_dirty();
}

}