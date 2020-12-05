//
// Created by alex on 24.09.20.
//

#ifndef BCG_QUATERNIONMEDIAN_H
#define BCG_QUATERNIONMEDIAN_H


#include "math/bcg_linalg.h"
#include "bcg_rotation_robust_elementwise_median.h"
#include "bcg_rotations_convert.h"

namespace bcg{

struct quaternion_median{
    quaternion_median(bool robust, bool outlier_reject);

    quaternion_median(std::vector<bcg_scalar_t> weights, bool robust, bool outlier_reject);

    quaternion_median(const Quaternion &initial_estimate, bool outlier_reject);

    quaternion_median(const Quaternion &initial_estimate, std::vector<bcg_scalar_t> weights, bool outlier_reject);

    Quaternion operator()(const std::vector<Quaternion> &rotations, bcg_scalar_t eps = scalar_eps, int max_iterations = 100);
private:
    bool m_robust, m_outlier_reject;
    Quaternion m_s;
    std::vector<bcg_scalar_t> m_weights;
    std::vector<std::pair<bcg_scalar_t, Rotation>> dv;
    bcg_scalar_t d_max;
};

}

#endif //BCG_QUATERNIONMEDIAN_H
