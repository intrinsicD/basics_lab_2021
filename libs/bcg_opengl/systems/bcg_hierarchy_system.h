//
// Created by alex on 19.11.20.
//

#ifndef BCG_GRAPHICS_BCG_HIERARCHY_SYSTEM_H
#define BCG_GRAPHICS_BCG_HIERARCHY_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct hierarchy_system : public system {
    explicit hierarchy_system(viewer_state *state);

    void on_set_parent(const event::hierarchy::set_parent &event);

    void on_add_child(const event::hierarchy::add_child &event);

    void on_remove_child(const event::hierarchy::remove_child &event);

    void on_update(const event::internal::update &event);

    void update_accumulated_model(entt::entity id);
};

}


#endif //BCG_GRAPHICS_BCG_HIERARCHY_SYSTEM_H
