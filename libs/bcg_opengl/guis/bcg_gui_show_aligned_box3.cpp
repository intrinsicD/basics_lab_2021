//
// Created by alex on 16.11.20.
//

#include "bcg_gui_show_aligned_box3.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_show_aligned_box3(aligned_box3 *aabb){
    if (!aabb) return;
    ImGui::LabelText("min", "%f %f %f", aabb->min[0], aabb->min[1], aabb->min[2]);
    ImGui::LabelText("max", "%f %f %f", aabb->max[0], aabb->max[1], aabb->max[2]);
    Vector<float, 3> center = aabb->center().cast<float>();
    ImGui::LabelText("center", "%f %f %f", center[0], center[1], center[2]);
    Vector<float, 3> halfextent = aabb->halfextent().cast<float>();
    ImGui::LabelText("halfextent", "%f %f %f", halfextent[0], halfextent[1], halfextent[2]);
}

}