//
// Created by alex on 24.06.21.
//

#include "bcg_gui_components_geometry.h"
#include "geometry/curve/bcg_curve_bezier.h"
#include "bcg_gui_mesh.h"
#include "bcg_gui_graph.h"
#include "bcg_gui_point_cloud.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg {

void gui_components_geometry(viewer_state *state){
    gui_components_geometry(state, state->picker.entity_id);
}

void gui_components_geometry(viewer_state *state, entt::entity id){
    if(!state->scene.valid(id)) return;

    if (state->scene.has<halfedge_mesh>(id) && ImGui::CollapsingHeader("Mesh")){
        gui_mesh(state, id);
        ImGui::Separator();
    }
    if (state->scene.has<halfedge_graph>(id) && ImGui::CollapsingHeader("Graph")){
        gui_graph(state, id);
        ImGui::Separator();
    }
    if (state->scene.has<halfedge_graph>(id) && ImGui::CollapsingHeader("Graph")){
        gui_graph(state, id);
        ImGui::Separator();
    }
    if (state->scene.has<curve_bezier>(id) && ImGui::CollapsingHeader("Curve")){
        auto &curve = state->scene.get<curve_bezier>(id);
        gui_graph(state, curve);
        ImGui::Separator();
    }
    if (state->scene.has<point_cloud>(id) && ImGui::CollapsingHeader("Point Cloud")) {
        gui_point_cloud(state, id);
        ImGui::Separator();
    }
}

}