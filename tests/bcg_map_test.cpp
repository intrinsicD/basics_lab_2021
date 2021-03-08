//
// Created by alex on 12.10.20.
//

#include <gtest/gtest.h>

#include "bcg_library/math/bcg_linalg.h"

using namespace bcg;

TEST(TestSuiteMap, test){
    MatrixS<-1, 3> M(3, 3);
    M << 0, 1, 2, 3, 4, 5, 6, 7, 8;
    std::cout << M << "\n\n";
    std::cout << Eigen::Map<const VectorS<-1>>(M.data(), M.size()) << "\n";
}