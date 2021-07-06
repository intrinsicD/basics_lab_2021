//
// Created by alex on 21.06.21.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_TRANSFORM_H
#define BCG_GRAPHICS_BCG_COMPONENT_TRANSFORM_H

#include "math/bcg_linalg.h"

namespace bcg {

struct component_transform : public Transform{
    using Transform::Transform;

    explicit component_transform(const Transform &transformation) : Transform(transformation) {}
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_TRANSFORM_H
