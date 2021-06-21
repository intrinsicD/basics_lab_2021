//
// Created by alex on 21.06.21.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_TRANSFORM_OBJECT_SPACE_H
#define BCG_GRAPHICS_BCG_COMPONENT_TRANSFORM_OBJECT_SPACE_H

#include "math/bcg_linalg.h"

namespace bcg {

struct object_space_transform : Transform{
    using Transform::Transform;

    explicit object_space_transform(const Transform &transformation) : Transform(transformation) { }
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_TRANSFORM_OBJECT_SPACE_H
