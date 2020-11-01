//
// Created by alex on 29.10.20.
//

#ifndef BCG_GRAPHICS_BCG_GPU_SYSTEM_H
#define BCG_GRAPHICS_BCG_GPU_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct gpu_system : public system {
    explicit gpu_system(viewer_state *state);

    void on_update_vertex_attributes(const event::gpu::update_vertex_attributes &event);

    void on_update_edge_attributes(const event::gpu::update_edge_attributes &event);

    void on_update_face_attributes(const event::gpu::update_face_attributes &event);
};

}

#endif //BCG_GRAPHICS_BCG_GPU_SYSTEM_H
