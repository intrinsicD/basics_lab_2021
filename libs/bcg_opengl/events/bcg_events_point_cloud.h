//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_POINT_CLOUD_H
#define BCG_GRAPHICS_BCG_EVENTS_POINT_CLOUD_H

#include "entt/entt.hpp"

namespace bcg {

}
namespace bcg::event::point_cloud {
struct setup {
    entt::entity id;
    std::string filename;
};
namespace build {
struct graph_knn {
    entt::entity id;
    int num_closest = 0;
};
struct graph_radius {
    entt::entity id;
    bcg_scalar_t radius = 0;
};
}

namespace vertex {
namespace filter{
struct bilateral_knn{
    entt::entity id;
    bcg_scalar_t points_sigma, normals_sigma;
    int num_closest = 12;
};
struct bilateral_radius{
    entt::entity id;
    bcg_scalar_t points_sigma, normals_sigma;
    bcg_scalar_t radius = 0.01;
};
}
namespace curvature {
struct taubin_knn {
    entt::entity id;
    int num_closest = 0;
};
struct taubin_radius {
    entt::entity id;
    bcg_scalar_t radius = 0;
};
}
namespace outliers {
struct probability_knn {
    entt::entity id;
    int num_closest = 0;
};
struct probability_radius {
    entt::entity id;
    bcg_scalar_t radius = 0;
};
struct remove {
    entt::entity id;
    bcg_scalar_t threshold = scalar_eps;
};
}
namespace quadric {
struct point {
    entt::entity id;
};
struct plane {
    entt::entity id;
};
struct probabilistic_plane_quadric_isotropic_knn {
    entt::entity id;
    int num_closest = 0;
};
struct probabilistic_plane_quadric_isotropic_radius {
    entt::entity id;
    bcg_scalar_t radius = 0;
};
struct probabilistic_plane_quadric_anisotropic_knn {
    entt::entity id;
    int num_closest = 0;
};
struct probabilistic_plane_quadric_anisotropic_radius {
    entt::entity id;
    bcg_scalar_t radius = 0;
};
struct collect{
    entt::entity id;
};

}
namespace kernel_density {
struct knn {
    entt::entity id;
    int num_closest = 0;
};
struct radius {
    entt::entity id;
    bcg_scalar_t radius = 0;
};
}
namespace pca {
struct svd {
    entt::entity id;
    bool compute_mean = true;
    int num_closest = 0;
    bcg_scalar_t radius = 0;
};
struct weighted_svd {
    entt::entity id;
    bool compute_mean = true;
    int num_closest = 0;
    bcg_scalar_t radius = 0;
};
struct eig {
    entt::entity id;
    bool compute_mean = true;
    int num_closest = 0;
    bcg_scalar_t radius = 0;
};
struct weighted_eig {
    entt::entity id;
    bool compute_mean = true;
    int num_closest = 0;
    bcg_scalar_t radius = 0;
};
}
}
}
#endif //BCG_GRAPHICS_BCG_EVENTS_POINT_CLOUD_H
