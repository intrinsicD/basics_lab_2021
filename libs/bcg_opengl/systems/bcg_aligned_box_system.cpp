//
// Created by alex on 04.11.20.
//

#include "bcg_aligned_box_system.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_library/geometry/aligned_box/bcg_aligned_box.h"

namespace bcg {

aligned_box_system::aligned_box_system(viewer_state *state) : system("bounds_system", state) {
    state->dispatcher.sink<event::aligned_box::add>().connect<&aligned_box_system::on_add>(this);
    state->dispatcher.sink<event::aligned_box::set>().connect<&aligned_box_system::on_set>(this);
}

void aligned_box_system::on_add(const event::aligned_box::add &event) {
    if (!state->scene.valid(event.id)) return;
    property_container *vertices = state->get_vertices(event.id);
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    state->scene().emplace_or_replace<aligned_box3>(event.id, aligned_box3(positions.vector()));
}

void aligned_box_system::on_set(const event::aligned_box::set &event) {
    if (!state->scene.valid(event.id)) return;
    state->scene().emplace_or_replace<aligned_box3>(event.id, event.aabb);
}

}