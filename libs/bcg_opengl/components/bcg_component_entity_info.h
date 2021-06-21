//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_ENTITY_INFO_H
#define BCG_GRAPHICS_BCG_COMPONENT_ENTITY_INFO_H

#include <string>
#include "math/bcg_linalg.h"
#include "geometry/aligned_box/bcg_aligned_box.h"

namespace bcg{

struct entity_info{
    std::string filename, entity_name;
    Transform loading_model;
    aligned_box3 loading_aabb;
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_ENTITY_INFO_H
