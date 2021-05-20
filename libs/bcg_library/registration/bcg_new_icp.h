//
// Created by alex on 19.05.21.
//

#ifndef BCG_GRAPHICS_BCG_NEW_ICP_H
#define BCG_GRAPHICS_BCG_NEW_ICP_H

#include "math/bcg_linalg.h"

namespace bcg{

struct PointsSelect{

};

struct CorrespondenceWeighted{

};

struct CorrespondenceTruncated{

};

struct PointsMatched{

};

struct icp{
    Transform point_2_point(const MatrixS<-1, 3> &P, const MatrixS<-1, 3> &Q, const VectorS<-1> &w);
    Transform point_2_plane(const MatrixS<-1, 3> &P, const MatrixS<-1, 3> &Q, const MatrixS<-1, 3> &N, const VectorS<-1> &w);
    Transform point_2_plane_gaussnewton(const MatrixS<-1, 3> &P, const MatrixS<-1, 3> &Q, const MatrixS<-1, 3> &N, const VectorS<-1> &w);
};

}

#endif //BCG_GRAPHICS_BCG_NEW_ICP_H
