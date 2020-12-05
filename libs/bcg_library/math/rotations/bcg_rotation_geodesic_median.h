//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROTATION_GEODESIC_MEDIAN_H
#define BCG_GRAPHICS_BCG_ROTATION_GEODESIC_MEDIAN_H

#include "math/bcg_linalg.h"

namespace bcg {


struct geodesic_median_so3 {
    geodesic_median_so3(bool robust, bool outlier_reject);

    geodesic_median_so3(std::vector<bcg_scalar_t> weights, bool robust, bool outlier_reject);

    geodesic_median_so3(const MatrixS<3, 3> &initial_estimate, bool outlier_reject);

    geodesic_median_so3(const MatrixS<3, 3> &initial_estimate, std::vector<bcg_scalar_t> weights, bool outlier_reject);

    MatrixS<3, 3> operator()(const std::vector<MatrixS<3, 3>> &rotations,
                             bcg_scalar_t eps = scalar_eps,
                             int max_iterations = 100);

private:
    bool m_robust, m_outlier_reject;
    MatrixS<3, 3> m_R;
    std::vector<bcg_scalar_t> m_weights;
    std::vector<std::pair<bcg_scalar_t, VectorS<3>>> dv;
    bcg_scalar_t d_max;
};

}

#endif //BCG_GRAPHICS_BCG_ROTATION_GEODESIC_MEDIAN_H
