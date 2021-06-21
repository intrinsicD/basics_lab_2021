//
// Created by alex on 30.04.21.
//

#include "bcg_gui_hq_pbr_ufsi_material.h"
#include "bcg_hq_pbr_ufsi_renderer_events.h"
#include "bcg_opengl/guis/bcg_gui_property_selector.h"
#include "bcg_hq_pbr_ufsi_material.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_hq_pbr_ufsi_material(viewer_state *state, entt::entity id){
    if(!state->scene.valid(id)) return;
    auto &material = state->scene().get_or_emplace<hq_pbr_ufsi_material>(id);

    if(ImGui::CollapsingHeader("hq_pbr_ufsi_material")){
        ImGui::PushID(("hq_pbr_ufsi_material" + std::to_string(int(id))).c_str());
        auto *vertices = state->get_vertices(id);
        auto &position = material.attributes[0];
        if (ImGui::CollapsingHeader("vertex array object")) {
            draw_label(&state->window, (material.vao.name + " vao id").c_str(), std::to_string(material.vao.handle));
            for (const auto &item : material.vao.captured_attributes) {
                draw_label(&state->window, std::to_string(item.first).c_str(), item.second);
            }
        }
        if (gui_property_selector(state, vertices, {3}, ("shader " + position.shader_attribute_name).c_str(),
                                  position.property_name)) {
            if (position.property_name.empty()) {
                position.property_name = "v_position";
            }
            state->dispatcher.trigger<event::hq_pbr_ufsi_renderer::set_position>(id, position.property_name);
        }
        auto &color = material.attributes[1];
        if (gui_property_selector(state, vertices, {1, 3}, ("shader " + color.shader_attribute_name).c_str(), color.property_name)) {
            if (color.property_name.empty()) {
                material.use_uniform_color = true;
            } else {
                state->dispatcher.trigger<event::hq_pbr_ufsi_renderer::set_color>(id, color.property_name);
            }
        }

        auto &normal = material.attributes[2];
        if (gui_property_selector(state, vertices, {3}, ("shader " + normal.shader_attribute_name).c_str(), normal.property_name)) {
            if (normal.property_name.empty()) {

            } else {
                state->dispatcher.trigger<event::hq_pbr_ufsi_renderer::set_normal>(id, normal.property_name);
            }
        }
        if (ImGui::Checkbox("use_uniform_color", &material.use_uniform_color)) {
            if (material.use_uniform_color) {
                color.property_name = "";
            }
        }

        draw_coloredit(&state->window, "uniform_color", material.uniform_color);
        ImGui::InputFloat("alpha", &material.alpha);
        draw_input(&state->window, "max_point_size", state->config.max_point_size);
        ImGui::PopID();
    }
}

}