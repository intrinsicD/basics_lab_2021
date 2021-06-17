//
// Created by alex on 17.06.21.
//

#include "bcg_object_space.h"

namespace bcg{

VectorS<3> object_space_compute_center(const MatrixS<-1, 3> &data){
    return data.colwise().mean();
}

VectorS<3> object_space_compute_scale(const MatrixS<-1, 3> &data){
    return data.cwiseAbs().colwise().maxCoeff();
}

bcg_scalar_t object_space_compute_uniform_scale(const MatrixS<-1, 3> &data){
    return data.cwiseAbs().maxCoeff();
}

}