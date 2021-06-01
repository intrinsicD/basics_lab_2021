//
// Created by alex on 28.10.20.
//

#include "bcg_point_cloud_system.h"
#include "viewer/bcg_viewer_state.h"
#include "aligned_box/bcg_aligned_box.h"
#include "viewer/bcg_entity_info.h"
#include "bcg_property_map_eigen.h"
#include "renderers/picking_renderer/bcg_events_picking_renderer.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"
#include "renderers/point_rendering/hq_pbr_ufsi/bcg_hq_pbr_ufsi_renderer_events.h"
#include "point_cloud/bcg_point_cloud_graph_builder.h"
#include "point_cloud/bcg_point_cloud_vertex_pca.h"
#include "point_cloud/bcg_point_cloud_curvature_taubin.h"
#include "point_cloud/bcg_point_cloud_vertex_outlier_probability.h"
#include "point_cloud/bcg_point_cloud_kernel_density_estimation.h"
#include "point_cloud/bcg_point_cloud_vertex_quadric.h"
#include "point_cloud/bcg_point_cloud_bilateral_filter.h"

namespace bcg {

point_cloud_system::point_cloud_system(viewer_state *state) : system("point_cloud_system", state) {
    state->dispatcher.sink<event::point_cloud::setup>().connect<&point_cloud_system::on_setup>(this);
    state->dispatcher.sink<event::point_cloud::build::graph_knn>().connect<&point_cloud_system::on_build_graph_knn>(
            this);
    state->dispatcher.sink<event::point_cloud::build::graph_radius>().connect<&point_cloud_system::on_build_graph_radius>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::pca::svd>().connect<&point_cloud_system::on_vertex_pca_svd>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::pca::weighted_svd>().connect<&point_cloud_system::on_vertex_pca_weighted_svd>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::pca::eig>().connect<&point_cloud_system::on_vertex_pca_eig>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::pca::weighted_eig>().connect<&point_cloud_system::on_vertex_pca_weighted_eig>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::curvature::taubin_knn>().connect<&point_cloud_system::on_vertex_curvature_taubin_knn>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::curvature::taubin_radius>().connect<&point_cloud_system::on_vertex_curvature_taubin_radius>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::outliers::probability_knn>().connect<&point_cloud_system::on_vertex_outlier_probability_knn>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::outliers::probability_radius>().connect<&point_cloud_system::on_vertex_outlier_probability_radius>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::outliers::remove>().connect<&point_cloud_system::on_vertex_outlier_remove>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::kernel_density::knn>().connect<&point_cloud_system::on_vertex_kernel_density_estimation_knn>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::kernel_density::radius>().connect<&point_cloud_system::on_vertex_kernel_density_estimation_radius>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::quadric::point>().connect<&point_cloud_system::on_vertex_quadric_point>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::quadric::plane>().connect<&point_cloud_system::on_vertex_quadric_plane>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::quadric::probabilistic_plane_quadric_isotropic_knn>().connect<&point_cloud_system::on_vertex_quadric_probabilistic_plane_quadric_isotropic_knn>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::quadric::probabilistic_plane_quadric_isotropic_radius>().connect<&point_cloud_system::on_vertex_quadric_probabilistic_plane_quadric_isotropic_radius>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::quadric::probabilistic_plane_quadric_anisotropic_knn>().connect<&point_cloud_system::on_vertex_quadric_probabilistic_plane_quadric_anisotropic_knn>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::quadric::probabilistic_plane_quadric_anisotropic_radius>().connect<&point_cloud_system::on_vertex_quadric_probabilistic_plane_quadric_anisotropic_radius>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::quadric::collect>().connect<&point_cloud_system::on_vertex_quadric_collect>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::filter::bilateral_knn>().connect<&point_cloud_system::on_vertex_bilateral_filter_knn>(
            this);
    state->dispatcher.sink<event::point_cloud::vertex::filter::bilateral_radius>().connect<&point_cloud_system::on_vertex_bilateral_filter_radius>(
            this);
}

void point_cloud_system::on_setup(const event::point_cloud::setup &event) {
    auto &pc = state->scene.get<point_cloud>(event.id);

    state->dispatcher.trigger<event::transform::add>(event.id);

    aligned_box3 aabb(pc.positions.vector());
    Transform loading_model;
    bcg_scalar_t scale = aabb.halfextent().maxCoeff();
    loading_model.linear() = Scaling(scale, scale, scale);
    loading_model.translation() = aabb.center();
    state->scene.emplace<entity_info>(event.id, event.filename, "point_cloud", loading_model, aabb);

    Map(pc.positions) =
            (MapConst(pc.positions).rowwise() - aabb.center().transpose()) / aabb.halfextent().maxCoeff();

    state->dispatcher.trigger<event::aligned_box::add>(event.id);
    state->scene.emplace_or_replace<event::picking_renderer::enqueue>(event.id);
    state->scene.emplace_or_replace<event::points_renderer::enqueue>(event.id);
    //state->scene.emplace_or_replace<event::hq_pbr_ufsi_renderer::enqueue>(event.id);
    state->picker.entity_id = event.id;
    std::cout << pc << "\n";
}

void point_cloud_system::on_build_graph_knn(const event::point_cloud::build::graph_knn &event) {
    if (!state->scene.valid(event.id)) return;
    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);
    auto graph = point_cloud_build_knn_graph(*vertices, index, event.num_closest);
    if (graph.num_edges() != 0) {
        auto id = state->scene.create();
        state->scene.emplace<halfedge_graph>(id, graph);
        state->dispatcher.trigger<event::graph::setup>(id, "");
    }
}

