//
// Created by alex on 16.11.20.
//

#include "bcg_gui_materials.h"
#include "bcg_gui_material_points.h"
#include "bcg_gui_material_graph.h"
#include "bcg_gui_material_mesh.h"
#include "bcg_gui_material_curve.h"
#include "bcg_gui_vectorfields.h"
#include "bcg_viewer_state.h"

namespace bcg{

void gui_materials(viewer_state *state, entt::entity id,
                   bool show_points, bool show_edges, bool show_meshes, bool show_vectors, bool show_curves){
    if (ImGui::CollapsingHeader("materials")) {
        if (show_points) {
            ImGui::Separator();
            gui_material_points(state, state->scene.try_get<material_points>(id), id);
        }
        if (show_edges) {
            ImGui::Separator();
            gui_material_graph(state, state->scene.try_get<material_graph>(id), id);
        }
        if (show_meshes) {
            ImGui::Separator();
            gui_material_mesh(state, state->scene.try_get<material_mesh>(id), id);
        }
        if (show_vectors) {
            ImGui::Separator();
            gui_vectorfields(state, state->scene.try_get<vectorfields>(id), id);
        }
        if (show_curves) {
            ImGui::Separator();
            gui_material_curve(state, state->scene.try_get<material_curve>(id), id);
        }
    }
}

}