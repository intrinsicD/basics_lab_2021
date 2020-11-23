//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_LAPLACIAN_H
#define BCG_GRAPHICS_BCG_EVENTS_LAPLACIAN_H

#include "entt/entt.hpp"

namespace bcg::event::laplacian{
namespace smoothing{
struct explicit_euler{
    entt::entity id;
    std::string property_name;
    size_t iterations;
    bcg_scalar_t time;
    bool normalize = false, symmetric = false;
};
struct implicit_euler{
    entt::entity id;
    std::string property_name;
    size_t iterations;
    bcg_scalar_t time;
    bool normalize = false, symmetric = false;
};
struct bilaplacian{
    entt::entity id;
    std::string property_name;
    size_t iterations;
    bcg_scalar_t time;
};
struct unconstrained{
    entt::entity id;
    std::string property_name;
    size_t iterations;
    bcg_scalar_t weight;
};
}
}

#endif //BCG_GRAPHICS_BCG_EVENTS_LAPLACIAN_H
