//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_SYSTEM_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_SYSTEM_H

#include "bcg_systems.h"

namespace bcg{

struct point_cloud_system : public system{
    explicit point_cloud_system(viewer_state *state);

    void on_setup(const event::point_cloud::setup &event);

    void on_build_graph(const event::point_cloud::build_graph &event);
};

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_SYSTEM_H
