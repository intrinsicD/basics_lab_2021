//
// Created by alex on 04.05.21.
//

#include "bcg_translation_mean.h"
#include "math/matrix/bcg_matrix_map_eigen.h"
#include "math/statistics/bcg_statistics_quantil.h"

namespace bcg {

translation_mean::translation_mean(bool outlier_reject) : m_outlier_reject(outlier_reject) {

}

VectorS<3> translation_mean::operator()(const std::vector<VectorS<3>> &translations) {
    return operator()(MapConst(translations));
}

VectorS<3> translation_mean::operator()(const MatrixS<-1, 3> &translations) {
    if (m_outlier_reject) {
        VectorS<-1> norms = translations.rowwise().norm();
        auto result = quantil<bcg_scalar_t>(norms);
        VectorS<-1> weights = (norms.array() < result.value).select(VectorS<-1>::Ones(translations.size()), 0.0);
        return weights.transpose() * translations / weights.sum();
    }
    return translations.colwise().mean();
}


}