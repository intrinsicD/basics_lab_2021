//
// Created by alex on 15.06.21.
//

#ifndef BCG_GRAPHICS_BCG_ORTHODONTIC_SYSTEM_H
#define BCG_GRAPHICS_BCG_ORTHODONTIC_SYSTEM_H

#include "../events/bcg_events_orthodontic.h"
#include "bcg_systems.h"


struct orthodontic_system : public system {
    explicit orthodontic_system(viewer_state *state);

};

}

#endif //BCG_GRAPHICS_BCG_ORTHODONTIC_SYSTEM_H
