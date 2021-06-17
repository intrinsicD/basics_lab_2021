//
// Created by alex on 17.06.21.
//

#ifndef BCG_GRAPHICS_BCG_OBJECT_SPACE_SYSTEM_H
#define BCG_GRAPHICS_BCG_OBJECT_SPACE_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct object_space_system : public system {
    explicit object_space_system(viewer_state *state);

    void on_add_object_space_transform(const event::object_space::add_component_object_space_transform &event);

    void on_set_object_space_transform(const event::object_space::set_component_object_space_transform &event);

    void on_object_space_translate_aabb_center_to_origin(const event::object_space::translate_aabb_center_to_origin &event);

    void on_object_space_uniform_scale_to_unit_cube(const event::object_space::uniform_scale_to_unit_cube &event);

};

}
#endif //BCG_GRAPHICS_BCG_OBJECT_SPACE_SYSTEM_H
