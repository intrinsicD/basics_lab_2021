//
// Created by alex on 21.06.21.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_TRANSFORM_WORLD_SPACE_H
#define BCG_GRAPHICS_BCG_EVENTS_TRANSFORM_WORLD_SPACE_H

#include "entt/entt.hpp"
#include "math/bcg_linalg.h"

namespace bcg::event::transform::world_space{

struct init{
    entt::entity id;
};

struct remove{
    entt::entity id;
};

struct set{
    entt::entity id;
    Transform transformation;
};

struct set_identity{
    entt::entity id;
};

struct pre_transform{
    entt::entity id;
    Transform transform;
};

struct pre_translate{
    entt::entity id;
    VectorS<3> translation;
};

struct pre_scale{
    entt::entity id;
    VectorS<3> scaling;
};

struct pre_uniform_scale{
    entt::entity id;
    bcg_scalar_t scaling;
};

struct pre_rotate{
    entt::entity id;
    VectorS<3> angle_axis;
};

struct post_transform{
    entt::entity id;
    Transform transform;
};

struct post_translate{
    entt::entity id;
    VectorS<3> translation;
};

struct post_scale{
    entt::entity id;
    VectorS<3> scaling;
};

struct post_uniform_scale{
    entt::entity id;
    bcg_scalar_t scaling;
};

struct post_rotate{
    entt::entity id;
    VectorS<3> angle_axis;
};

struct render_gui{
    entt::entity id;
};


}

#endif //BCG_GRAPHICS_BCG_EVENTS_TRANSFORM_WORLD_SPACE_H
