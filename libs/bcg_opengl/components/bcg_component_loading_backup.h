//
// Created by alex on 22.06.21.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_LOADING_BACKUP_H
#define BCG_GRAPHICS_BCG_COMPONENT_LOADING_BACKUP_H

#include "bcg_component_transform_object_space.h"
#include "bcg_component_transform_world_space.h"
#include "aligned_box/bcg_aligned_box.h"

namespace bcg {

struct loading_backup {
    aligned_box3 aabb;
    world_space_transform ws_model;
    object_space_transform os_model;
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_LOADING_BACKUP_H
