//
// Created by alex on 04.12.20.
//

#include "bcg_rotation_quaternion_mean.h"
#include "bcg_rotations_convert.h"
#include "bcg_rotation_robust_elementwise_median.h"
#include "bcg_rotation_distances.h"

namespace bcg{

quaternion_mean::quaternion_mean() : m_robust(false) {}

quaternion_mean::quaternion_mean(bool robust) : m_robust(robust) {}

Quaternion quaternion_mean::operator()(const std::vector<Quaternion> &rotations) const {
    Quaternion s;
    if(m_robust){
        s = Quaternion(robust_elementwise_median(convert_to_rotationmatrices(rotations)));
    }else{
        s = rotations[0];
    }
    s.normalize();

    Quaternion result;
    for (const Quaternion &ri : rotations) {
        assert(quaternion_distance(ri, s) < pi / 2);
        if ((ri.coeffs() - s.coeffs()).norm() > (ri.coeffs() + s.coeffs()).norm()) {
            result.coeffs() -= ri.coeffs();
        } else {
            result.coeffs() += ri.coeffs();
        }
    }

    return result.normalized();
}

}