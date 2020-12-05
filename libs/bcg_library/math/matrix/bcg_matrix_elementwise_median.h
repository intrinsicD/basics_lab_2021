//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_ELEMENTWISE_MEDIAN_H
#define BCG_GRAPHICS_BCG_MATRIX_ELEMENTWISE_MEDIAN_H

#include "math/matrix/bcg_matrix.h"
#include "math/statistics/bcg_statistics_running.h"

namespace bcg {

template<int M, int N>
inline MatrixS<M, N> elementwise_median(const std::vector<MatrixS<M, N>> &matrices) {
    running_stats stats;
    MatrixS<M, N> Median(MatrixS<M, N>::Zero());
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            for (size_t k = 0; k < matrices.size(); ++k) {
                stats.push(matrices[k](i, j));
            }
            Median(i, j) = stats.median();
            assert(!std::isnan(Median(i, j)));
        }
    }
    return Median;
}

}

#endif //BCG_GRAPHICS_BCG_MATRIX_ELEMENTWISE_MEDIAN_H
