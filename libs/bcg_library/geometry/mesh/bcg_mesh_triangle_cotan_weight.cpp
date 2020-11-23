//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_triangle_cotan_weight.h"
#include "bcg_mesh_triangle_area_from_metric.h"

namespace bcg {

bcg_scalar_t clamp_cot(bcg_scalar_t v) {
    const bcg_scalar_t bound = 19.1;
    return (v < -bound ? -bound : (v > bound ? bound : v));
}

//compute the cotan value of alpha for edge v0-v1 at v2
bcg_scalar_t triangle_cotan(const VectorS<3> &v0, const VectorS<3> &v1, const VectorS<3> &v2) {
    const auto d0 = v0 - v2;
    const auto d1 = v1 - v2;
    const auto d2 = v1 - v0;
    const auto area = triangle_area_from_metric(d0.norm(), d1.norm(), d2.norm());
    if (area > scalar_eps) {
        return clamp_cot(d0.dot(d1) / area); // ||a||*||b||*cos(alpha) / (||a||*||b||*sin(alpha) = cot(alpha)
    }
    return scalar_eps;
}

}