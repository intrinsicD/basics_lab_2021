//
// Created by alex on 16.11.20.
//

#include "bcg_gui_viewer_state.h"
#include "bcg_imgui.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_entity_info.h"
#include "bcg_gui_aligned_box3.h"
#include "bcg_gui_transform.h"
#include "bcg_gui_rendering_options.h"
#include "bcg_gui_mesh.h"
#include "bcg_gui_graph.h"
#include "bcg_gui_point_cloud.h"
#include "bcg_gui_ogl_shape.h"

namespace bcg {

void gui_viewer_state(viewer_state *state) {
    if (ImGui::CollapsingHeader("Scene")) {
        auto view = state->scene.view<entity_info>();
        for (const auto id : view) {
            auto &info = state->scene.get<entity_info>(id);
            std::stringstream ss;
            ss << info.entity_name << " id: " << std::to_string((unsigned int) id);
            if (ImGui::TreeNode(ss.str().c_str())) {
                gui_entity_info(state, state->scene.try_get<entity_info>(id), id);
                gui_aligned_box3(state, state->scene.try_get<aligned_box3>(id));
                gui_transform(state, state->scene.try_get<Transform>(id));
                gui_rendering_options(state, id);
                auto *mesh = state->scene.try_get<halfedge_mesh>(id);
                if (mesh) gui_mesh(state, mesh);
                auto *graph = state->scene.try_get<halfedge_graph>(id);
                if (graph) gui_graph(state, graph);
                auto *pc = state->scene.try_get<point_cloud>(id);
                if (pc) gui_point_cloud(state, pc);
                gui_ogl_shape(state, state->scene.try_get<ogl_shape>(id));
                ImGui::TreePop();
            }
        }
    }
    if (ImGui::CollapsingHeader("Colors")) {
        draw_coloredit(&state->window, "background", state->colors.background);
        draw_coloredit(&state->window, "overlay", state->colors.overlay);
        draw_coloredit(&state->window, "entity_selection", state->colors.entity_selection);
        draw_coloredit(&state->window, "vertex_selection", state->colors.vertex_selection);
        draw_coloredit(&state->window, "edge_selection", state->colors.edge_selection);
        draw_coloredit(&state->window, "face_selection", state->colors.face_selection);
        draw_coloredit(&state->window, "boundary", state->colors.boundary);
        draw_coloredit(&state->window, "wireframe", state->colors.wireframe);
    }

    if (ImGui::CollapsingHeader("Window")) {
        draw_label(&state->window, "title", state->window.title);
        draw_label(&state->window, "width", std::to_string(state->window.width));
        draw_label(&state->window, "height", std::to_string(state->window.height));
        std::stringstream ss;
        ss << state->window.framebuffer_viewport.transpose();
        draw_label(&state->window, "framebuffer_viewport", ss.str());
        ImGui::InputInt("widget_width", &state->window.widgets_width, 1, 10);
    }

    if (ImGui::CollapsingHeader("Keyboard")) {
        ImGui::Checkbox("shift pressed", &state->keyboard.shift_pressed);
        ImGui::Checkbox("alt pressed", &state->keyboard.alt_pressed);
        ImGui::Checkbox("ctrl pressed", &state->keyboard.ctrl_pressed);
        ImGui::Checkbox("command pressed", &state->keyboard.command_pressed);
        ImGui::Checkbox("no modifier", &state->keyboard.no_modifier);
        ImGui::Checkbox("captured by gui", &state->keyboard.is_captured_by_gui);
    }
    if (ImGui::CollapsingHeader("Mouse")) {
        ImGui::Checkbox("moving", &state->mouse.is_moving);
        ImGui::Checkbox("scrolling", &state->mouse.is_scrolling);
        ImGui::Checkbox("dragging", &state->mouse.is_dragging);
        ImGui::Checkbox("captured by gui", &state->mouse.is_captured_by_gui);
        ImGui::Checkbox("left pressed", &state->mouse.left);
        ImGui::Checkbox("middle pressed", &state->mouse.middle);
        ImGui::Checkbox("right pressed", &state->mouse.right);
        ImGui::Separator();
        std::stringstream ss;
        ss << state->mouse.last_left_click.transpose();
        draw_label(&state->window, "last left click", ss.str());
        ss.str("");
        ss << state->mouse.last_middle_click.transpose();
        draw_label(&state->window, "last middle click", ss.str());
        ss.str("");
        ss << state->mouse.last_right_click.transpose();
        draw_label(&state->window, "last right click", ss.str());
        ss.str("");
        ss << state->mouse.cursor_position.transpose();
        draw_label(&state->window, "cursor_position", ss.str());
        ss.str("");
        ss << state->mouse.last_cursor_position.transpose();
        draw_label(&state->window, "last_cursor_position", ss.str());
        ss.str("");
        ss << state->mouse.cursor_delta.transpose();
        draw_label(&state->window, "cursor_delta", ss.str());
        draw_label(&state->window, "scroll_value", std::to_string(state->mouse.scroll_value));
    }
    if (ImGui::CollapsingHeader("Time")) {
        draw_label(&state->window, "clock_now", std::to_string(state->time.clock_now));
        draw_label(&state->window, "clock_last", std::to_string(state->time.clock_last));
        draw_label(&state->window, "time_now", std::to_string(state->time.time_now));
        draw_label(&state->window, "time_delta", std::to_string(state->time.time_delta));
    }
    if (ImGui::CollapsingHeader("Gui")) {
        ImGui::Checkbox("widgets_active", &state->gui.widgets_active);
        ImGui::Checkbox("show_menu", &state->gui.show_menu);
        ImGui::Checkbox("hide_all", &state->gui.hide_all);
        draw_label(&state->window, "menu_height", std::to_string(state->gui.menu_height));
    }
    if (ImGui::CollapsingHeader("Picker")) {
        draw_label(&state->window, "entity_id", std::to_string((unsigned int) state->picker.entity_id));
        draw_label(&state->window, "vertex_id", std::to_string(state->picker.vertex_id.idx));
        draw_label(&state->window, "edge_id", std::to_string(state->picker.edge_id.idx));
        draw_label(&state->window, "face_id", std::to_string(state->picker.face_id.idx));
        ImGui::Separator();
        std::stringstream ss;
        ss << state->picker.model_space_point.transpose();
        draw_label(&state->window, "model_space_point", ss.str());
        ss.str("");
        ss << state->picker.world_space_point.transpose();
        draw_label(&state->window, "world_space_point", ss.str());
        ss.str("");
        ss << state->picker.view_space_point.transpose();
        draw_label(&state->window, "view_space_point", ss.str());
    }
    if (ImGui::CollapsingHeader("Camera")) {
        float near = state->cam.near;
        if (ImGui::InputFloat("near", &near)) {
            state->cam.near = near;
        }
        float far = state->cam.far;
        if (ImGui::InputFloat("far", &far)) {
            state->cam.far = far;
        }
        float aspect = state->cam.aspect;
        if (ImGui::InputFloat("aspect", &aspect)) {
            state->cam.aspect = aspect;
        }
        float fovy_degrees = state->cam.fovy_degrees;
        if (ImGui::InputFloat("fovy degrees", &fovy_degrees)) {
            state->cam.fovy_degrees = fovy_degrees;
        }
        float rot_speed = state->cam.rot_speed;
        if (ImGui::InputFloat("rot_speed", &rot_speed)) {
            state->cam.rot_speed = rot_speed;
        }
        draw_checkbox(&state->window, "orthoraphic", state->cam.orthographic);
        ImGui::Separator();
        float left = state->cam.left;
        if (ImGui::InputFloat("left", &left)) {
            state->cam.left = left;
        }
        float right = state->cam.right;
        if (ImGui::InputFloat("right", &right)) {
            state->cam.right = right;
        }
        float top = state->cam.top;
        if (ImGui::InputFloat("top", &top)) {
            state->cam.top = top;
        }
        float bottom = state->cam.bottom;
        if (ImGui::InputFloat("bottom", &bottom)) {
            state->cam.bottom = bottom;
        }
        ImGui::Separator();
        std::stringstream ss;
        ss << state->cam.target_point.transpose();
        draw_label(&state->window, "target_point", ss.str());
        ss.str("");
        ss << state->cam.model_matrix.matrix();

        draw_label(&state->window, "model\n\n\n\n", ss.str());
        ss.str("");
        ss << state->cam.view_matrix();
        draw_label(&state->window, "view\n\n\n\n", ss.str());
        ss.str("");
        ss << state->cam.projection_matrix;
        draw_label(&state->window, "projection\n\n\n\n", ss.str());
        ss.str("");
    }
}

}