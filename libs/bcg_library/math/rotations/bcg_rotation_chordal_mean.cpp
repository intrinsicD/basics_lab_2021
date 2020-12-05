//
// Created by alex on 04.12.20.
//

#include "bcg_rotation_chordal_mean.h"
#include "bcg_rotation_project_on_so.h"
#include "bcg_rotation_matrix_logarithm.h"
#include "bcg_rotation_matrix_exponential.h"
#include "Eigen/Eigenvalues"

namespace bcg {

chrodal_mean_so3::chrodal_mean_so3(bool closed_form, bool outlier_reject) : m_closed_form(closed_form),
                                                                            m_outlier_reject(outlier_reject) {}

chrodal_mean_so3::chrodal_mean_so3(std::vector<bcg_scalar_t> weights, bool closed_form, bool outlier_reject)
        : m_closed_form(closed_form), m_outlier_reject(outlier_reject), m_weights(std::move(weights)) {}

MatrixS<3, 3> chrodal_mean_so3::closed_form(const std::vector<MatrixS<3, 3>> &rotations) {
    if (m_weights.empty()) {
        m_weights = std::vector<bcg_scalar_t>(rotations.size(), 1);
    }
    MatrixS<3, 3> A(MatrixS<3, 3>::Zero());
    bcg_scalar_t weight_sum = 0;
    if (m_outlier_reject) {
        dv = std::vector<std::pair<bcg_scalar_t, VectorS<3>>>(rotations.size());
        d_max = (rotations.size() > 50 ? 0.5 : 1.0);

        for (size_t i = 0; i < rotations.size(); ++i) {
            dv[i].second = matrix_logarithm(rotations[i]);
            dv[i].first = dv[i].second.norm();
            if (dv[i].first == 0) {
                dv[i].first = scalar_max;
            }
        }

        std::sort(dv.begin(), dv.end(),
                  [](const std::pair<bcg_scalar_t, VectorS<3>> &lhs, const std::pair<bcg_scalar_t, VectorS<3>> &rhs) {
                      return lhs.first < rhs.first;
                  });
        size_t d_quantil(std::floor(dv.size() / 4));
        bcg_scalar_t d_threshold(fmax(d_max, dv[d_quantil].first));

        for (size_t i = 0; i < rotations.size(); ++i) {
            const auto &item = dv[i];
            if (item.first > d_threshold) break;
            weight_sum += m_weights[i];
        }

        for (size_t i = 0; i < rotations.size(); ++i) {
            const auto &item = dv[i];
            if (item.first > d_threshold) break;

            A += matrix_exponential(item.second * m_weights[i] / weight_sum);
        }
    } else {
        for (size_t i = 0; i < rotations.size(); ++i) {
            weight_sum += m_weights[i];
        }
        for (size_t i = 0; i < rotations.size(); ++i) {
            A += matrix_exponential(matrix_logarithm(rotations[i]) * m_weights[i] / weight_sum);
        }
    }

    return project_on_so(A, true).transpose();
}

MatrixS<3, 3> chrodal_mean_so3::operator()(const std::vector<MatrixS<3, 3>> &rotations) {
    if (m_closed_form) {
        return closed_form(rotations);
    } else {
        if (m_weights.empty()) {
            m_weights = std::vector<bcg_scalar_t>(rotations.size(), 1);
        }

        bcg_scalar_t weight_sum = 0;
        MatrixS<4, 4> A(MatrixS<4, 4>::Zero());
        if (m_outlier_reject) {
            dv = std::vector<std::pair<bcg_scalar_t, VectorS<3>>>(rotations.size());
            d_max = (rotations.size() > 50 ? 0.5 : 1.0);

            for (size_t i = 0; i < rotations.size(); ++i) {
                dv[i].second = matrix_logarithm(rotations[i]);
                dv[i].first = dv[i].second.norm();
                if (dv[i].first == 0) {
                    dv[i].first = scalar_max;
                }
            }

            std::sort(dv.begin(), dv.end(),
                      [](const std::pair<bcg_scalar_t, VectorS<3>> &lhs,
                         const std::pair<bcg_scalar_t, VectorS<3>> &rhs) {
                          return lhs.first < rhs.first;
                      });
            size_t d_quantil(std::floor(dv.size() / 4));
            bcg_scalar_t d_threshold(fmax(d_max, dv[d_quantil].first));

            for (size_t i = 0; i < rotations.size(); ++i) {
                const auto &item = dv[i];
                if (item.first > d_threshold) break;
                weight_sum += m_weights[i];
            }

            for (size_t i = 0; i < rotations.size(); ++i) {
                const auto &item = dv[i];
                if (item.first > d_threshold) break;

                Quaternion r(matrix_exponential(item.second * m_weights[i] / weight_sum));
                A += r.coeffs() * r.coeffs().transpose();
            }
        } else {
            for (size_t i = 0; i < rotations.size(); ++i) {
                weight_sum += m_weights[i];
            }
            for (size_t i = 0; i < rotations.size(); ++i) {
                Quaternion
                        r(matrix_exponential(matrix_logarithm(rotations[i]) * m_weights[i] / weight_sum));
                A += r.coeffs() * r.coeffs().transpose();
            }
        }
        Eigen::SelfAdjointEigenSolver<MatrixS<4, 4>> eigenSolver(A);
        return Quaternion(eigenSolver.eigenvectors().col(3)).toRotationMatrix();
    }
}


}