//
// Created by alex on 03.12.20.
//

#ifndef BCG_GRAPHICS_BCG_GAUSSIAN_H
#define BCG_GRAPHICS_BCG_GAUSSIAN_H

#include "math/bcg_linalg.h"
#include "Eigen/Eigenvalues"

namespace bcg {

template<int N>
inline bcg_scalar_t
mahalonobis_distance_squared(const VectorS <N> &x, const VectorS <N> &mean, const MatrixS <N, N> &cov_inv) {
    VectorS<N> diff = x - mean;
    return diff.transpose() * cov_inv * diff;
}

template<int N>
inline bcg_scalar_t
mahalonobis_distance(const VectorS <N> &x, const VectorS <N> &mean, const MatrixS <N, N> &cov_inv) {
    return std::sqrt(mahalonobis_distance_squared<N>(x, mean, cov_inv));
}

inline bcg_scalar_t
gaussian_distance(bcg_scalar_t distance_squared) {
    return std::exp(-distance_squared / 2.0);
}

inline bcg_scalar_t
gaussian_distance(bcg_scalar_t distance_squared, bcg_scalar_t sigma_squared) {
    return gaussian_distance(distance_squared / sigma_squared);
}

template<int N>
inline bcg_scalar_t
gaussian_distance(const VectorS <N> &x, const VectorS <N> &mean, const MatrixS <N, N> &cov_inv) {
    return gaussian_distance(mahalonobis_distance_squared<N>(x, mean, cov_inv));
}

inline bcg_scalar_t
gaussian_normalizer(bcg_scalar_t sigma_squared) {
    return 1.0 / std::sqrt(2.0 * pi * sigma_squared);
}

template<int N>
inline bcg_scalar_t
gaussian_normalizer(const MatrixS <N, N> &cov) {
    return 1.0 / std::sqrt(std::pow(2.0 * pi, N) * cov.determinant());
}

template<int N>
struct Gaussian {
    VectorS <N> mean;
    MatrixS <N, N> cov;

    Gaussian() = default;

    explicit Gaussian(size_t dims) : mean(VectorS<N>::Zero(dims)), cov(MatrixS<N, N>::Identity(dims, dims)) {

    }

    Gaussian(const VectorS <N> &mean, const MatrixS <N, N> &cov) : mean(mean), cov(cov) {

    }

    bcg_scalar_t normalizer() const {
        return gaussian_normalizer<N>(cov);
    }

    void construct_from_points(const MatrixS <N, N> &points, const VectorS <N> &mean_) {
        MatrixS<N, N> centered(points.rowwise() - mean_.transpose());
        mean = mean_;
        cov = centered.transpose() * centered / (points.rows() - 1);
    }

    bcg_scalar_t operator()(const VectorS <N> &x) const {
        return gaussian_distance<N>(x, mean, cov_inverse()) * normalizer();
    }

    //Paper: Designing a Metric for the DifferenceBetween Gaussian Densities
    bcg_scalar_t distance(const Gaussian<N> &other) const {
        MatrixS<N, N> S_inv = (cov / 2.0 + other.cov / 2.0).inverse();
        VectorS<N> u = mean - other.mean;
        Eigen::GeneralizedSelfAdjointEigenSolver<MatrixS<N, N>> solver(cov, other.cov, Eigen::EigenvaluesOnly);
        return std::sqrt(u.transpose() * S_inv * u) + std::sqrt(solver.eigenvalues().array().log().squared().sum());
    }

    MatrixS <N, N> cov_inverse() const {
        return cov.inverse();
    }
};

template<int N>
bcg_scalar_t symmetric_kullback_leibler_divergence(const Gaussian<N> &g1, const Gaussian<N> &g2) {
    VectorS<N> u = g1.mean - g2.mean;
    return (u.transpose() * (g1.cov_inv + g2.cov_inv) * u +
            (g1.cov_inv * g2.cov + g2.cov_inv * g1.cov - 2 * MatrixS<N, N>::Identity(g1.mean.size(), g1.mean.size()))) /
           2.0;
}

template<int N>
bcg_scalar_t bhattacharyya_coefficient(const Gaussian<N> &g1, const Gaussian<N> &g2) {
    MatrixS<N, N> cov = (g1.cov / 2.0 + g2.cov / 2.0);
    return std::pow(g1.cov.determinant() * g2.cov.determinant(), 0.25) *
           gaussian_distance<N>(g1.mean, g2.mean, cov.inverse() / 4) / std::sqrt(cov.determinant());
}

template<int N>
bcg_scalar_t bhattacharyya_divergence(const Gaussian<N> &g1, const Gaussian<N> &g2) {
    VectorS<N> u = g1.mean - g2.mean;
    return u.transpose() * (g1.cov / 2.0 + g2.cov / 2.0).inverse() * u / 8.0 +
           std::log((g1.cov + g2.cov).determinant() / std::sqrt(g1.cov.determinant() * g2.cov.determinant())) / 2.0;
}

using Gaussian3 = Gaussian<3>;

}

#endif //BCG_GRAPHICS_BCG_GAUSSIAN_H
