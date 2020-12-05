//
// Created by alex on 04.12.20.
//

#include "bcg_rotation_geodesic_mean.h"

namespace bcg{

geodesic_mean_so3::geodesic_mean_so3(bool robust, bool outlier_reject) : m_robust(robust), m_outlier_reject(outlier_reject), m_R(MatrixS<3, 3>::Identity()){}

geodesic_mean_so3::geodesic_mean_so3(std::vector<bcg_scalar_t> weights, bool robust, bool outlier_reject): m_robust(robust), m_outlier_reject(outlier_reject), m_R(MatrixS<3, 3>::Identity()), m_weights(std::move(weights)){}

geodesic_mean_so3::geodesic_mean_so3(const MatrixS<3, 3> &initial_estimate, bool outlier_reject) : m_robust(false), m_outlier_reject(outlier_reject), m_R(initial_estimate){}

geodesic_mean_so3::geodesic_mean_so3(const MatrixS<3, 3> &initial_estimate, std::vector<bcg_scalar_t> weights, bool outlier_reject): m_robust(false), m_outlier_reject(outlier_reject), m_R(initial_estimate), m_weights(std::move(weights)){}

MatrixS<3, 3> geodesic_mean_so3::operator()(const std::vector<MatrixS<3, 3>> &rotations, bcg_scalar_t eps, int max_iterations){
    if(m_robust){
        m_R = robust_elementwise_median(rotations);
    }else{
        m_R = rotations[0];
    }

    if(m_weights.empty()){
        m_weights = std::vector<bcg_scalar_t>(rotations.size(), 1);
    }

    if(m_outlier_reject){
        dv = std::vector<std::pair<bcg_scalar_t, VectorS<3>>>(rotations.size());
        d_max = (rotations.size() > 50 ? 0.5 : 1.0);
    }

    bcg_scalar_t last_error = scalar_max;
    int counter = 0;
    while (true) {
        VectorS<3> delta = VectorS<3>::Zero();
        bcg_scalar_t weight_sum = 0;

        if(m_outlier_reject){
            for (size_t i = 0; i < rotations.size(); ++i) {
                dv[i].second = matrix_logarithm(rotations[i] * m_R.transpose());
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

                delta += item.second * m_weights[i];
                weight_sum += m_weights[i];

                assert(!delta.array().isNaN().any());
            }
        }else{
            for (size_t i = 0; i < rotations.size(); ++i) {
                delta += matrix_logarithm(rotations[i] * m_R.transpose()) * m_weights[i];
                weight_sum += m_weights[i];
                assert(!delta.array().isNaN().any());
            }
        }

        if (weight_sum > 0) {
            delta /= weight_sum;
        }

        bcg_scalar_t error = delta.norm();

        if (error < eps || last_error == error) {
            break;
        }

        last_error = error;

        m_R = matrix_exponential(delta) * m_R;
        if (counter > max_iterations) break;
        ++counter;
    }

    return m_R;
}

}