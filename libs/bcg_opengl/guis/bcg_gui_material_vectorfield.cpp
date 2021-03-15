//
// Created by alex on 16.11.20.
//

#include "bcg_gui_material_vectorfield.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_property_selector.h"
#include "renderers/vectorfield_renderer/bcg_events_vectorfield_renderer.h"

namespace bcg{

void gui_material_vectorfield(viewer_state *state, material_vectorfield *material, property_container *container,
                              entt::entity id, std::string vectorfield_name){
    if (!material) return;
    if(ImGui::CollapsingHeader(("vectorfield material " + vectorfield_name).c_str())){
        ImGui::PushID(("vectorfield_material" + vectorfield_name).c_str());
        ImGui::Checkbox("enabled", &material->enabled);
        auto &position = material->attributes[0];
        if (ImGui::CollapsingHeader("vertex array object")) {
            draw_label(&state->window, (material->vao.name + " vao id").c_str(), std::to_string(material->vao.handle));
            for (const auto &item : material->vao.captured_attributes) {
                draw_label(&state->window, std::to_string(item.first).c_str(), item.second);
            }
        }
        std::string property_name = position.property_name;
        if (gui_property_selector(state, container, {3}, ("shader " + position.shader_attribute_name).c_str(),
                                  position.property_name)) {
            if (position.property_name.empty()) {
                position.property_name = property_name;
            }
            state->dispatcher.trigger<event::vectorfield_renderer::set_position_attribute>(id, vectorfield_name, position);
        }
        auto &color = material->attributes[2];
        if (gui_property_selector(state, container, {1, 3}, ("shader " + color.shader_attribute_name).c_str(), color.property_name)) {
            if (color.property_name.empty()) {
                material->use_uniform_color = true;
            } else {
                color.buffer_name = color.property_name;
                state->dispatcher.trigger<event::vectorfield_renderer::set_color_attribute>(id, vectorfield_name, color);
            }
        }
        if (ImGui::Checkbox("use_uniform_color", &material->use_uniform_color)) {
            if (material->use_uniform_color) {
                color.property_name = "";
            }
        }
        ImGui::Checkbox("use_uniform_vector_length", &material->use_uniform_size);
        draw_dragger(&state->window, "uniform_size", material->uniform_size, 0.01, 0.001, 2.0);
        draw_coloredit(&state->window, "uniform_color", material->uniform_color);
        ImGui::InputFloat("alpha", &material->uniform_alpha);
        ImGui::PopID();
    }
}

}
