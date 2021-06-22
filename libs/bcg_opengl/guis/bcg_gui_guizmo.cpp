//
// Created by alex on 17.06.21.
//

#include "bcg_gui_guizmo.h"
#include "viewer/bcg_viewer_state.h"
#include "ImGuizmo/ImGuizmo.h"

namespace bcg{

bool gui_guizmo(viewer_state *state, entt::entity id, Transform &transform){
    ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    ImGui::SameLine();
    static bool disabled = false;
    ImGui::Checkbox("Disabled", &disabled);
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        if(state->scene.valid(id)){
            transform.setIdentity();
        }
    }
    ImGuizmo::Enable(!disabled);

    static bool useSnap(false);
    ImGui::PushID("use_snap");
    ImGui::Checkbox("", &useSnap);
    ImGui::PopID();
    ImGui::SameLine();
    static float snap[3] = {1.0f, 1.0f, 1.0f};
    ImGui::InputFloat3("Snap", &snap[0]);

    ImGuiIO &io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    Matrix<float, 4, 4> M = transform.matrix().cast<float>();
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix().cast<float>();
    bool manipulated = ImGuizmo::Manipulate(view.data(), proj.data(), mCurrentGizmoOperation, mCurrentGizmoMode, M.data(),
                         nullptr, useSnap ? &snap[0] : nullptr);
    transform = Transform(M.cast<bcg_scalar_t>());
    return manipulated;
}

}