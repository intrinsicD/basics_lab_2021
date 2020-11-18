//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_CURVE_H
#define BCG_GRAPHICS_BCG_CURVE_H

#include <functional>
#include "math/bcg_linalg.h"
#include "geometry/graph/bcg_graph.h"

namespace bcg {

struct curve : public halfedge_graph{
    curve() = default;

    VectorS<3> operator()(bcg_scalar_t t) const;

    virtual VectorS<3> evaluate(bcg_scalar_t t) const;

    virtual VectorS<3> derivative_vector(bcg_scalar_t t, int order) const;

    const std::vector<VectorS<3>> &control_points() const;

    VectorS<3> first_derivative(bcg_scalar_t t) const;

    VectorS<3> second_derivative(bcg_scalar_t t) const;

    VectorS<3> third_derivative(bcg_scalar_t t) const;

    VectorS<3> tangent_vector(bcg_scalar_t t) const;

    VectorS<3> curvature_vector(bcg_scalar_t t) const;

    VectorS<3> normal_vector(bcg_scalar_t t) const;

    VectorS<3> binormal_vector(bcg_scalar_t t) const;

    bcg_scalar_t curvature(bcg_scalar_t t) const;

    bcg_scalar_t torsion(bcg_scalar_t t) const;

    bcg_scalar_t length(bcg_scalar_t a, bcg_scalar_t b, size_t n = 100) const;

    MatrixS<3, 3> frenet_frame(bcg_scalar_t t) const;

    std::vector<VectorS<3>> sample(size_t num_points) const;

    std::vector<VectorI<2>> connectivity(size_t num_points) const;

    int degree() const;
};

}

#endif //BCG_GRAPHICS_BCG_CURVE_H
