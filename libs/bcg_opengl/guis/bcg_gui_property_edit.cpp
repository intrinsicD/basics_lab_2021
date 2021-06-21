//
// Created by alex on 01.04.21.
//

#include "bcg_gui_property_edit.h"
#include "bcg_gui_property_editfield.h"
#include "bcg_gui_property_selector.h"
#include "bcg_property_map_eigen.h"
#include "bcg_property_utils.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_material_mesh.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "bcg_opengl/renderers/points_renderer/bcg_material_points.h"
#include "bcg_opengl/renderers/points_renderer/bcg_events_points_renderer.h"

namespace bcg {

void gui_property_edit(viewer_state *state, property_container *container, const std::string &name) {
    ImGui::PushID(name.c_str());

    if(ImGui::CollapsingHeader((name + " 1 dimensional").c_str())){
        static std::string current_property_name11;
        static std::string current_property_name12;
        bool change = false;
        static float min = 0;
        static float max = 0;
        if(gui_property_selector(state, container, {1}, "property1", current_property_name11)){
            change = true;
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            min = MapConst(property).cast<float>().minCoeff();
            max = MapConst(property).cast<float>().maxCoeff();
        }
        if(gui_property_selector(state, container, {1}, "property2", current_property_name12)){
            change = true;
            auto property = container->get<bcg_scalar_t, 1>(current_property_name12);
            min = MapConst(property).cast<float>().minCoeff();
            max = MapConst(property).cast<float>().maxCoeff();
        }
        ImGui::Text("min: %f", min);
        ImGui::Text("max: %f", max);

        static float clamp_min = 0;
        static float clamp_max = 0;
        if (ImGui::Button("reset_clamp_values")) {
            clamp_min = min;
            clamp_max = max;
        }
        ImGui::InputFloat("clamp_min", &clamp_min);
        ImGui::InputFloat("clamp_max", &clamp_max);
        if (ImGui::Button("apply clamp")) {
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            clamp(container, property, clamp_min, clamp_max);
            change = true;
        }

        if (ImGui::Button("apply invert")) {
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            invert(container, property);
            change = true;
        }
        if (ImGui::Button("apply abs")) {
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            abs(container, property);
            change = true;
        }
        static float sigma = 1.0;
        if (ImGui::Button("apply gaussian")) {
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            gaussian(container, property, sigma);
            change = true;
        }
        ImGui::SameLine();
        ImGui::InputFloat("sigma", &sigma);

        static float shift_value = 0.0;
        if (ImGui::Button("apply shift")) {
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            shift(container, property, shift_value);
            change = true;
        }
        ImGui::SameLine();
        ImGui::InputFloat("shift_value", &shift_value);

        static float scale_value = 1.0;
        if (ImGui::Button("apply scaling")) {
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            scale(container, property, scale_value);
            change = true;
        }
        ImGui::SameLine();
        ImGui::InputFloat("scale_value", &scale_value);

        if(!current_property_name12.empty()){
            if (ImGui::Button("apply sum")) {
                auto property1 = container->get<bcg_scalar_t, 1>(current_property_name11);
                auto property2 = container->get<bcg_scalar_t, 1>(current_property_name12);
                sum(container, property1, property2);
                change = true;
            }

            if (ImGui::Button("apply prod")) {
                auto property1 = container->get<bcg_scalar_t, 1>(current_property_name11);
                auto property2 = container->get<bcg_scalar_t, 1>(current_property_name12);
                prod(container, property1, property2);
                change = true;
            }
        }
        if(change){
            if(state->scene.has<event::mesh_renderer::enqueue>(state->picker.entity_id)){
                auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                auto &color = material.attributes[2];
                color.property_name = current_property_name11;
                state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
            }else{
                auto &material = state->scene.get<material_points>(state->picker.entity_id);
                auto &color = material.attributes[1];
                color.property_name = current_property_name11;
                state->dispatcher.trigger<event::points_renderer::set_color_attribute>(state->picker.entity_id, color);
            }
        }
    }


    if(ImGui::CollapsingHeader((name + " 3 dimensional").c_str())){
        static std::string current_property_name31;
        static std::string current_property_name32;
        gui_property_selector(state, container, {3}, "property1", current_property_name31);
        gui_property_selector(state, container, {3}, "property2", current_property_name32);

        if (ImGui::Button("apply invert")) {
            auto property = container->get<VectorS<3>, 3>(current_property_name31);
            invert(container, property);
        }
        if (ImGui::Button("apply abs")) {
            auto property = container->get<VectorS<3>, 3>(current_property_name31);
            abs(container, property);
        }

        static Vector<float, 3> shift_values = Vector<float, 3>::Zero();
        if (ImGui::Button("apply shift")) {
            auto property = container->get<VectorS<3>, 3>(current_property_name31);
            shift(container, property, shift_values.cast<bcg_scalar_t>());
        }
        ImGui::SameLine();
        ImGui::InputFloat3("shift_values", shift_values.data());

        static Vector<float, 3> scale_values = Vector<float, 3>::Ones();
        if (ImGui::Button("apply scaling")) {
            auto property = container->get<VectorS<3>, 3>(current_property_name31);
            scale(container, property, scale_values.cast<bcg_scalar_t>());
        }
        ImGui::SameLine();
        ImGui::InputFloat3("scale_values", scale_values.data());

        if(!current_property_name32.empty()){
            if (ImGui::Button("apply sum")) {
                auto property1 = container->get<VectorS<3>, 3>(current_property_name31);
                auto property2 = container->get<VectorS<3>, 3>(current_property_name32);
                sum(container, property1, property2);
            }

            if (ImGui::Button("apply prod")) {
                auto property1 = container->get<VectorS<3>, 3>(current_property_name31);
                auto property2 = container->get<VectorS<3>, 3>(current_property_name32);
                prod(container, property1, property2);
            }
        }
    }
    if(ImGui::CollapsingHeader("rename or delete")){
        edit_field(state, container, name);
    }
    ImGui::PopID();
}

}