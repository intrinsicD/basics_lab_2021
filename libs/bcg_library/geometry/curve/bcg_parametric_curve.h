//
// Created by alex on 12.11.20.
//

#ifndef BCG_GRAPHICS_BCG_PARAMETRIC_CURVE_H
#define BCG_GRAPHICS_BCG_PARAMETRIC_CURVE_H

#include <functional>
#include "geometry/graph/bcg_graph.h"

namespace bcg {

template<typename DerivedCurve>
struct parametric_curve : public halfedge_graph {
    parametric_curve() {
        parameterization_function = [](bcg_scalar_t t) {
            return t;
        };
    }

    virtual VectorS<3> evaluate(bcg_scalar_t t) {};

    virtual DerivedCurve derivative(int order) {};

    virtual VectorS<3> derivative(bcg_scalar_t t, int order) {};

    bcg_scalar_t speed(bcg_scalar_t t) {
        return derivative(t, 1).norm();
    }

    bcg_scalar_t length() {
        return derivative(1, 1).norm() - derivative(0, 1).norm();
    }

    bcg_scalar_t curvature(bcg_scalar_t t) {
        return curvature_vector(t).norm();
    }

    VectorS<3> tangent_vector(bcg_scalar_t t) {
        return derivative(t, 1);
    }

    VectorS<3> curvature_vector(bcg_scalar_t t) {
        return derivative(t, 2);
    }

    VectorS<3> normal_vector(bcg_scalar_t t) {
        return curvature_vector(t).normalized();
    }

    VectorS<3> binormal_vector(bcg_scalar_t t) {
        return tangent_vector(t).cross(normal_vector(t));
    }

    std::function<bcg_scalar_t(bcg_scalar_t)> parameterization_function;
};

}

#endif //BCG_GRAPHICS_BCG_PARAMETRIC_CURVE_H
