//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_LAPLACIAN_HARMONIC_FIELD_H
#define BCG_GRAPHICS_BCG_LAPLACIAN_HARMONIC_FIELD_H

#include "bcg_laplacian_matrix.h"
#include "bcg_property_map_eigen.h"
#include "point_cloud/bcg_point_cloud.h"
#include "Eigen/SparseLU"

namespace bcg{

template<typename T, int N>
void laplacian_harmonic_field(vertex_container *vertices, const laplacian_matrix &laplacian, property<T, N> &p, bcg_scalar_t weight = 1000){
    Eigen::SparseMatrix<bcg_scalar_t > Op = laplacian.S;
    auto v_feature = vertices->get<bool, 1>("v_feature");

    MatrixS<-1, N> F = MatrixS<-1, N>::Zero(p.size(), p.dims());
    MatrixS<-1, N> B = MatrixS<-1, N>::Zero(p.size(), p.dims());

    std::vector<vertex_handle> constrained;
    for (const auto &v : *vertices) {
        if(v_feature && v_feature[v]){
            F.row(v.idx) = MapConst(p).row(v);
            constrained.push_back(v);
        }
    }

    if(constrained.empty()){
        std::cerr << "Please provide constrained vertices in v_feature property\n";
        return;
    }

    B -= Op * F;

    for (const auto &v : constrained) {
        Op.row(v.idx) *= 0;
        Op.col(v.idx) *= 0;
        Op.coeffRef(v.idx, v.idx) = weight;
        B.row(v.idx) = MapConst(p).row(v) * weight;
    }

    Eigen::SparseLU<Eigen::SparseMatrix<bcg_scalar_t>> solver;

    solver.compute(Op);
    if (solver.info() != Eigen::Success) {
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! SPARSE DECOMPOSTION FAILED !!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        return;
    }
    auto result = vertices->get_or_add<T, N>("v_harmonic_field");
    Map(result) = solver.solve(B);
    for (const auto &v : constrained) {
        if (result[v] != p[v]) {
            std::cout << "idx: " << v.idx << " result: " << result[v] << "!=" << p[v] << std::endl;
            assert(false);
        }
    }
}

void laplacian_harmonic_field_segmentation(vertex_container *vertices){

}

}

#endif //BCG_GRAPHICS_BCG_LAPLACIAN_HARMONIC_FIELD_H
