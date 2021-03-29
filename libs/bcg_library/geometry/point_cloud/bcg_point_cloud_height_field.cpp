//
// Created by alex on 29.03.21.
//

#include "bcg_point_cloud_height_field.h"
#include "bcg_property_map_eigen.h"

namespace bcg {

void point_cloud_height_field(vertex_container *vertices, int dim) {
    point_cloud_height_field(vertices, property<bcg_scalar_t, 1>(), 0, dim, false);
}

void
point_cloud_height_field(vertex_container *vertices, property<bcg_scalar_t, 1> weighting, bcg_scalar_t alpha, int dim,
                         bool invert) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto height_field = vertices->get_or_add<bcg_scalar_t, 1>("v_height_field");
    if (dim < 3) {
        if (weighting) {
            VectorS<-1> weights = MapConst(weighting).array() - MapConst(weighting).minCoeff();
            weights.array() /= weights.maxCoeff();
            Map(height_field) = alpha * MapConst(positions).col(dim).array() +
                                (1 - alpha) * weights.array() * (invert ? -1.0 : 1.0);
        } else {
            Map(height_field) = MapConst(positions).col(dim);
        }
    }
    height_field.set_dirty();
}

}