//
// Created by alex on 22.06.21.
//

#include "bcg_gui_edit_transform.h"
#include "viewer/bcg_viewer_state.h"
#include "ImGuizmo/ImGuizmo.h"

namespace bcg {

void gui_edit_transform(viewer_state *state, camera &cam, const Transform &accumulated_model, Transform &transformation){
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);

    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE)){
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE)){
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE)){
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    }

    Matrix<float, 4, 4> matrix = transformation.matrix().cast<float>();
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(matrix.data(), matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.data());

    if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL)){
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD)){
            mCurrentGizmoMode = ImGuizmo::WORLD;
        }
    }
    static bool useSnap(false);
    ImGui::PushID("use_snap");
    ImGui::Checkbox("", &useSnap);
    ImGui::SameLine();
    static float snap[3] = {1.f, 1.f, 1.f};
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

    Matrix<float, 4, 4> view = (cam.view_matrix() * accumulated_model.matrix()).cast<float>();
    Matrix<float, 4, 4> proj = cam.projection_matrix().cast<float>();
    ImGuizmo::Manipulate(view.data(), proj.data(), mCurrentGizmoOperation, mCurrentGizmoMode,
                         matrix.data(), NULL, useSnap ? &snap[0] : NULL);

    transformation.matrix() = matrix.cast<bcg_scalar_t>();
}

void gui_edit_transform(viewer_state *state, Transform &transformation) {
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);

    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE)){
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE)){
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE)){
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    }

    Matrix<float, 4, 4> matrix = transformation.matrix().cast<float>();
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(matrix.data(), matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.data());

    if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL)){
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD)){
            mCurrentGizmoMode = ImGuizmo::WORLD;
        }
    }
    static bool useSnap(false);
    ImGui::PushID("use_snap");
    ImGui::Checkbox("", &useSnap);
    ImGui::SameLine();
    static float snap[3] = {1.f, 1.f, 1.f};
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

    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix().cast<float>();
    ImGuizmo::Manipulate(view.data(), proj.data(), mCurrentGizmoOperation, mCurrentGizmoMode,
                         matrix.data(), NULL, useSnap ? &snap[0] : NULL);

    transformation.matrix() = matrix.cast<bcg_scalar_t>();
}

}