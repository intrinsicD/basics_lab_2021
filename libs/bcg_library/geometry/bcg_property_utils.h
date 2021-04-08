//
// Created by alex on 01.04.21.
//

#ifndef BCG_GRAPHICS_BCG_PROPERTY_UTILS_H
#define BCG_GRAPHICS_BCG_PROPERTY_UTILS_H

#include "point_cloud/bcg_point_cloud.h"

namespace bcg {

property<bcg_scalar_t, 1> invert(property_container *container, property<bcg_scalar_t, 1> property);

property<bcg_scalar_t, 1> abs(property_container *container, property<bcg_scalar_t, 1> property);

property<bcg_scalar_t, 1> gaussian(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t sigma);

property<bcg_scalar_t, 1> shift(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t value);

property<bcg_scalar_t, 1> scale(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t value);

property<bcg_scalar_t, 1> sum(property_container *container, property<bcg_scalar_t, 1> property1, property<bcg_scalar_t, 1> property2);

property<bcg_scalar_t, 1> prod(property_container *container, property<bcg_scalar_t, 1> property1, property<bcg_scalar_t, 1> property2);

property<bcg_scalar_t, 1> clamp(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t min, bcg_scalar_t max);

property<bcg_scalar_t, 1> classify_min(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t min);

property<bcg_scalar_t, 1> classify_max(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t min);

property<bcg_scalar_t, 1> clampthreshold_min(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t min);

property<bcg_scalar_t, 1> clampthreshold_max(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t max);

property<VectorS<3>, 3>  invert(property_container *container, property<VectorS<3>, 3> property);

property<VectorS<3>, 3>  abs(property_container *container, property<VectorS<3>, 3>  property);

property<VectorS<3>, 3>  shift(property_container *container, property<VectorS<3>, 3>  property, const VectorS<3> &value);

property<VectorS<3>, 3>  scale(property_container *container, property<VectorS<3>, 3>  property, const VectorS<3> &value);

property<VectorS<3>, 3> sum(property_container *container, property<VectorS<3>, 3> property1, property<VectorS<3>, 3> property2);

property<VectorS<3>, 3> prod(property_container *container, property<VectorS<3>, 3> property1, property<VectorS<3>, 3> property2);

}

#endif //BCG_GRAPHICS_BCG_PROPERTY_UTILS_H
