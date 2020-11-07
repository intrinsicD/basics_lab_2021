//
// Created by alex on 07.11.20.
//

#ifndef BCG_GRAPHICS_BCG_DISTANCE_SEGMENT_POINT_H
#define BCG_GRAPHICS_BCG_DISTANCE_SEGMENT_POINT_H

#include "segment/bcg_segment.h"

namespace bcg {

template<bcg_index_t N>
struct distance_point_segment {
    struct result {
        bcg_scalar_t distance, sqr_distance;
        VectorS<N> closest;
        bcg_scalar_t t;
    };

    result operator()(const VectorS<N> &point, const segment<N> &seg) const {
        result res;

        VectorS<N> direction = seg.points[1] - seg.points[0];
        VectorS<N> diff = point - seg.points[1];
        auto t = direction.dot(diff);
        if (t >= 0) {
            res.t = 1;
            res.closest = seg.points[1];
        } else {
            diff = point - seg.points[0];
            t = direction.dot(diff);
            if (t <= 0) {
                res.t = 0;
                res.closest = seg.points[0];
            } else {
                auto sqr_length = direction.dot(direction);
                if (sqr_length > 0) {
                    t /= sqr_length;
                    res.t = t;
                    res.closest = seg.points[0] + t * direction;
                } else {
                    res.t = 0;
                    res.closest = seg.points[0];
                }
            }
        }
        diff = point - res.closest;
        res.sqr_distance = diff.dot(diff);
        res.distance = std::sqrt(res.sqr_distance);

        return res;
    }
};

using distance_point2_segment2 = distance_point_segment<2>;

using distance_point3_segment3 = distance_point_segment<3>;
}

#endif //BCG_GRAPHICS_BCG_DISTANCE_SEGMENT_POINT_H
