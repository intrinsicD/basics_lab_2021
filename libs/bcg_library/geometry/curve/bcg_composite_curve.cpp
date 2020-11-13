//
// Created by alex on 13.11.20.
//

#include "bcg_composite_curve.h"

namespace bcg{

VectorS<3> composite_curve::operator()(bcg_scalar_t t) const{
    return evaluate(t);
}

VectorS<3> composite_curve::evaluate(bcg_scalar_t t) const{

}

VectorS<3> composite_curve::derivative_vector(bcg_scalar_t t, int order) const{

}

VectorS<3> composite_curve::first_derivative(bcg_scalar_t t) const{

}

VectorS<3> composite_curve::second_derivative(bcg_scalar_t t) const{

}

VectorS<3> composite_curve::third_derivative(bcg_scalar_t t) const{

}

VectorS<3> composite_curve::tangent_vector(bcg_scalar_t t) const{

}

VectorS<3> composite_curve::curvature_vector(bcg_scalar_t t) const{

}

VectorS<3> composite_curve::normal_vector(bcg_scalar_t t) const{

}

VectorS<3> composite_curve::binormal_vector(bcg_scalar_t t) const{

}

bcg_scalar_t composite_curve::curvature(bcg_scalar_t t) const{

}

bcg_scalar_t composite_curve::torsion(bcg_scalar_t t) const{

}

bcg_scalar_t composite_curve::length(bcg_scalar_t a, bcg_scalar_t b, size_t n) const{

}

MatrixS<3, 3> composite_curve::frenet_frame(bcg_scalar_t t) const{

}

std::vector<VectorS<3>> composite_curve::sample(size_t num_points) const{

}

std::vector<VectorI<2>> composite_curve::connectivity(size_t num_points) const{

}

int composite_curve::degree() const{

}

int composite_curve::segment_index(bcg_scalar_t t){
    return std::floor(t * (segments.size() - 1));
}

bcg_scalar_t composite_curve::segment_parameter(bcg_scalar_t t){
    return segment_index(t) / bcg_scalar_t(segments.size() - 1);
}

}