void point_cloud_system::on_build_graph_radius(const event::point_cloud::build::graph_radius &event) {
    if (!state->scene.valid(event.id)) return;
    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);
    auto graph = point_cloud_build_radius_graph(*vertices, index, event.radius);
    if (graph.num_edges() != 0) {
        auto id = state->scene.create();
        state->scene.emplace<halfedge_graph>(id, graph);
        state->dispatcher.trigger<event::graph::setup>(id, "");
    }
}

void point_cloud_system::on_vertex_pca_svd(const event::point_cloud::vertex::pca::svd &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    if (event.num_closest > 0) {
        point_cloud_vertex_pcas_knn(vertices, index, event.num_closest, point_cloud_vertex_pca_least_squares_svd,
                                    event.compute_mean,
                                    state->config.parallel_grain_size);
    } else if (event.radius > 0) {
        point_cloud_vertex_pcas_radius(vertices, index, event.radius, point_cloud_vertex_pca_least_squares_svd,
                                       event.compute_mean,
                                       state->config.parallel_grain_size);
    }
}

void point_cloud_system::on_vertex_pca_weighted_svd(const event::point_cloud::vertex::pca::weighted_svd &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    if (event.num_closest > 0) {
        point_cloud_vertex_pcas_knn(vertices, index, event.num_closest,
                                    point_cloud_vertex_pca_weighted_least_squares_svd,
                                    event.compute_mean,
                                    state->config.parallel_grain_size);
    } else if (event.radius > 0) {
        point_cloud_vertex_pcas_radius(vertices, index, event.radius, point_cloud_vertex_pca_weighted_least_squares_svd,
                                       event.compute_mean,
                                       state->config.parallel_grain_size);

    }
}

void point_cloud_system::on_vertex_pca_eig(const event::point_cloud::vertex::pca::eig &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    if (event.num_closest > 0) {
        point_cloud_vertex_pcas_knn(vertices, index, event.num_closest, point_cloud_vertex_pca_least_squares_eig,
                                    event.compute_mean,
                                    state->config.parallel_grain_size);
    } else if (event.radius > 0) {
        point_cloud_vertex_pcas_radius(vertices, index, event.radius, point_cloud_vertex_pca_least_squares_eig,
                                       event.compute_mean,
                                       state->config.parallel_grain_size);

    }
}

void point_cloud_system::on_vertex_pca_weighted_eig(const event::point_cloud::vertex::pca::weighted_eig &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    if (event.num_closest > 0) {
        point_cloud_vertex_pcas_knn(vertices, index, event.num_closest,
                                    point_cloud_vertex_pca_weighted_least_squares_eig,
                                    event.compute_mean,
                                    state->config.parallel_grain_size);
    } else if (event.radius > 0) {
        point_cloud_vertex_pcas_radius(vertices, index, event.radius, point_cloud_vertex_pca_weighted_least_squares_eig,
                                       event.compute_mean,
                                       state->config.parallel_grain_size);

    }
}

