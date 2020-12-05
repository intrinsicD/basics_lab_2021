//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROTATION_QUATERNION_MEAN_H
#define BCG_GRAPHICS_BCG_ROTATION_QUATERNION_MEAN_H

#include "math/bcg_linalg.h"

namespace bcg{

struct quaternion_mean{
    quaternion_mean();

    quaternion_mean(bool robust);

    Quaternion operator()(const std::vector<Quaternion> &rotations) const;

private:
    bool m_robust;
};

}

#endif //BCG_GRAPHICS_BCG_ROTATION_QUATERNION_MEAN_H
