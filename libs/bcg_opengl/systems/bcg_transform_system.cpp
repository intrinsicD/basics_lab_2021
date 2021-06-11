//
// Created by alex on 28.10.20.
//

#include "bcg_transform_system.h"
#include "viewer/bcg_viewer_state.h"
#include "ImGuizmo/ImGuizmo.h"

namespace bcg {

transform_system::transform_system(viewer_state *state) : system("transform_system", state) {
    state->dispatcher.sink<event::transform::add>().connect<&transform_system::on_add>(this);
    state->dispatcher.sink<event::transform::translate>().connect<&transform_system::on_translate>(this);
    state->dispatcher.sink<event::transform::scale>().connect<&transform_system::on_scale>(this);
    state->dispatcher.sink<event::transform::rotate>().connect<&transform_system::on_rotate>(this);
    state->dispatcher.sink<event::internal::update>().connect<&transform_system::on_update>(this);
    state->dispatcher.sink<event::internal::render_gui>().connect<&transform_system::on_render_gui>(this);
}

void transform_system::on_add(const event::transform::add &event) {
    if (state->scene.valid(event.id) && !state->scene.all_of<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
}

void transform_system::on_translate(const event::transform::translate &event) {
    if (state->scene.valid(event.id)) {
        auto &model = state->scene.get<Transform>(event.id);
        model = Translation(event.trans) * model;
    }
}

void transform_system::on_scale(const event::transform::scale &event) {
    if (state->scene.valid(event.id)) {
        auto &model = state->scene.get<Transform>(event.id);
        model = model * Scaling(event.scales);
    }
}

void transform_system::on_rotate(const event::transform::rotate &event) {
    if (state->scene.valid(event.id)) {
        auto &model = state->scene.get<Transform>(event.id);
        auto world_translation = Translation(model.translation());

        model = world_translation *
                Rotation(event.angle_axis[0], event.angle_axis.tail<3>()) *
                world_translation.inverse() *
                model;
    }
}

void transform_system::on_update(const event::internal::update &) {
    if (state->mouse.is_dragging && !state->gui.captured_mouse && state->keyboard.ctrl_pressed) {
        if (!state->picker.valid || !state->scene.all_of<Transform>(state->picker.entity_id)) return;
        auto &model = state->scene.get<Transform>(state->picker.entity_id);
/*        if (state->mouse.middle) {
            //translate camera in plane

            model = world_translation(state->mouse.cursor_delta,
                                      state->window.width,
                                      state->window.height,
                                      state->cam).inverse() * model;
        }
        if (state->mouse.left) {
            //rotate camera around target point

            model = Translation(model.translation()) *
                    world_rotation(state->mouse.cursor_position,
                                   state->window.width,
                                   state->window.height,
                                   state->cam) *
                    Translation(-model.translation()) *
                    model;
        }*/
    }
}

void transform_system::on_render_gui(const event::internal::render_gui &) {
    if (!state->scene.valid(state->picker.entity_id)) return;
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix().cast<float>();
    ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    bool open_ptr = state->picker.entity_id != entt::null;
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoBackground;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    ImGui::Begin("TransparentTransformWindow", &open_ptr, window_flags);
    auto win_size = ImGui::GetWindowSize();
    win_size[0] = 250;
    win_size[1] = 60;
    ImGui::SetWindowSize(win_size);
    ImVec2 pos(state->window.width - win_size[0], 19);
    ImGui::SetWindowPos(pos);
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    static bool useSnap(false);
    ImGui::PushID("use_snap");
    ImGui::Checkbox("", &useSnap);
    ImGui::SameLine();
    static float snap[3] = {1.f, 1.f, 1.f};
    ImGui::InputFloat3("Snap", &snap[0]);
    ImGui::PopID();

    ImGuiIO &io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    auto &model = state->scene.get<Transform>(state->picker.entity_id);
    Matrix<float, 4, 4> M = model.matrix().cast<float>();
    ImGuizmo::Enable(true);
    ImGuizmo::Manipulate(view.data(), proj.data(), mCurrentGizmoOperation, mCurrentGizmoMode, M.data(),
                         useSnap ? &snap[0] : NULL);
    model.matrix() = M.cast<bcg_scalar_t>();
    ImGui::End();
}

}