//
// Created by alex on 24.09.20.
//

#ifndef BCG_CHRODALMEAN_H
#define BCG_CHRODALMEAN_H

#include "math/bcg_linalg.h"
#include <vector>

namespace bcg {

struct chordal_mean_so3 {
    chordal_mean_so3(bool closed_form, bool outlier_reject);

    chordal_mean_so3(std::vector<bcg_scalar_t> weights, bool closed_form, bool outlier_reject);

    MatrixS<3, 3> closed_form(const std::vector<MatrixS<3, 3>> &rotations);

    MatrixS<3, 3> operator()(const std::vector<MatrixS<3, 3>> &rotations);

private:
    bool m_closed_form, m_outlier_reject;
    std::vector<bcg_scalar_t> m_weights;
    std::vector<std::pair<bcg_scalar_t, VectorS<3>>> dv;
    bcg_scalar_t d_max;
};

}

#endif //BCG_CHRODALMEAN_H
