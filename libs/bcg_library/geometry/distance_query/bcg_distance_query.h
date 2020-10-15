//
// Created by alex on 15.10.20.
//

#ifndef BCG_GRAPHICS_BCG_DISTANCE_QUERY_H
#define BCG_GRAPHICS_BCG_DISTANCE_QUERY_H

namespace bcg {
// Distance and closest-point queries.
template<typename Type0, typename Type1>
struct distance_query {
    struct result {

    };

    result operator()(const Type0 &primitive0, const Type1 &primitive1) const;
};
}

#endif //BCG_GRAPHICS_BCG_DISTANCE_QUERY_H
