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
    static bool show_edit = false;
    if(ImGui::Button(name.c_str())){
        show_edit = true;
    }
    if(show_edit) edit_field(state, container, name, show_edit);
    if(ImGui::CollapsingHeader((name + " 1 dimensional").c_str())){
        static std::string current_property_name11;
        static std::string current_property_name12;
        bool change = false;
        static float min = 0;
        static float max = 0;
        if(gui_property_selector(state, container, {1}, "property1", current_property_name11)){
            change = true;
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            min = MapConst(property).minCoeff();
            max = MapConst(property).maxCoeff();
        }
        if(gui_property_selector(state, container, {1}, "property2", current_property_name12)){
            change = true;
            auto property = container->get<bcg_scalar_t, 1>(current_property_name12);
            min = MapConst(property).minCoeff();
            max = MapConst(property).maxCoeff();
        }
        ImGui::Text("min: %f", min);
        ImGui::Text("max: %f", max);

        if (ImGui::Button("apply invert")) {
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            invert(property);
            change = true;
        }
        if (ImGui::Button("apply abs")) {
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            abs(property);
            change = true;
        }
        static float sigma = 1.0;
        if (ImGui::Button("apply gaussian")) {
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            gaussian(property, sigma);
            change = true;
        }
        ImGui::SameLine();
        ImGui::InputFloat("sigma", &sigma);

        static float shift_value = 0.0;
        if (ImGui::Button("apply shift")) {
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            shift(property, shift_value);
            change = true;
        }
        ImGui::SameLine();
        ImGui::InputFloat("shift_value", &shift_value);

        static float scale_value = 1.0;
        if (ImGui::Button("apply scaling")) {
            auto property = container->get<bcg_scalar_t, 1>(current_property_name11);
            scale(property, scale_value);
            change = true;
        }
        ImGui::SameLine();
        ImGui::InputFloat("scale_value", &scale_value);

        if(!current_property_name12.empty()){
            if (ImGui::Button("apply sum")) {
                auto property1 = container->get<bcg_scalar_t, 1>(current_property_name11);
                auto property2 = container->get<bcg_scalar_t, 1>(current_property_name12);
                sum(property1, property2);
                change = true;
            }

            if (ImGui::Button("apply prod")) {
                auto property1 = container->get<bcg_scalar_t, 1>(current_property_name11);
                auto property2 = container->get<bcg_scalar_t, 1>(current_property_name12);
                prod(property1, property2);
                change = true;
            }
        }
        if(change){
            if(state->scene.all_of<event::mesh_renderer::enqueue>(state->picker.entity_id)){
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
            invert(property);
        }
        if (ImGui::Button("apply abs")) {
            auto property = container->get<VectorS<3>, 3>(current_property_name31);
            abs(property);
        }

        static Vector<float, 3> shift_values = Vector<float, 3>::Zero();
        if (ImGui::Button("apply shift")) {
            auto property = container->get<VectorS<3>, 3>(current_property_name31);
            shift(property, shift_values.cast<bcg_scalar_t>());
        }
        ImGui::SameLine();
        ImGui::InputFloat3("shift_values", shift_values.data());

        static Vector<float, 3> scale_values = Vector<float, 3>::Ones();
        if (ImGui::Button("apply scaling")) {
            auto property = container->get<VectorS<3>, 3>(current_property_name31);
            scale(property, scale_values.cast<bcg_scalar_t>());
        }
        ImGui::SameLine();
        ImGui::InputFloat3("scale_values", scale_values.data());

        if(!current_property_name32.empty()){
            if (ImGui::Button("apply sum")) {
                auto property1 = container->get<VectorS<3>, 3>(current_property_name31);
                auto property2 = container->get<VectorS<3>, 3>(current_property_name32);
                sum(property1, property2);
            }

            if (ImGui::Button("apply prod")) {
                auto property1 = container->get<VectorS<3>, 3>(current_property_name31);
                auto property2 = container->get<VectorS<3>, 3>(current_property_name32);
                prod(property1, property2);
            }
        }
    }

    ImGui::End();
}

}