void
point_cloud_system::on_vertex_curvature_taubin_knn(const event::point_cloud::vertex::curvature::taubin_knn &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_curvature_taubin(vertices, index, event.num_closest, state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_curvature_taubin_radius(
        const event::point_cloud::vertex::curvature::taubin_radius &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_curvature_taubin(vertices, index, event.radius, state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_outlier_probability_knn(
        const event::point_cloud::vertex::outliers::probability_knn &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_vertex_outlier_probability_knn(vertices, index, event.num_closest, state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_outlier_probability_radius(
        const event::point_cloud::vertex::outliers::probability_radius &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_vertex_outlier_probability_radius(vertices, index, event.radius, state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_outlier_remove(const event::point_cloud::vertex::outliers::remove &event) {
    if (!state->scene.valid(event.id)) return;

    auto *pc = state->scene.try_get<point_cloud>(event.id);
    if (!pc) return;
    point_cloud_vertex_remove_outliers(*pc, event.threshold, state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_kernel_density_estimation_knn(
        const event::point_cloud::vertex::kernel_density::knn &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_kernel_density_estimation_knn(vertices, index, event.num_closest, state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_kernel_density_estimation_radius(
        const event::point_cloud::vertex::kernel_density::radius &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_kernel_density_estimation_radius(vertices, index, event.radius, state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_quadric_point(const event::point_cloud::vertex::quadric::point &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_vertex_quadric_knn(vertices, point_quadric, index, 1, state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_quadric_plane(const event::point_cloud::vertex::quadric::plane &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_vertex_quadric_knn(vertices, plane_quadric, index, 1, state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_quadric_probabilistic_plane_quadric_isotropic_knn(
        const event::point_cloud::vertex::quadric::probabilistic_plane_quadric_isotropic_knn &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_vertex_quadric_knn(vertices, probabilistic_plane_quadric_isotropic, index, event.num_closest,
                                   state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_quadric_probabilistic_plane_quadric_isotropic_radius(
        const event::point_cloud::vertex::quadric::probabilistic_plane_quadric_isotropic_radius &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_vertex_quadric_radius(vertices, probabilistic_plane_quadric_isotropic, index, event.radius,
                                      state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_quadric_probabilistic_plane_quadric_anisotropic_knn(
        const event::point_cloud::vertex::quadric::probabilistic_plane_quadric_anisotropic_knn &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_vertex_quadric_knn(vertices, probabilistic_plane_quadric_anisotropic, index, event.num_closest,
                                   state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_quadric_probabilistic_plane_quadric_anisotropic_radius(
        const event::point_cloud::vertex::quadric::probabilistic_plane_quadric_anisotropic_radius &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_vertex_quadric_radius(vertices, probabilistic_plane_quadric_anisotropic, index, event.radius,
                                      state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_quadric_collect(const event::point_cloud::vertex::quadric::collect &event) {
    if (!state->scene.valid(event.id)) return;
    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;

    point_cloud_vertex_quadric_collect_neighbors(vertices, state->config.parallel_grain_size);
}

void
point_cloud_system::on_vertex_bilateral_filter_knn(const event::point_cloud::vertex::filter::bilateral_knn &event) {
    if (!state->scene.valid(event.id)) return;
    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    point_cloud_bilateral_filter_knn(vertices, index, {event.points_sigma, event.normals_sigma}, event.num_closest, state->config.parallel_grain_size);
}

void point_cloud_system::on_vertex_bilateral_filter_radius(
        const event::point_cloud::vertex::filter::bilateral_radius &event) {
    if (!state->scene.valid(event.id)) return;
    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;
    if (!state->scene.all_of<kdtree_property<bcg_scalar_t >>(event.id)) {
        state->dispatcher.trigger<event::spatial_index::setup_kdtree>(event.id);
    }
    auto &index = state->scene.get<kdtree_property<bcg_scalar_t >>(event.id);

    point_cloud_bilateral_filter_radius(vertices, index, {event.points_sigma, event.normals_sigma}, event.radius, state->config.parallel_grain_size);
}

}