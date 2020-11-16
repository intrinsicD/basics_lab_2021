//
// Created by alex on 16.11.20.
//

#include "bcg_gui_material_curve.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_property_selector.h"
#include "renderers/curve_renderer/bcg_events_curve_renderer.h"

namespace bcg{

void gui_material_curve(viewer_state *state, material_curve *material, entt::entity id){
    if (!material) return;
    ImGui::PushID("curve_material");
    auto *vertices = state->get_vertices(id);
    auto &position = material->attributes[0];
    draw_label(&state->window, (material->vao.name + " vao id").c_str(), std::to_string(material->vao.handle));
    if (ImGui::CollapsingHeader("Captured Attributes")) {
        for (const auto &item : material->vao.captured_attributes) {
            draw_label(&state->window, std::to_string(item.first).c_str(), item.second);
        }
    }
    if (gui_property_selector(state, vertices, {3}, position.shader_attribute_name.c_str(),
                                   position.property_name)) {
        if (position.property_name.empty()) {
            position.property_name = "v_position";
        }
        state->dispatcher.trigger<event::curve_renderer::set_position_attribute>(id, position);
    }

    draw_coloredit(&state->window, "uniform_color", material->uniform_color);
    ImGui::InputFloat("alpha", &material->uniform_alpha);
    ImGui::Checkbox("show_bezier", &material->show_bezier);
    ImGui::Checkbox("show_hermite", &material->show_hermite);
    draw_dragger(&state->window, "tesselation_level", material->tesselation_level, 1, 1, 256);
    ImGui::PopID();
}

}