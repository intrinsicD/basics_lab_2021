//
// Created by alex on 01.04.21.
//

#ifndef BCG_GRAPHICS_BCG_PROPERTY_UTILS_H
#define BCG_GRAPHICS_BCG_PROPERTY_UTILS_H

#include "point_cloud/bcg_point_cloud.h"

namespace bcg {

property<bcg_scalar_t, 1> invert(property<bcg_scalar_t, 1> property);

property<bcg_scalar_t, 1> abs(property<bcg_scalar_t, 1> property);

property<bcg_scalar_t, 1> gaussian(property<bcg_scalar_t, 1> property, bcg_scalar_t sigma);

property<bcg_scalar_t, 1> shift(property<bcg_scalar_t, 1> property, bcg_scalar_t value);

property<bcg_scalar_t, 1> scale(property<bcg_scalar_t, 1> property, bcg_scalar_t value);

property<bcg_scalar_t, 1> sum(property<bcg_scalar_t, 1> property1, property<bcg_scalar_t, 1> property2);

property<bcg_scalar_t, 1> prod(property<bcg_scalar_t, 1> property1, property<bcg_scalar_t, 1> property2);

property<VectorS<3>, 3>  invert(property<VectorS<3>, 3> property);

property<VectorS<3>, 3>  abs(property<VectorS<3>, 3>  property);

property<VectorS<3>, 3>  shift(property<VectorS<3>, 3>  property, const VectorS<3> &value);

property<VectorS<3>, 3>  scale(property<VectorS<3>, 3>  property, const VectorS<3> &value);

property<VectorS<3>, 3> sum(property<VectorS<3>, 3> property1, property<VectorS<3>, 3> property2);

property<VectorS<3>, 3> prod(property<VectorS<3>, 3> property1, property<VectorS<3>, 3> property2);

}

#endif //BCG_GRAPHICS_BCG_PROPERTY_UTILS_H
