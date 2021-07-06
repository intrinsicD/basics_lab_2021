//
// Created by alex on 22.06.21.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_LOADING_BACKUP_H
#define BCG_GRAPHICS_BCG_COMPONENT_LOADING_BACKUP_H

#include "bcg_component_transform.h"
#include "aligned_box/bcg_aligned_box.h"

namespace bcg {

struct loading_backup {
    aligned_box3 aabb;
    component_transform loading_model;
    component_transform ws_model;
    bcg_scalar_t scale;
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_LOADING_BACKUP_H
