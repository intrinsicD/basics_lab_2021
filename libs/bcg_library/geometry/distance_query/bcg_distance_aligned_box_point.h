//
// Created by alex on 15.10.20.
//

#ifndef BCG_GRAPHICS_BCG_DISTANCE_ALIGNED_BOX_POINT_H
#define BCG_GRAPHICS_BCG_DISTANCE_ALIGNED_BOX_POINT_H

#include "bcg_distance_query.h"
#include "../aligned_box/bcg_aligned_box.h"

namespace bcg {

template<bcg_index_t N>
struct distance_query<VectorS<N>, aligned_box<N>> {
    struct result {
        bcg_scalar_t distance, sqrDistance;
        VectorS<N> boxClosest;
    };

    result operator()(const VectorS<N> &point, const aligned_box<N> &box) const {
        VectorS<N> boxCenter, boxExtent;
        box.get_centered_form(boxCenter, boxExtent);
        return do_query(point - boxCenter, boxCenter, boxExtent);
    }

protected:
    result do_query(const VectorS<N> &point,
                   const VectorS<N> &boxCenter,
                   const VectorS<N> &boxExtent) const {
        result res{0, 0, point};
        for (int i = 0; i < N; ++i) {
            if (res.boxClosest[i] < -boxExtent[i]) {
                bcg_scalar_t delta = res.boxClosest[i] + boxExtent[i];
                res.sqrDistance += delta * delta;
                res.boxClosest[i] = -boxExtent[i];
            } else if (res.boxClosest[i] > boxExtent[i]) {
                bcg_scalar_t delta = res.boxClosest[i] - boxExtent[i];
                res.sqrDistance += delta * delta;
                res.boxClosest[i] = boxExtent[i];
            }
        }
        res.distance = std::sqrt(res.sqrDistance);
        res.boxClosest += boxCenter;
        return res;
    }
};
}

#endif //BCG_GRAPHICS_BCG_DISTANCE_ALIGNED_BOX_POINT_H
