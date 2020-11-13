//
// Created by alex on 13.11.20.
//

#include "bcg_curve.h"

namespace bcg {

VectorS<3> curve::operator()(bcg_scalar_t t) const {
    return evaluate(t);
}

VectorS<3> curve::evaluate(bcg_scalar_t t) const { return VectorS<3>::Zero(); }

VectorS<3> curve::derivative_vector(bcg_scalar_t t, int order) const { return VectorS<3>::Zero(); }

const std::vector<VectorS<3>> &curve::control_points() const { return positions.vector(); };

VectorS<3> curve::first_derivative(bcg_scalar_t t) const {
    return derivative_vector(t, 1);
}

VectorS<3> curve::second_derivative(bcg_scalar_t t) const {
    return derivative_vector(t, 2);
}

VectorS<3> curve::third_derivative(bcg_scalar_t t) const {
    return derivative_vector(t, 3);
}

VectorS<3> curve::tangent_vector(bcg_scalar_t t) const {
    return first_derivative(t);
}

VectorS<3> curve::curvature_vector(bcg_scalar_t t) const {
    return second_derivative(t);
}

VectorS<3> curve::normal_vector(bcg_scalar_t t) const {
    return curvature_vector(t).normalized();
}

VectorS<3> curve::binormal_vector(bcg_scalar_t t) const {
    return tangent_vector(t).cross(normal_vector(t));
}

bcg_scalar_t curve::curvature(bcg_scalar_t t) const {
    return curvature_vector(t).norm();
}

bcg_scalar_t curve::torsion(bcg_scalar_t t) const {
    auto tmp = tangent_vector(t).cross(curvature_vector(t));
    return tmp.dot(third_derivative(t)) / tmp.squaredNorm();
}

bcg_scalar_t curve::length(bcg_scalar_t a, bcg_scalar_t b, size_t n = 100) const {
    bcg_scalar_t result = 0;
    bcg_scalar_t dt = (b - a) / bcg_scalar_t(n);
    bcg_scalar_t t = a;
    for (size_t i = 0; i < n; ++i) {
        result += tangent_vector(t).norm();
        t += dt;
    }
    return result;
}

MatrixS<3, 3> curve::frenet_frame(bcg_scalar_t t) const {
    MatrixS<3, 3> frame;
    frame.col(0) = derivative_vector(t, 1).normalized();
    for (size_t j = 1; j < 3; ++j) {
        VectorS<3> dv = derivative_vector(t, j);
        VectorS<3> vec = dv;
        for (size_t i = 1; i < j - 1; ++i) {
            vec -= dv.dot(frame.col(i)) * frame.col(i);
        }
        frame.col(j) = vec;
    }
    return frame;
}

std::vector<VectorS<3>> curve::sample(size_t num_points) const {
    std::vector<VectorS<3>> samples(num_points);
    bcg_scalar_t t = 0;
    bcg_scalar_t dt = 1.0 / bcg_scalar_t(num_points);
    for (int i = 0; i < num_points; ++i) {
        samples[i] = evaluate(t);
        t += dt;
    }
    return samples;
}

std::vector<VectorI<2>> curve::connectivity(size_t num_points) const {
    std::vector<VectorI<2>> edges(num_points);
    for (int i = 0; i < num_points - 1; ++i) {
        edges[i] = VectorI<2>(i, i + 1);
    }
    return edges;
}

int curve::degree() const{
    return positions.size();
}

}