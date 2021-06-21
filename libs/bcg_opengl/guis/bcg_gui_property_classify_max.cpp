//
// Created by alex on 08.04.21.
//

#include "bcg_gui_property_classify_max.h"
#include "bcg_property_utils.h"
#include "bcg_gui_property_selector.h"
#include "bcg_property_map_eigen.h"
#include "viewer/bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg {

Result gui_property_classify_max(viewer_state *state){
    static Result result{false, ""};
    if(state->scene.valid(state->picker.entity_id)){
        auto *vertices = state->get_vertices(state->picker.entity_id);
        if(vertices){
            static std::string current_property_name;
            static float min;
            static float max;
            static bcg_scalar_t threshold;
            bool change = false;
            if(gui_property_selector(state, vertices, {1}, "property", current_property_name)){
                if(state->scene.has<halfedge_mesh>(state->picker.entity_id)){
                    auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                    auto &color = material.attributes[2];
                    color.property_name = current_property_name;
                    state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id,
                                                                                                color);
                    result.current_property_name = current_property_name;
                    change = true;
                }
            }
            auto base_ptr = vertices->get_base_ptr(current_property_name);

            if(base_ptr != nullptr && (base_ptr->type() == property_types::Type::FLOAT ||  base_ptr->type() == property_types::Type::DOUBLE)){
                auto p = vertices->get<bcg_scalar_t, 1>(current_property_name);
                if(p){
                    if(change){
                        min = MapConst(p).cast<float>().minCoeff();
                        max = MapConst(p).cast<float>().maxCoeff();
                        threshold = MapConst(p).mean();
                    }
                    result = gui_property_classify_max(state, vertices, p, threshold, min, max);
                }
            }
        }
    }
    return result;
}

Result gui_property_classify_max(viewer_state *state, property_container *container, property<bcg_scalar_t, 1> p,
                               bcg_scalar_t &threshold, bcg_scalar_t min, bcg_scalar_t max) {
    static Result result{false, ""};
    ImGui::PushID((p.name() + "_classify_max").c_str());
    float classify_max_value = threshold;
    if (ImGui::DragFloat((p.name() + "_classify_max_value").c_str(), &classify_max_value, 0.001, min, max)) {
        auto result_max = classify_max(container, p, classify_max_value);
        if(state->scene.has<halfedge_mesh>(state->picker.entity_id)){
            auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
            auto &color = material.attributes[2];
            color.property_name = result_max.name();
            state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id,
                                                                                        color);
            result.current_property_name = result_max.name();
            result.triggered = true;
        }
        threshold = classify_max_value;
    }
    ImGui::PopID();
    return result;
}

}