//
// Created by alex on 29.03.21.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_HEIGHT_FIELD_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_HEIGHT_FIELD_H

#include "bcg_point_cloud.h"

namespace bcg {

void point_cloud_height_field(vertex_container *vertices, int dim);

void
point_cloud_height_field(vertex_container *vertices, property<bcg_scalar_t, 1> weighting, bcg_scalar_t alpha, int dim, bool invert);

}

#endif //BCG_GRAPHICS_BCG_GUI_POINT_CLOUD_HEIGHT_FIELD_H
