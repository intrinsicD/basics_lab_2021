//
// Created by alex on 15.06.21.
//

#include "bcg_gui_orthodontic_jaw.h"
#include "viewer/bcg_viewer_state.h"
#include "orthodontic/bcg_jaw_component.h"
#include "orthodontic/bcg_tooth_component.h"
#include "events/bcg_events_orthodontic.h"

namespace bcg {

void gui_orthodontic_jaw(viewer_state *state) {
    auto entity_id = state->picker.entity_id;
    if (!state->scene.valid(entity_id)) { return; }
    if (!state->scene.all_of<jaw_component>(entity_id)) {
        state->scene.emplace<jaw_component>(entity_id);
    }

    static std::vector<std::string> jaw_type = jaw_type_names();
    static std::vector<std::string> jaw_age = jaw_age_type_names();
    static std::vector<std::string> tooth_type = tooth_type_names();
    static std::vector<std::string> tooth_quadrant = tooth_quadrant_names();

    auto &jaw = state->scene.get<jaw_component>(entity_id);

    int selected_jaw_type = static_cast<int>(jaw.type);
    int selected_jaw_age = static_cast<int>(jaw.age);
    int selected_tooth_type = static_cast<int>(ToothType::undefined);
    int selected_tooth_quadrant = static_cast<int>(ToothQuadrant::undefined);


    if(draw_combobox(&state->window, "JawType", selected_jaw_type, jaw_type)){
        jaw.type = static_cast<JawType>(selected_jaw_type);
    }
    if(draw_combobox(&state->window, "JawAge", selected_jaw_age, jaw_age)){
        jaw.age = static_cast<JawAge>(selected_jaw_age);
        if(jaw.age == JawAge::primary){
            jaw.max_number_teeth = 10;
        }else if(jaw.age == JawAge::permament){
            jaw.max_number_teeth = 16;
        }

    }
    ImGui::Separator();

    static bool button_clicked = false;
    static int fid_number = 0;
    //TODO consume clicks to set teeth.

    if(jaw.age == JawAge::primary){
        float width = float(state->window.widgets_width) / (jaw.max_number_teeth + 3);
        ImVec2 button_sz(width, width);
        if(ImGui::CollapsingHeader("Primary Dentition")){
            ImGui::Columns(2, "prim_dentition");
            ImGui::Separator();
            ImGui::Text("upper right - 5"); ImGui::NextColumn();
            ImGui::Text("upper left - 6"); ImGui::NextColumn();
            ImGui::Separator();
            if(jaw.has_teeth(55)){
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 255, 0)));
            }
            if(ImGui::Button("55", button_sz)){
                button_clicked = true;
                fid_number = 55;
            }
            if(jaw.has_teeth(55)){
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();
            ImGui::Button("54", button_sz); ImGui::SameLine();
            ImGui::Button("53", button_sz); ImGui::SameLine();
            ImGui::Button("52", button_sz); ImGui::SameLine();
            ImGui::Button("51", button_sz); ImGui::SameLine();
            ImGui::NextColumn();
            ImGui::Button("61", button_sz); ImGui::SameLine();
            ImGui::Button("62", button_sz); ImGui::SameLine();
            ImGui::Button("63", button_sz); ImGui::SameLine();
            ImGui::Button("64", button_sz); ImGui::SameLine();
            ImGui::Button("65", button_sz);
            ImGui::NextColumn();
            ImGui::Button("85", button_sz); ImGui::SameLine();
            ImGui::Button("84", button_sz); ImGui::SameLine();
            ImGui::Button("83", button_sz); ImGui::SameLine();
            ImGui::Button("82", button_sz); ImGui::SameLine();
            ImGui::Button("81", button_sz); ImGui::SameLine();
            ImGui::NextColumn();
            ImGui::Button("71", button_sz); ImGui::SameLine();
            ImGui::Button("72", button_sz); ImGui::SameLine();
            ImGui::Button("73", button_sz); ImGui::SameLine();
            ImGui::Button("74", button_sz); ImGui::SameLine();
            ImGui::Button("75", button_sz);
            ImGui::NextColumn();
            ImGui::Separator();
            ImGui::Text("lower right - 8"); ImGui::NextColumn();
            ImGui::Text("lower left - 7"); ImGui::NextColumn();
            ImGui::Separator();
            ImGui::Columns(1);
        }
    }else if(jaw.age == JawAge::permament){
        float width = float(state->window.widgets_width) / (jaw.max_number_teeth + 6);
        ImVec2 button_sz(width, width);
        if(ImGui::CollapsingHeader("Permanent Dentition")){
            ImGui::Columns(2, "prim_dentition");
            ImGui::Separator();
            ImGui::Text("upper right - 1"); ImGui::NextColumn();
            ImGui::Text("upper left - 2"); ImGui::NextColumn();
            ImGui::Separator();
            ImGui::Button("18", button_sz); ImGui::SameLine();
            ImGui::Button("17", button_sz); ImGui::SameLine();
            ImGui::Button("16", button_sz); ImGui::SameLine();
            ImGui::Button("15", button_sz); ImGui::SameLine();
            ImGui::Button("14", button_sz); ImGui::SameLine();
            ImGui::Button("13", button_sz); ImGui::SameLine();
            ImGui::Button("12", button_sz); ImGui::SameLine();
            ImGui::Button("11", button_sz); ImGui::SameLine();
            ImGui::NextColumn();
            ImGui::Button("21", button_sz); ImGui::SameLine();
            ImGui::Button("22", button_sz); ImGui::SameLine();
            ImGui::Button("23", button_sz); ImGui::SameLine();
            ImGui::Button("24", button_sz); ImGui::SameLine();
            ImGui::Button("25", button_sz); ImGui::SameLine();
            ImGui::Button("26", button_sz); ImGui::SameLine();
            ImGui::Button("27", button_sz); ImGui::SameLine();
            ImGui::Button("28", button_sz);
            ImGui::NextColumn();
            ImGui::Button("48", button_sz); ImGui::SameLine();
            ImGui::Button("47", button_sz); ImGui::SameLine();
            ImGui::Button("46", button_sz); ImGui::SameLine();
            ImGui::Button("45", button_sz); ImGui::SameLine();
            ImGui::Button("44", button_sz); ImGui::SameLine();
            ImGui::Button("43", button_sz); ImGui::SameLine();
            ImGui::Button("42", button_sz); ImGui::SameLine();
            ImGui::Button("41", button_sz); ImGui::SameLine();
            ImGui::NextColumn();
            ImGui::Button("31", button_sz); ImGui::SameLine();
            ImGui::Button("32", button_sz); ImGui::SameLine();
            ImGui::Button("33", button_sz); ImGui::SameLine();
            ImGui::Button("34", button_sz); ImGui::SameLine();
            ImGui::Button("35", button_sz); ImGui::SameLine();
            ImGui::Button("36", button_sz); ImGui::SameLine();
            ImGui::Button("37", button_sz); ImGui::SameLine();
            ImGui::Button("38", button_sz);
            ImGui::NextColumn();
            ImGui::Separator();
            ImGui::Text("lower right - 4"); ImGui::NextColumn();
            ImGui::Text("lower left - 5"); ImGui::NextColumn();
            ImGui::Separator();
            ImGui::Columns(1);
        }
    }
}

}