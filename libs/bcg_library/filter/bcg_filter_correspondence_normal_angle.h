//
// Created by alex on 20.05.21.
//

#ifndef BCG_GRAPHICS_BCG_FILTER_CORRESPONDENCE_NORMAL_ANGLE_H
#define BCG_GRAPHICS_BCG_FILTER_CORRESPONDENCE_NORMAL_ANGLE_H

#include "bcg_filter.h"

namespace bcg {

struct filter_correspondences_normal_angle_less : public filter_matrix<MatrixS<-1, 3>> {
    filter_correspondences_normal_angle_less() : filter_matrix([](const VectorRef &row1,
                                                                  const VectorRef &row2,
                                                                  Scalar threshold) {
        return row1.dot(row2) < threshold;
    }) {}

    ~filter_correspondences_normal_angle_less() override = default;
};

struct filter_correspondences_normal_angle_less_equal : public filter_matrix<MatrixS<-1, 3>> {
    filter_correspondences_normal_angle_less_equal() : filter_matrix([](const VectorRef &row1,
                                                                        const VectorRef &row2,
                                                                        Scalar threshold) {
        return row1.dot(row2) <= threshold;
    }) {}

    ~filter_correspondences_normal_angle_less_equal() override = default;
};

struct filter_correspondences_normal_angle_greater_equal : public filter_matrix<MatrixS<-1, 3>> {
    filter_correspondences_normal_angle_greater_equal() : filter_matrix([](const VectorRef &row1,
                                                                           const VectorRef &row2,
                                                                           Scalar threshold) {
        return row1.dot(row2) >= threshold;
    }) {}

    ~filter_correspondences_normal_angle_greater_equal() override = default;
};

struct filter_correspondences_normal_angle_greater : public filter_matrix<MatrixS<-1, 3>> {
    filter_correspondences_normal_angle_greater() : filter_matrix([](const VectorRef &row1,
                                                                     const VectorRef &row2,
                                                                     Scalar threshold) {
        return row1.dot(row2) > threshold;
    }) {}

    ~filter_correspondences_normal_angle_greater() override = default;
};


}

#endif //BCG_GRAPHICS_BCG_FILTER_CORRESPONDENCE_NORMAL_ANGLE_H
