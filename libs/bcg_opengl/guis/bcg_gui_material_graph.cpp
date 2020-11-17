//
// Created by alex on 16.11.20.
//

#include "bcg_gui_material_graph.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_property_selector.h"
#include "renderers/graph_renderer/bcg_events_graph_renderer.h"

namespace bcg {

void gui_material_graph(viewer_state *state, material_graph *material, entt::entity id) {
    if (!material) return;
    if (ImGui::CollapsingHeader("graph material")) {
        ImGui::PushID("graph_material");
        auto *vertices = state->get_vertices(id);
        auto *edges = state->get_edges(id);
        auto &position = material->attributes[0];
        if (ImGui::CollapsingHeader("vertex array object")) {
            draw_label(&state->window, (material->vao.name + " vao id").c_str(), std::to_string(material->vao.handle));
            for (const auto &item : material->vao.captured_attributes) {
                draw_label(&state->window, std::to_string(item.first).c_str(), item.second);
            }
        }
        if (gui_property_selector(state, vertices, {3}, ("shader " + position.shader_attribute_name).c_str(),
                                  position.property_name)) {
            if (position.property_name.empty()) {
                position.property_name = "v_position";
            }
            state->dispatcher.trigger<event::graph_renderer::set_position_attribute>(id, position);
        }
        auto &color = material->attributes[1];
        if (gui_property_selector(state, edges, {1, 3}, ("shader " + color.shader_attribute_name).c_str(), color.property_name)) {
            if (color.property_name.empty()) {
                material->use_uniform_color = true;
            } else {
                state->dispatcher.trigger<event::graph_renderer::set_color_attribute>(id, color);
            }
        }

        if (ImGui::Checkbox("use_uniform_color", &material->use_uniform_color)) {
            if (material->use_uniform_color) {
                color.property_name = "";
            }
        }

        draw_coloredit(&state->window, "uniform_color", material->uniform_color);
        ImGui::InputFloat("alpha", &material->uniform_alpha);
        ImGui::PopID();
    }
}

}
