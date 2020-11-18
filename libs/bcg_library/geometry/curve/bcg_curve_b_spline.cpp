//
// Created by alex on 18.11.20.
//

#include "bcg_curve_b_spline.h"

namespace bcg{

curve_b_spline::curve_b_spline(){

}

curve_b_spline::curve_b_spline(const std::vector<VectorS<3>> &control_points_, KnotType knot_type){
    for (const auto &cp : control_points_) {
        add_vertex(cp);
    }
    for (size_t i = 0; i < control_points_.size() - 1; ++i) {
        add_edge(i, i + 1);
    }
    bcg_scalar_t u = 0;
    bcg_scalar_t du = 1.0 / control_points_.size();
    if(knot_type == KnotType::nonuniform){
        knots = {0, 0, 0, 0};
    }
    for (const auto &cp : control_points_) {
       knots.push_back(u);
       u += du;
    }
    if(knot_type == KnotType::nonuniform){
        knots.push_back(1.0);
        knots.push_back(1.0);
        knots.push_back(1.0);
        knots.push_back(1.0);
    }
}

VectorS<3> curve_b_spline::evaluate(bcg_scalar_t t) const{

}

curve_b_spline curve_b_spline::derivative(int order) const{

}

VectorS<3> curve_b_spline::derivative_vector(bcg_scalar_t t, int order) const {

}

VectorS<3> curve_b_spline::de_boor(bcg_scalar_t t){

}

std::array<curve_b_spline, 2> curve_b_spline::subdivide(bcg_scalar_t t = 0.5){

}

curve_b_spline curve_b_spline::increase_degree(){

}

}