//
// Created by alex on 16.11.20.
//

#include "bcg_gui_material_mesh.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_property_selector.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg{

void gui_material_mesh(viewer_state *state, material_mesh *material, entt::entity id){
    if (!material) return;
    ImGui::PushID("mesh_material");
    auto &position = material->attributes[0];
    auto *vertices = state->get_vertices(id);
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
        state->dispatcher.trigger<event::mesh_renderer::set_position_attribute>(id, position);
    }
    auto &normal = material->attributes[1];
    if (gui_property_selector(state, vertices, {3}, normal.shader_attribute_name, normal.property_name)) {
        if (normal.property_name.empty()) {
            normal.property_name = "v_normal";
        }
        state->dispatcher.trigger<event::mesh_renderer::set_normal_attribute>(id, normal);
    }
    auto &color = material->attributes[2];
    if (gui_property_selector(state, vertices, {1, 3}, color.shader_attribute_name, color.property_name)) {
        if (color.property_name.empty()) {
            material->use_uniform_color = true;
        } else {
            state->dispatcher.trigger<event::mesh_renderer::set_color_attribute>(id, color);
        }
    }
    if (ImGui::Checkbox("use_uniform_color", &material->use_uniform_color)) {
        color.property_name = "";
    }
    draw_coloredit(&state->window, "ambient", material->ambient);
    draw_coloredit(&state->window, "diffuse", material->diffuse);
    draw_coloredit(&state->window, "specular", material->specular);
    ImGui::InputFloat("shininess", &material->shininess);
    ImGui::InputFloat("alpha", &material->uniform_alpha);
    ImGui::PopID();
}

}