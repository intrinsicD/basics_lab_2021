//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_SYSTEM_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct point_cloud_system : public system {
    explicit point_cloud_system(viewer_state *state);

    void on_setup(const event::point_cloud::setup &event);

    void on_make_overlay_point_cloud(const event::point_cloud::setup &event);

    void on_build_graph_knn(const event::point_cloud::build::graph_knn &event);

    void on_build_graph_radius(const event::point_cloud::build::graph_radius &event);

    void on_vertex_pca_svd(const event::point_cloud::vertex::pca::svd &event);

    void on_vertex_pca_weighted_svd(const event::point_cloud::vertex::pca::weighted_svd &event);

    void on_vertex_pca_eig(const event::point_cloud::vertex::pca::eig &event);

    void on_vertex_pca_weighted_eig(const event::point_cloud::vertex::pca::weighted_eig &event);

    void on_vertex_curvature_taubin_knn(const event::point_cloud::vertex::curvature::taubin_knn &event);

    void on_vertex_curvature_taubin_radius(const event::point_cloud::vertex::curvature::taubin_radius &event);

    void on_vertex_outlier_probability_knn(const event::point_cloud::vertex::outliers::probability_knn &event);

    void on_vertex_outlier_probability_radius(const event::point_cloud::vertex::outliers::probability_radius &event);

    void on_vertex_outlier_remove(const event::point_cloud::vertex::outliers::remove &event);

    void on_vertex_kernel_density_estimation_knn(const event::point_cloud::vertex::kernel_density::knn &event);

    void on_vertex_kernel_density_estimation_radius(const event::point_cloud::vertex::kernel_density::radius &event);

    void on_vertex_quadric_point(const event::point_cloud::vertex::quadric::point &event);

    void on_vertex_quadric_plane(const event::point_cloud::vertex::quadric::plane &event);

    void on_vertex_quadric_probabilistic_plane_quadric_isotropic_knn(
            const event::point_cloud::vertex::quadric::probabilistic_plane_quadric_isotropic_knn &event);

    void on_vertex_quadric_probabilistic_plane_quadric_isotropic_radius(
            const event::point_cloud::vertex::quadric::probabilistic_plane_quadric_isotropic_radius &event);

    void on_vertex_quadric_probabilistic_plane_quadric_anisotropic_knn(
            const event::point_cloud::vertex::quadric::probabilistic_plane_quadric_anisotropic_knn &event);

    void on_vertex_quadric_probabilistic_plane_quadric_anisotropic_radius(
            const event::point_cloud::vertex::quadric::probabilistic_plane_quadric_anisotropic_radius &event);

    void on_vertex_quadric_collect(const event::point_cloud::vertex::quadric::collect &event);

    void on_vertex_bilateral_filter_knn(const event::point_cloud::vertex::filter::bilateral_knn &event);

    void on_vertex_bilateral_filter_radius(const event::point_cloud::vertex::filter::bilateral_radius &event);
};

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_SYSTEM_H
