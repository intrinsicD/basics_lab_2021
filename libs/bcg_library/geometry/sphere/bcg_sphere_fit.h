//
// Created by alex on 15.10.20.
//

#ifndef BCG_GRAPHICS_BCG_SPHERE_FIT_H
#define BCG_GRAPHICS_BCG_SPHERE_FIT_H

#include <vector>
#include "bcg_sphere.h"

namespace bcg {

bool FitUsingSquaredLengths(int numPoints, const VectorS<3> *points, sphere3 &sphere) {
    // Compute the average of the data points.
    bcg_scalar_t const zero(0);
    VectorS<3> A = {zero, zero, zero};
    for (int i = 0; i < numPoints; ++i) {
        A += points[i];
    }
    bcg_scalar_t invNumPoints = ((bcg_scalar_t) 1) / static_cast<bcg_scalar_t>(numPoints);
    A *= invNumPoints;

    // Compute the covariance matrix M of the Y[i] = X[i]-A and the
    // right-hand side R of the linear system M*(C-A) = R.
    bcg_scalar_t M00 = zero, M01 = zero, M02 = zero, M11 = zero, M12 = zero, M22 = zero;
    VectorS<3> R = {zero, zero, zero};
    for (int i = 0; i < numPoints; ++i) {
        VectorS<3> Y = points[i] - A;
        bcg_scalar_t Y0Y0 = Y[0] * Y[0];
        bcg_scalar_t Y0Y1 = Y[0] * Y[1];
        bcg_scalar_t Y0Y2 = Y[0] * Y[2];
        bcg_scalar_t Y1Y1 = Y[1] * Y[1];
        bcg_scalar_t Y1Y2 = Y[1] * Y[2];
        bcg_scalar_t Y2Y2 = Y[2] * Y[2];
        M00 += Y0Y0;
        M01 += Y0Y1;
        M02 += Y0Y2;
        M11 += Y1Y1;
        M12 += Y1Y2;
        M22 += Y2Y2;
        R += (Y0Y0 + Y1Y1 + Y2Y2) * Y;
    }
    R *= (bcg_scalar_t) 0.5;

    // Solve the linear system M*(C-A) = R for the center C.
    bcg_scalar_t cof00 = M11 * M22 - M12 * M12;
    bcg_scalar_t cof01 = M02 * M12 - M01 * M22;
    bcg_scalar_t cof02 = M01 * M12 - M02 * M11;
    bcg_scalar_t det = M00 * cof00 + M01 * cof01 + M02 * cof02;
    if (det != zero) {
        bcg_scalar_t cof11 = M00 * M22 - M02 * M02;
        bcg_scalar_t cof12 = M01 * M02 - M00 * M12;
        bcg_scalar_t cof22 = M00 * M11 - M01 * M01;
        sphere.center[0] = A[0] + (cof00 * R[0] + cof01 * R[1] + cof02 * R[2]) / det;
        sphere.center[1] = A[1] + (cof01 * R[0] + cof11 * R[1] + cof12 * R[2]) / det;
        sphere.center[2] = A[2] + (cof02 * R[0] + cof12 * R[1] + cof22 * R[2]) / det;
        bcg_scalar_t rsqr = zero;
        for (int i = 0; i < numPoints; ++i) {
            VectorS<3> delta = points[i] - sphere.center;
            rsqr += delta.dot(delta);
        }
        rsqr *= invNumPoints;
        sphere.radius = std::sqrt(rsqr);
        return true;
    } else {
        sphere.center = {zero, zero, zero};
        sphere.radius = zero;
        return false;
    }
}

unsigned int FitUsingLengths(int numPoints, VectorS<3> const *points,
                             unsigned int maxIterations, bool initialCenterIsAverage,
                             sphere3 &sphere, bcg_scalar_t epsilon = (bcg_scalar_t) 0) {
    // Compute the average of the data points.
    VectorS<3> average = points[0];
    for (int i = 1; i < numPoints; ++i) {
        average += points[i];
    }
    bcg_scalar_t invNumPoints = ((bcg_scalar_t) 1) / static_cast<bcg_scalar_t>(numPoints);
    average *= invNumPoints;

    // The initial guess for the center.
    if (initialCenterIsAverage) {
        sphere.center = average;
    }

    bcg_scalar_t epsilonSqr = epsilon * epsilon;
    unsigned int iteration;
    for (iteration = 0; iteration < maxIterations; ++iteration) {
        // Update the iterates.
        VectorS<3> current = sphere.center;

        // Compute average L, dL/da, dL/db, dL/dc.
        bcg_scalar_t lenAverage = (bcg_scalar_t) 0;
        VectorS<3> derLenAverage = VectorS<3>::Zero();
        for (int i = 0; i < numPoints; ++i) {
            VectorS<3> diff = points[i] - sphere.center;
            bcg_scalar_t length = diff.norm();
            if (length > (bcg_scalar_t) 0) {
                lenAverage += length;
                bcg_scalar_t invLength = ((bcg_scalar_t) 1) / length;
                derLenAverage -= invLength * diff;
            }
        }
        lenAverage *= invNumPoints;
        derLenAverage *= invNumPoints;

        sphere.center = average + lenAverage * derLenAverage;
        sphere.radius = lenAverage;

        VectorS<3> diff = sphere.center - current;
        bcg_scalar_t diffSqrLen = diff.dot(diff);
        if (diffSqrLen <= epsilonSqr) {
            break;
        }
    }

    return ++iteration;
}

}
#endif //BCG_GRAPHICS_BCG_SPHERE_FIT_H
