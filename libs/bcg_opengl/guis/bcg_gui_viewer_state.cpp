//
// Created by alex on 16.11.20.
//

#include "bcg_gui_viewer_state.h"
#include "bcg_gui_viewer_colors.h"
#include "bcg_gui_viewer_window.h"
#include "bcg_gui_viewer_keyboard.h"
#include "bcg_gui_viewer_mouse.h"
#include "bcg_gui_viewer_time.h"
#include "bcg_gui_viewer_gui.h"
#include "bcg_gui_viewer_picker.h"
#include "bcg_gui_viewer_camera.h"
#include "viewer/bcg_imgui.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_component_entity_info.h"
#include "bcg_gui_show_aligned_box3.h"
#include "bcg_gui_component_transform_world_space.h"
#include "bcg_gui_component_transform_object_space.h"
#include "bcg_gui_component_loading_backup.h"
#include "bcg_gui_rendering_options.h"
#include "bcg_gui_mesh.h"
#include "bcg_gui_graph.h"
#include "bcg_gui_point_cloud.h"
#include "bcg_gui_ogl_shape.h"
#include "bcg_gui_component_entity_hierarchy.h"
#include "geometry/curve/bcg_curve_bezier.h"

namespace bcg {

void gui_viewer_state(viewer_state *state) {
    if (ImGui::CollapsingHeader("Scene")) {
        auto view = state->scene().view<entity_info>();
        for (const auto id : view) {
            auto &info = state->scene.get<entity_info>(id);
            std::stringstream ss;
            ss << info.entity_name << " id: " << std::to_string((unsigned int) id);
            if (ImGui::TreeNode(ss.str().c_str())) {
                if(ImGui::CollapsingHeader("Entity Info")){
                    gui_component_entity_info(state, id);
                }
                if(ImGui::CollapsingHeader("Aligned Box")){
                    gui_show_aligned_box3(state->scene.try_get<aligned_box3>(id));
                }
                if(ImGui::CollapsingHeader("World Space Transform")){
                    gui_component_transform_world_space(state, id);
                }
                if(ImGui::CollapsingHeader("Object Space Transform")){
                    gui_component_transform_object_space(state, id);
                }
                if(ImGui::CollapsingHeader("Loading Backup")){
                    gui_component_loading_backup(state, id);
                }
                gui_rendering_options(state, id);
                auto *mesh = state->scene.try_get<halfedge_mesh>(id);
                if (mesh) gui_mesh(state, mesh);
                auto *graph = state->scene.try_get<halfedge_graph>(id);
                if (graph) gui_graph(state, graph);
                auto *curve = state->scene.try_get<curve_bezier>(id);
                if (curve) gui_graph(state, curve);
                auto *pc = state->scene.try_get<point_cloud>(id);
                if (pc) gui_point_cloud(state, pc);
                gui_ogl_shape(state, state->scene.try_get<ogl_shape>(id));
                gui_component_entity_hierarchy(state, state->scene.try_get<entity_hierarchy>(id));
                ImGui::TreePop();
            }
        }
    }

    gui_viewer_colors(state);
    gui_viewer_window(state);
    gui_viewer_keyboard(state);
    gui_viewer_mouse(state);
    gui_viewer_time(state);
    gui_viewer_gui(state);
    gui_viewer_picker(state);
    gui_viewer_camera(state);
}

}