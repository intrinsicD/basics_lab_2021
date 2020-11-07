//
// Created by alex on 15.10.20.
//

#ifndef BCG_GRAPHICS_BCG_DISTANCE_ALIGNED_BOX_POINT_H
#define BCG_GRAPHICS_BCG_DISTANCE_ALIGNED_BOX_POINT_H

#include "bcg_distance_query.h"
#include "aligned_box/bcg_aligned_box.h"

namespace bcg {

template<bcg_index_t N>
struct distance_query<VectorS<N>, aligned_box<N>> {
    struct result {
        bcg_scalar_t distance, sqr_distance;
        VectorS<N> closest;
    };

    result operator()(const VectorS<N> &point, const aligned_box<N> &box) const {
        VectorS<N> box_center, box_extent;
        box.get_centered_form(box_center, box_extent);
        return do_query(point - box_center, box_center, box_extent);
    }

protected:
    result do_query(const VectorS<N> &point,
                   const VectorS<N> &box_center,
                   const VectorS<N> &box_extent) const {
        result res{0, 0, point};
        for (int i = 0; i < N; ++i) {
            if (res.closest[i] < -box_extent[i]) {
                bcg_scalar_t delta = res.closest[i] + box_extent[i];
                res.sqr_distance += delta * delta;
                res.closest[i] = -box_extent[i];
            } else if (res.closest[i] > box_extent[i]) {
                bcg_scalar_t delta = res.closest[i] - box_extent[i];
                res.sqr_distance += delta * delta;
                res.closest[i] = box_extent[i];
            }
        }
        res.distance = std::sqrt(res.sqr_distance);
        res.closest += box_center;
        return res;
    }
};
}

#endif //BCG_GRAPHICS_BCG_DISTANCE_ALIGNED_BOX_POINT_H
