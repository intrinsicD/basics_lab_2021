//
// Created by alex on 24.09.20.
//

#ifndef BCG_GEODESICMEAN_H
#define BCG_GEODESICMEAN_H

#include <utility>
#include "bcg_rotation_matrix_exponential.h"
#include "bcg_rotation_matrix_logarithm.h"
#include "bcg_rotation_robust_elementwise_median.h"

namespace bcg {

struct geodesic_mean_so3{
    geodesic_mean_so3(bool robust, bool outlier_reject);

    geodesic_mean_so3(std::vector<bcg_scalar_t> weights, bool robust, bool outlier_reject);

    geodesic_mean_so3(const MatrixS<3, 3> &initial_estimate, bool outlier_reject);

    geodesic_mean_so3(const MatrixS<3, 3> &initial_estimate, std::vector<bcg_scalar_t> weights, bool outlier_reject);

    MatrixS<3, 3> operator()(const std::vector<MatrixS<3, 3>> &rotations, bcg_scalar_t eps = 1e-7, int max_iterations = 1000);

private:
    bool m_robust, m_outlier_reject;
    MatrixS<3, 3> m_R;
    std::vector<bcg_scalar_t> m_weights;
    std::vector<std::pair<bcg_scalar_t, VectorS<3>>> dv;
    bcg_scalar_t d_max;
};

}

#endif //BCG_GEODESICMEAN_H
