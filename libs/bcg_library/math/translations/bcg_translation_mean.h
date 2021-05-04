//
// Created by alex on 04.05.21.
//

#ifndef BCG_GRAPHICS_BCG_TRANSLATION_MEAN_H
#define BCG_GRAPHICS_BCG_TRANSLATION_MEAN_H

#include "math/bcg_linalg.h"

namespace bcg{

struct translation_mean{
    translation_mean(bool outlier_reject);

    VectorS<3> operator()(const std::vector<VectorS<3>> &translations);

    VectorS<3> operator()(const MatrixS<-1, 3> &translations);

private:
    bool  m_outlier_reject;
};

}

#endif //BCG_GRAPHICS_BCG_TRANSLATION_MEAN_H
