//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_CORRESPONDENCE_SYSTEM_H
#define BCG_GRAPHICS_BCG_CORRESPONDENCE_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct correspondence_system : public system {
    correspondence_system(viewer_state *state);

    void on_correspondences_estiamte(const event::correspondences::estimate &event);

    void on_correspondences_estiamte_and_filter(const event::correspondences::estimate_and_filter &event);

    void on_correspondences_filter_distance(const event::correspondences::filter::distance &event);

    void on_correspondences_filter_normal_angle(const event::correspondences::filter::normal_angle &event);
};

}
#endif //BCG_GRAPHICS_BCG_CORRESPONDENCE_SYSTEM_H
