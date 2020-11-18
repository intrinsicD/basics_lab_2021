//
// Created by alex on 17.11.20.
//

#include "bcg_gui_viewer_camera.h"
#include "bcg_viewer_state.h"

namespace bcg{

void gui_viewer_camera(viewer_state *state){

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
        ImGui::Separator();
    }
}

}
