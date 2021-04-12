//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_ENTITY_INFO_H
#define BCG_GRAPHICS_BCG_ENTITY_INFO_H

#include <string>
#include "math/bcg_linalg.h"

namespace bcg{

struct entity_info{
    std::string filename, entity_name;
    VectorS<3> loading_center;
    bcg_scalar_t loading_scale;
    bool save = false;
};

}

#endif //BCG_GRAPHICS_BCG_ENTITY_INFO_H
