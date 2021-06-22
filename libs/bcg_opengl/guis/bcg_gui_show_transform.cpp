//
// Created by alex on 22.06.21.
//

#include "bcg_gui_show_transform.h"
#include "viewer/bcg_viewer_state.h"
#include "ImGuizmo/ImGuizmo.h"

namespace bcg {

void gui_show_transform(const Transform &transformation) {
    Matrix<float, 4, 4> matrix = transformation.matrix().cast<float>();
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(matrix.data(), matrixTranslation, matrixRotation, matrixScale);
    ImGui::LabelText("Tr", "%f %f %f", matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
    ImGui::LabelText("Rt", "%f %f %f", matrixRotation[0], matrixRotation[1], matrixRotation[2]);
    ImGui::LabelText("Sc", "%f %f %f", matrixScale[0], matrixScale[1], matrixScale[2]);
}

}