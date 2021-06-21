//
// Created by alex on 21.06.21.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_TRANSFORM_WORLD_SPACE_H
#define BCG_GRAPHICS_BCG_COMPONENT_TRANSFORM_WORLD_SPACE_H

#include "math/bcg_linalg.h"

namespace bcg {

struct world_space_transform : public Transform{
    using Transform::Transform;

    explicit world_space_transform(const Transform &transformation) : Transform(transformation) {}
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_TRANSFORM_WORLD_SPACE_H
