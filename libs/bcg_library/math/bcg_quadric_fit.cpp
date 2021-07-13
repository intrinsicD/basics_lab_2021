//
// Created by alex on 12.07.21.
//

#include "bcg_quadric_fit.h"
#include "Eigen/Eigenvalues"
#include "Eigen/SVD"
#include "matrix/bcg_matrix_map_eigen.h"

namespace bcg {

bcg_scalar_t fit(const std::vector<VectorS<3>> &points, quadric &Q) {
    return fit(MapConst(points), Q);
}

bcg_scalar_t fit(const MatrixS<-1, 3> &points, quadric &Q) {
    MatrixS<10, 10> A(MatrixS<10, 10>::Zero());
    for (int i = 0; i < points.rows(); ++i) {
        bcg_scalar_t x = points(i, 0);
        bcg_scalar_t y = points(i, 1);
        bcg_scalar_t z = points(i, 2);
        bcg_scalar_t x2 = x * x;
        bcg_scalar_t y2 = y * y;
        bcg_scalar_t z2 = z * z;
        bcg_scalar_t xy = x * y;
        bcg_scalar_t xz = x * z;
        bcg_scalar_t yz = y * z;
        bcg_scalar_t x3 = x * x2;
        bcg_scalar_t xy2 = x * y2;
        bcg_scalar_t xz2 = x * z2;
        bcg_scalar_t x2y = x * xy;
        bcg_scalar_t x2z = x * xz;
        bcg_scalar_t xyz = x * yz;
        bcg_scalar_t y3 = y * y2;
        bcg_scalar_t yz2 = y * z2;
        bcg_scalar_t y2z = y * yz;
        bcg_scalar_t z3 = z * z2;
        bcg_scalar_t x4 = x * x3;
        bcg_scalar_t x2y2 = x * xy2;
        bcg_scalar_t x2z2 = x * xz2;
        bcg_scalar_t x3y = x * x2y;
        bcg_scalar_t x3z = x * x2z;
        bcg_scalar_t x2yz = x * xyz;
        bcg_scalar_t y4 = y * y3;
        bcg_scalar_t y2z2 = y * yz2;
        bcg_scalar_t xy3 = x * y3;
        bcg_scalar_t xy2z = x * y2z;
        bcg_scalar_t y3z = y * y2z;
        bcg_scalar_t z4 = z * z3;
        bcg_scalar_t xyz2 = x * yz2;
        bcg_scalar_t xz3 = x * z3;
        bcg_scalar_t yz3 = y * z3;

        // M(0, 0) += 1
        A(0, 1) += x;
        A(0, 2) += y;
        A(0, 3) += z;
        A(0, 4) += x2;
        A(0, 5) += xy;
        A(0, 6) += xz;
        A(0, 7) += y2;
        A(0, 8) += yz;
        A(0, 9) += z2;

        // M(1, 1) += x2    [M(0,4)]
        // M(1, 2) += xy    [M(0,5)]
        // M(1, 3) += xz    [M(0,6)]
        A(1, 4) += x3;
        A(1, 5) += x2y;
        A(1, 6) += x2z;
        A(1, 7) += xy2;
        A(1, 8) += xyz;
        A(1, 9) += xz2;

        // M(2, 2) += y2    [M(0,7)]
        // M(2, 3) += yz    [M(0,8)]
        // M(2, 4) += x2y   [M(1,5)]
        A(2, 5) += xy2;
        // M(2, 6) += xyz   [M(1,8)]
        A(2, 7) += y3;
        A(2, 8) += y2z;
        A(2, 9) += yz2;

        // M(3, 3) += z2    [M(0,9)]
        // M(3, 4) += x2z   [M(1,6)]
        // M(3, 5) += xyz   [M(1,8)]
        // M(3, 6) += xz2   [M(1,9)]
        // M(3, 7) += y2z   [M(2,8)]
        // M(3, 8) += yz2   [M(2,9)]
        A(3, 9) += z3;

        A(4, 4) += x4;
        A(4, 5) += x3y;
        A(4, 6) += x3z;
        A(4, 7) += x2y2;
        A(4, 8) += x2yz;
        A(4, 9) += x2z2;

        // M(5, 5) += x2y2  [M(4,7)]
        // M(5, 6) += x2yz  [M(4,8)]
        A(5, 7) += xy3;
        A(5, 8) += xy2z;
        A(5, 9) += xyz2;

        // M(6, 6) += x2z2  [M(4,9)]
        // M(6, 7) += xy2z  [M(5,8)]
        // M(6, 8) += xyz2  [M(5,9)]
        A(6, 9) += xz3;

        A(7, 7) += y4;
        A(7, 8) += y3z;
        A(7, 9) += y2z2;

        // M(8, 8) += y2z2  [M(7,9)]
        A(8, 9) += yz3;

        A(9, 9) += z4;
    }

    bcg_scalar_t const rNumPoints = static_cast<bcg_scalar_t>(points.rows());
    A(0, 0) = rNumPoints;
    A(1, 1) = A(0, 4);  // x2
    A(1, 2) = A(0, 5);  // xy
    A(1, 3) = A(0, 6);  // xz
    A(2, 2) = A(0, 7);  // y2
    A(2, 3) = A(0, 8);  // yz
    A(2, 4) = A(1, 5);  // x2y
    A(2, 6) = A(1, 8);  // xyz
    A(3, 3) = A(0, 9);  // z2
    A(3, 4) = A(1, 6);  // x2z
    A(3, 5) = A(1, 8);  // xyz
    A(3, 6) = A(1, 9);  // xz2
    A(3, 7) = A(2, 8);  // y2z
    A(3, 8) = A(2, 9);  // yz2
    A(5, 5) = A(4, 7);  // x2y2
    A(5, 6) = A(4, 8);  // x2yz
    A(6, 6) = A(4, 9);  // x2z2
    A(6, 7) = A(5, 8);  // xy2z
    A(6, 8) = A(5, 9);  // xyz2
    A(8, 8) = A(7, 9);  // y2z2

    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < row; ++col) {
            A(row, col) = A(col, row);
        }
    }

    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            A(row, col) /= rNumPoints;
        }
    }

    Eigen::SelfAdjointEigenSolver<MatrixS<10, 10>> solver(A);
    if (solver.info() == Eigen::Success) {
        VectorS<10> C = solver.eigenvectors().col(0);
        //c = C[0],
        //B = (C[1],C[2],C[3]), and A is a 3x3 symmetric matrix with
        //A00 = C[4], A01 = C[5]/2, A02 = C[6]/2, A11 = C[7], A12 = C[8]/2, A22 = C[9]
        //Q = quadric::from_coefficients(C[4], C[5] / 2, C[6] / 2, C[7], C[8] / 2, C[9], C[1], C[2], C[3], C[0]);
        Q = quadric::from_coefficients(C[4], C[5] / 2, C[6] / 2, C[7], C[8] / 2, C[9], C[1], C[2], C[3], C[0]);
        VectorS<3> p = points.colwise().mean();
        Q += quadric::probabilistic_plane_quadric(p, Q.normal(p), 0.1, 0.1);
    } else {
        Q = quadric::point_quadric(points.colwise().mean());
    }
    return std::max(solver.eigenvalues()(0), 0.0);
}

VectorS<3> quadric_minimizer(const quadric &Q) {
    MatrixS<3, 3> A = Q.A();
    VectorS<3> b = Q.b();
    return A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV).solve(b);
}

}