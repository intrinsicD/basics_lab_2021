//
// Created by alex on 16.11.20.
//

#include "bcg_gui_rendering_options.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_materials.h"
#include "bcg_gui_ogl_shape.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"
#include "renderers/point_rendering/hq_pbr_ufsi/bcg_hq_pbr_ufsi_renderer_events.h"
#include "renderers/graph_renderer/bcg_events_graph_renderer.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/vectorfield_renderer/bcg_events_vectorfield_renderer.h"
#include "renderers/curve_renderer/bcg_events_curve_renderer.h"
#include "renderers/deferred_renderer/bcg_events_deferred_renderer.h"

namespace bcg {

void gui_rendering_options(viewer_state *state) {
    gui_rendering_options(state, state->picker.entity_id);
}

void gui_rendering_options(viewer_state *state, entt::entity id) {
    if (!state->scene.valid(id)) return;

    bool show_points = state->scene.has<event::points_renderer::enqueue>(id);
    bool show_hq_points = state->scene.has<event::hq_pbr_ufsi_renderer::enqueue>(id);
    bool show_edges = state->scene.has<event::graph_renderer::enqueue>(id);
    bool show_mesh = state->scene.has<event::mesh_renderer::enqueue>(id);
    bool show_vectors = state->scene.has<event::vectorfield_renderer::enqueue>(id);
    bool show_curves = state->scene.has<event::curve_renderer::enqueue>(id);
    bool render_deferred = state->scene.has<event::deferred_renderer::enqueue>(id);

    if (ImGui::Checkbox("Show Points", &show_points)) {
        if (show_points) {
            state->scene().emplace_or_replace<event::points_renderer::enqueue>(id);
        } else {
            state->scene().remove_if_exists<event::points_renderer::enqueue>(id);
        }
    }
    if (ImGui::Checkbox("Show HQ_Points", &show_hq_points)) {
        if (show_hq_points) {
            state->scene().emplace_or_replace<event::hq_pbr_ufsi_renderer::enqueue>(id);
        } else {
            state->scene().remove_if_exists<event::hq_pbr_ufsi_renderer::enqueue>(id);
        }
    }
    if (ImGui::Checkbox("Show Edges", &show_edges)) {
        if (show_edges && state->get_edges(id) != nullptr) {
            state->scene().emplace_or_replace<event::graph_renderer::enqueue>(id);
        } else {
            state->scene().remove_if_exists<event::graph_renderer::enqueue>(id);
        }
    }
    if (ImGui::Checkbox("Show Mesh", &show_mesh)) {
        if (show_mesh && state->get_faces(id) != nullptr) {
            state->scene().emplace_or_replace<event::mesh_renderer::enqueue>(id);
        } else {
            state->scene().remove_if_exists<event::mesh_renderer::enqueue>(id);
        }
    }
    if (ImGui::Checkbox("Show Vectors", &show_vectors)) {
        if (show_vectors) {
            state->scene().emplace_or_replace<event::vectorfield_renderer::enqueue>(id);
        } else {
            state->scene().remove_if_exists<event::vectorfield_renderer::enqueue>(id);
        }
    }
    if (ImGui::Checkbox("Show Curves", &show_curves)) {
        if (show_curves) {
            state->scene().emplace_or_replace<event::curve_renderer::enqueue>(id);
        } else {
            state->scene().remove_if_exists<event::curve_renderer::enqueue>(id);
        }
    }
    if (ImGui::Checkbox("Render Deferred", &render_deferred)) {
        if (render_deferred) {
            state->scene().emplace_or_replace<event::deferred_renderer::enqueue>(id);
        } else {
            state->scene().remove_if_exists<event::deferred_renderer::enqueue>(id);
        }
    }
    gui_materials(state, id, show_points, show_hq_points, show_edges, show_mesh, show_vectors, show_curves);
    if(ImGui::CollapsingHeader("Buffers")){
        gui_ogl_shape(state, id);
    }
}

}