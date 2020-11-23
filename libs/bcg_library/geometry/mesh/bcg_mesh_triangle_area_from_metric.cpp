//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_triangle_area_from_metric.h"

namespace bcg{

bcg_scalar_t triangle_area_from_metric(bcg_scalar_t a, bcg_scalar_t b, bcg_scalar_t c, bcg_scalar_t eps) {
    //Numerically stable herons formula
    auto swap = [](bcg_scalar_t *a, bcg_scalar_t *b) {
        bcg_scalar_t t = *b;
        *b = *a;
        *a = t;
    };
    if (a < b) swap(&a, &b);
    if (a < c) swap(&a, &c);
    if (b < c) swap(&b, &c);

    bcg_scalar_t p = std::abs((a + (b + c)) * (c - (a - b)) * (c + (a - b)) * (a + (b - c)));
    if(p < eps) p = eps;
    return 0.25 * std::sqrt(p);
}

bcg_scalar_t triangle_area_from_metric(const VectorS<3> &abc, bcg_scalar_t eps) {
    return triangle_area_from_metric(abc[0], abc[1], abc[2], eps);
}

}