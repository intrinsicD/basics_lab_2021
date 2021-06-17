//
// Created by alex on 17.06.21.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_OBJECT_SPACE_VIEW_H
#define BCG_GRAPHICS_BCG_COMPONENT_OBJECT_SPACE_VIEW_H

#include "math/bcg_linalg.h"

namespace bcg {

struct object_space_view {
    object_space_view() : transform(Transform::Identity()) {

    }

    explicit object_space_view(const Transform &transform) : transform(transform) {

    }

    explicit object_space_view(const MatrixS<4, 4> &matrix) : transform(matrix) {

    }

    operator const Transform &() const {
        return transform;
    }

    operator const MatrixS<4, 4> &() const {
        return transform.matrix();
    }

protected:
/*    operator Transform &() {
        return transform;
    }

    operator MatrixS<4, 4> &() {
        return transform.matrix();
    }*/

    Transform transform;
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_OBJECT_SPACE_VIEW_H
