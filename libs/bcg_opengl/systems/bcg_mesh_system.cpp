//
// Created by alex on 28.10.20.
//

#include "bcg_mesh_system.h"
#include "bcg_viewer_state.h"
#include "bcg_entity_info.h"
#include "bcg_property_map_eigen.h"
#include "geometry/aligned_box/bcg_aligned_box.h"

namespace bcg{

mesh_system::mesh_system(viewer_state *state) : system("mesh_system", state){
    state->dispatcher.sink<event::mesh::setup>().connect<&mesh_system::on_setup_mesh>(this);
}

void mesh_system::on_setup_mesh(const event::mesh::setup &event) {
    auto &mesh = state->scene.get<halfedge_mesh>(event.id);

    state->dispatcher.trigger<event::transform::add>(event.id);

    aligned_box3 aabb(mesh.positions.vector());
    state->scene.emplace<entity_info>(event.id, event.filename, "mesh", aabb.center(), aabb.halfextent().maxCoeff());

    Map(mesh.positions) = (MapConst(mesh.positions).rowwise() - aabb.center().transpose()) / aabb.halfextent().maxCoeff();
    state->dispatcher.trigger<event::aligned_box::add>(event.id);
}

}