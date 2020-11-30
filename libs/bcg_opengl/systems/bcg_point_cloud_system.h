//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_SYSTEM_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_SYSTEM_H

#include "bcg_systems.h"

namespace bcg{

struct point_cloud_system : public system{
    explicit point_cloud_system(viewer_state *state);

    void on_setup(const event::point_cloud::setup &event);

    void on_build_graph_knn(const event::point_cloud::build::graph_knn &event);

    void on_build_graph_radius(const event::point_cloud::build::graph_radius &event);

    void on_vertex_pca_svd(const event::point_cloud::vertex::pca::svd &event);

    void on_vertex_pca_weighted_svd(const event::point_cloud::vertex::pca::weighted_svd &event);

    void on_vertex_pca_eig(const event::point_cloud::vertex::pca::eig &event);

    void on_vertex_pca_weighted_eig(const event::point_cloud::vertex::pca::weighted_eig &event);
};

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_SYSTEM_H
