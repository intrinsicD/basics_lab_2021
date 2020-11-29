//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_PICKER_SYSTEM_H
#define BCG_GRAPHICS_BCG_PICKER_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct picker_system : public system {
    explicit picker_system(viewer_state *state);

    void on_enable_point(const event::picker::enable::point &event);

    void on_enable_vertex(const event::picker::enable::vertex &event);

    void on_enable_edge(const event::picker::enable::edge &event);

    void on_enable_face(const event::picker::enable::face &event);

    void on_disable(const event::picker::disable &event);

    void on_pick_point(const event::picker::pick::point &event);

    void on_pick_vertex(const event::picker::pick::vertex &event);

    void on_pick_edge(const event::picker::pick::edge &event);

    void on_pick_face(const event::picker::pick::face &event);
};

}

#endif //BCG_GRAPHICS_BCG_PICKER_SYSTEM_H
