//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_CURVE_H
#define BCG_GRAPHICS_BCG_CURVE_H

#include <functional>
#include "math/bcg_linalg.h"

namespace bcg {

template<int N>
struct curve {
    curve() = default;

    VectorS<N> operator()(bcg_scalar_t t) const {
        return evaluate(t);
    }

    virtual VectorS<N> evaluate(bcg_scalar_t t) const { return VectorS<N>::Zero(); }

    virtual VectorS<N> derivative_vector(bcg_scalar_t t, int order) const { return VectorS<N>::Zero(); }

    virtual std::vector<VectorS<N>> control_points() const { return std::vector<VectorS<N>>(); };

    VectorS<N> first_derivative(bcg_scalar_t t) const {
        return derivative_vector(t, 1);
    }

    VectorS<N> second_derivative(bcg_scalar_t t) const {
        return derivative_vector(t, 2);
    }

    VectorS<N> third_derivative(bcg_scalar_t t) const {
        return derivative_vector(t, 3);
    }

    VectorS<N> tangent_vector(bcg_scalar_t t) const {
        return first_derivative(t);
    }

    VectorS<N> curvature_vector(bcg_scalar_t t) const {
        return second_derivative(t);
    }

    VectorS<N> normal_vector(bcg_scalar_t t) const {
        return curvature_vector(t).normalized();
    }

    VectorS<N> binormal_vector(bcg_scalar_t t) const {
        return tangent_vector(t).cross(normal_vector(t));
    }

    bcg_scalar_t curvature(bcg_scalar_t t) const {
        return curvature_vector(t).norm();
    }

    bcg_scalar_t torsion(bcg_scalar_t t) const {
        auto tmp = tangent_vector(t).cross(curvature_vector(t));
        return tmp.dot(third_derivative(t)) / tmp.squaredNorm();
    }

    bcg_scalar_t length(bcg_scalar_t a, bcg_scalar_t b, size_t n = 100) const {
        bcg_scalar_t result = 0;
        bcg_scalar_t dt = (b - a) / bcg_scalar_t(n);
        bcg_scalar_t t = a;
        for (size_t i = 0; i < n; ++i) {
            result += tangent_vector(t).norm();
            t += dt;
        }
        return result;
    }

    MatrixS<N, N> frenet_frame(bcg_scalar_t t) const {
        MatrixS<N, N> frame;
        frame.col(0) = derivative_vector(t, 1).normalized();
        for (size_t j = 1; j < N; ++j) {
            VectorS<N> dv = derivative_vector(t, j);
            VectorS<N> vec = dv;
            for (size_t i = 1; i < j - 1; ++i) {
                vec -= dv.dot(frame.col(i)) * frame.col(i);
            }
            frame.col(j) = vec;
        }
        return frame;
    }

    std::vector<VectorS<N>> sample(size_t num_points) const {
        std::vector<VectorS<N>> samples(num_points);
        bcg_scalar_t t = 0;
        bcg_scalar_t dt = 1.0 / bcg_scalar_t(num_points);
        for (int i = 0; i < num_points; ++i) {
            samples[i] = evaluate(t);
            t += dt;
        }
        return samples;
    }

    std::vector<VectorI<2>> connectivity(size_t num_points) const {
        std::vector<VectorI<2>> edges(num_points);
        for (int i = 0; i < num_points - 1; ++i) {
            edges[i] = VectorI<2>(i, i + 1);
        }
        return edges;
    }


};

}

#endif //BCG_GRAPHICS_BCG_CURVE_H
