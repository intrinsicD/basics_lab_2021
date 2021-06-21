//
// Created by alex on 08.04.21.
//

#include "bcg_gui_property_clamp.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_property_utils.h"
#include "bcg_gui_property_selector.h"
#include "bcg_property_map_eigen.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg{

void gui_property_clamp(viewer_state *state, property_container *container, property<bcg_scalar_t, 1> p, float &min_value, float &max_value){
    static float property_min = MapConst(p).cast<float>().minCoeff();
    static float property_max = MapConst(p).cast<float>().maxCoeff();
    static float speed_clamp = 0.001;
    if(ImGui::DragFloat("clamp_min", &min_value, speed_clamp, property_min, property_max)){
        min_value = std::max(min_value, property_min);
        min_value = std::min(min_value, max_value - 2 * speed_clamp);
        auto result = clamp(container, p, min_value, max_value);
        if(state->scene.has<halfedge_mesh>(state->picker.entity_id)){
            auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
            auto &color = material.attributes[2];
            color.property_name = result.name();
            state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id,
                                                                                        color);
        }
    }
    if(ImGui::DragFloat("clamp_max", &max_value, speed_clamp, property_min, property_max)){
        max_value = std::max(max_value, min_value + 2 * speed_clamp);
        max_value = std::min(max_value, property_max);
        auto result = clamp(container, p, min_value, max_value);
        if(state->scene.has<halfedge_mesh>(state->picker.entity_id)){
            auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
            auto &color = material.attributes[2];
            color.property_name = result.name();
            state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id,
                                                                                        color);
        }
    }
}

}