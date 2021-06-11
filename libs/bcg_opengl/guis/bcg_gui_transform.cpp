//
// Created by alex on 16.11.20.
//

#include <sstream>
#include "bcg_gui_transform.h"
#include "viewer/bcg_viewer_state.h"
#include "ImGuizmo/ImGuizmo.h"
#include "viewer/bcg_camera.h"

namespace bcg {

void EditTransform(const camera &cam, MatrixS<4, 4> &matrix) {
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    if (ImGui::IsKeyPressed(90))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(69))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(82)) // r Key
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    Matrix<float, 4, 4> M = matrix.cast<float>();
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(M.data(), matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, M.data());

    if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    static bool useSnap(false);
    if (ImGui::IsKeyPressed(83))
        useSnap = !useSnap;
    ImGui::PushID("use_snap");
    ImGui::Checkbox("", &useSnap);
    ImGui::SameLine();
    static float snap[3] = { 1.f, 1.f, 1.f };
    switch (mCurrentGizmoOperation) {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
    }
    ImGui::PopID();
    ImGuiIO &io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    Matrix<float, 4, 4> view = cam.view_matrix().cast<float>();
    Matrix<float, 4, 4> proj = cam.projection_matrix().cast<float>();
    ImGuizmo::Manipulate(view.data(), proj.data(), mCurrentGizmoOperation, mCurrentGizmoMode,
                         M.data(), NULL, useSnap ? &snap[0] : NULL);
    matrix = M.cast<bcg_scalar_t>();
}

void gui_transform(viewer_state *state, Transform *model, const std::string *name) {
    if (!model) return;
    std::string used_name = "transform";
    if (name != nullptr) {
        used_name = *name;
    }
    if (ImGui::CollapsingHeader(used_name.c_str())) {
        std::stringstream ss;
        ss << model->translation().transpose();
        draw_label(&state->window, "position", ss.str());
        ss.str("");
        ss << model->linear();
        draw_label(&state->window, "rotation\n\n\n", ss.str());
        ss.str("");
        ss << model->matrix();
        draw_label(&state->window, "model_matrix\n\n\n\n", ss.str());
        ImGui::Separator();
        if (ImGui::Button("reset")) {
            *model = Transform::Identity();
        }
        static bool show_guizmo = false;
        ImGui::Checkbox("show_guizmo", &show_guizmo);
        if(show_guizmo){
            EditTransform(state->cam, model->matrix());
        }
    }
}

}