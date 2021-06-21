//
// Created by alex on 21.06.21.
//

#ifndef BCG_GRAPHICS_BCG_SYSTEM_WORLD_SPACE_TRANSFORM_H
#define BCG_GRAPHICS_BCG_SYSTEM_WORLD_SPACE_TRANSFORM_H

#include "bcg_systems.h"

namespace bcg {

struct system_world_space_transform : public system {
    explicit system_world_space_transform(viewer_state *state);

    ~system_world_space_transform() override;

    void init(const event::transform::world_space::init &event);

    void remove(const event::transform::world_space::remove &event);

    void set(const event::transform::world_space::set &event);

    void set_identity(const event::transform::world_space::set_identity &event);

    void pre_translate(const event::transform::world_space::pre_translate &event);

    void pre_scale(const event::transform::world_space::pre_scale &event);

    void pre_uniform_scale(const event::transform::world_space::pre_uniform_scale &event);

    void pre_rotate(const event::transform::world_space::pre_rotate &event);

    void post_translate(const event::transform::world_space::post_translate &event);

    void post_scale(const event::transform::world_space::post_scale &event);

    void post_uniform_scale(const event::transform::world_space::post_uniform_scale &event);

    void post_rotate(const event::transform::world_space::post_rotate &event);

    void render_gui(const event::transform::world_space::render_gui &event);
};

}

#endif //BCG_GRAPHICS_BCG_SYSTEM_WORLD_SPACE_TRANSFORM_H
