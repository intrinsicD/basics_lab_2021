//
// Created by alex on 04.12.20.
//

#include "bcg_rotation_quaternion_median.h"

namespace bcg{

quaternion_median::quaternion_median(bool robust, bool outlier_reject) : m_robust(robust), m_outlier_reject(outlier_reject), m_s(Quaternion::Identity()){}

quaternion_median::quaternion_median(std::vector<bcg_scalar_t> weights, bool robust, bool outlier_reject): m_robust(robust), m_outlier_reject(outlier_reject), m_s(Quaternion::Identity()), m_weights(std::move(weights)){}

quaternion_median::quaternion_median(const Quaternion &initial_estimate, bool outlier_reject) : m_robust(false), m_outlier_reject(outlier_reject), m_s(initial_estimate){}

quaternion_median::quaternion_median(const Quaternion &initial_estimate, std::vector<bcg_scalar_t> weights, bool outlier_reject): m_robust(false), m_outlier_reject(outlier_reject), m_s(initial_estimate), m_weights(std::move(weights)){}

Quaternion quaternion_median::operator()(const std::vector<Quaternion> &rotations, bcg_scalar_t eps, int max_iterations){
    if(m_robust){
        m_s = Rotation(robust_elementwise_median(convert_to_rotationmatrices(rotations)));
    }else{
        m_s = rotations[0];
    }

    m_s.normalize();

    if(m_weights.empty()){
        m_weights = std::vector<bcg_scalar_t>(rotations.size(), 1);
    }

    if(m_outlier_reject){
        dv = std::vector<std::pair<bcg_scalar_t, Rotation>>(rotations.size());
        d_max = (rotations.size() > 50 ? 0.5 : 1.0);
    }

    bcg_scalar_t last_error = scalar_max;
    int counter = 0;
    while (true) {
        VectorS<3> delta = VectorS<3>::Zero();
        bcg_scalar_t weight_sum = 0;

        if(m_outlier_reject){
            for (size_t i = 0; i < rotations.size(); ++i) {
                dv[i].second = Rotation(rotations[i] * m_s.conjugate());
                dv[i].first = dv[i].second.angle();
                if (dv[i].first == 0) {
                    dv[i].first = std::numeric_limits<bcg_scalar_t>::max();
                }
            }

            std::sort(dv.begin(), dv.end(),
                      [](const std::pair<bcg_scalar_t, Rotation> &lhs, const std::pair<bcg_scalar_t, Rotation> &rhs) {
                          return lhs.first < rhs.first;
                      });
            size_t d_quantil(std::floor(dv.size() / 4));
            bcg_scalar_t d_threshold(std::max<bcg_scalar_t>(d_max, dv[d_quantil].first));

            for (size_t i = 0; i < rotations.size(); ++i) {
                const auto &item = dv[i];
                if (item.first > d_threshold) break;

                delta += item.second.axis().normalized();
                weight_sum += m_weights[i] / item.first;

                assert(!delta.array().isNaN().any());
            }
        }else{
            for (size_t i = 0; i < rotations.size(); ++i) {
                auto v = Rotation(rotations[i] * m_s.conjugate());
                bcg_scalar_t length = v.angle();
                if (length > 0) {
                    delta += v.axis().normalized();
                    weight_sum += m_weights[i] / length;
                    assert(!delta.array().isNaN().any());
                }
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

        m_s = Quaternion(Rotation(error, delta.normalized())) * m_s;
        if (counter > max_iterations) break;
        ++counter;
    }

    return m_s;
}

}