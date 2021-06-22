//
// Created by alex on 15.04.21.
//

#include "bcg_gui_mesh_projected_distances.h"
#include "bcg_property_map_eigen.h"
#include "viewer/bcg_viewer_state.h"
#include "mesh/bcg_mesh_projected_distances.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "components/bcg_component_transform_world_space.h"

namespace bcg{

void gui_mesh_projected_distances(viewer_state *state){
    static entt::entity source_id = entt::null;
    static entt::entity target_id = entt::null;
    if(ImGui::Button("Set Source")){
        auto id = state->picker.entity_id;
        if(state->scene.valid(id) && state->scene.has<halfedge_mesh>(id)){
            source_id = id;
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Set Target")){
        auto id = state->picker.entity_id;
        if(state->scene.valid(id) && state->scene.has<halfedge_mesh>(id)){
            target_id = id;
        }
    }
    if(source_id != entt::null && state->scene.valid(source_id)){
        ImGui::LabelText("Source", "%d",source_id);
    }
    if(target_id != entt::null && state->scene.valid(target_id)){
        ImGui::LabelText("Target", "%d",target_id);
    }
    static float min = 0;
    static float max = 0;
    static float avg = 0;
    if(ImGui::Button("Compute projected distances")){
        auto &source = state->scene.get<halfedge_mesh>(source_id);
        auto &target = state->scene.get<halfedge_mesh>(target_id);
        auto &s_model = state->scene.get<world_space_transform>(source_id);
        auto &t_model = state->scene.get<world_space_transform>(target_id);
        Transform s_to_t = t_model.inverse() * s_model;
        mesh_projected_distances(source, target, s_to_t, state->config.parallel_grain_size);
        auto &material = state->scene.get<material_mesh>(source_id);
        auto &color = material.attributes[2];
        color.property_name = "v_projected_distances";
        state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(source_id, color);
        auto v_projected_distances = source.vertices.get<bcg_scalar_t, 1>("v_projected_distances");
        min = MapConst(v_projected_distances).minCoeff();
        max = MapConst(v_projected_distances).maxCoeff();
        avg = MapConst(v_projected_distances).mean();
        std::cout << "min " << min << " max " << max << " avg " << avg << "\n";
    }
    ImGui::Separator();
    ImGui::LabelText("min_value", "%f", min);
    ImGui::LabelText("max_value", "%f", max);
    ImGui::LabelText("avg_value", "%f", avg);
}

}