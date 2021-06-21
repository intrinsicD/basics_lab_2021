//
// Created by alex on 15.06.21.
//

#include "bcg_orthodontic_system.h"
#include "viewer/bcg_viewer_state.h"
#include "orthodontic/bcg_jaw_component.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"

namespace bcg {

orthodontic_system::orthodontic_system(viewer_state *state) : system("orthodontic_system", state) {
    state->dispatcher.sink<event::orthodontic::set_tooth_component>().connect<&orthodontic_system::on_set_tooth_component>(
            this);
    state->dispatcher.sink<event::orthodontic::set_tooth_color>().connect<&orthodontic_system::on_set_tooth_color>(
            this);
    state->dispatcher.sink<event::orthodontic::reset_teeth_colors>().connect<&orthodontic_system::on_reset_teeth_colors>(
            this);
    state->dispatcher.sink<event::internal::keyboard>().connect<&orthodontic_system::on_internal_keyboard>(
            this);
    state->dispatcher.sink<event::mouse::button>().connect<&orthodontic_system::on_mouse_button>(this);
    state->dispatcher.sink<event::orthodontic::render_gui>().connect<&orthodontic_system::on_render_gui>(this);
    consume_next_click = false;
    jaw_id = entt::null;
    fid_number = 0;
    colors[static_cast<int>(ToothType::incisor)] = color<float>::yellow;
    colors[static_cast<int>(ToothType::canine)] = color<float>::green;
    colors[static_cast<int>(ToothType::premolar)] = color<float>::magenta;
    colors[static_cast<int>(ToothType::molar)] = color<float>::blue;
    colors[static_cast<int>(ToothType::undefined)] = color<float>::grey;
}

void orthodontic_system::on_mouse_button(const event::mouse::button &) {
    auto id = state->picker.entity_id;
    if (state->gui.captured_mouse || !state->picker.valid) { return; }
    if (consume_next_click && state->mouse.left && state->scene.valid(id) && state->scene.valid(jaw_id) &&
        fid_number > 0) {
        auto &component = state->scene.get<jaw_component>(jaw_id);
        for (auto it = component.teeth.begin(); it != component.teeth.end(); ++it) {
            if ((*it).second == id) {
                component.teeth.erase(it);
                break;
            }
        }
        component.teeth[fid_number] = id;
        state->dispatcher.trigger<event::orthodontic::set_tooth_component>(id, fid_number, component.age);
        consume_next_click = false;
    }
}

void orthodontic_system::on_set_tooth_component(const event::orthodontic::set_tooth_component &event) {
    auto id = event.id;
    if (!state->scene.valid(id)) return;
    if (!state->scene.has<tooth_component>(id)) {
        state->scene().emplace<tooth_component>(id);
    }
    auto &tooth = state->scene.get<tooth_component>(id);
    switch (event.age) {
        case JawAge::primary: {
            primary_tooth_factory factory;
            tooth = factory.make(event.fdi_number);
            break;
        }
        case JawAge::permanent: {
            permanent_tooth_factory factory;
            tooth = factory.make(event.fdi_number);
            break;
        }
        default: {
            return;
        }
    }
    switch (tooth.type) {
        case ToothType::incisor : {
            state->dispatcher.trigger<event::orthodontic::set_tooth_color>(id, colors[static_cast<int>(tooth.type)]);
            break;
        }
        case ToothType::canine : {
            state->dispatcher.trigger<event::orthodontic::set_tooth_color>(id, colors[static_cast<int>(tooth.type)]);
            break;
        }
        case ToothType::premolar : {
            state->dispatcher.trigger<event::orthodontic::set_tooth_color>(id, colors[static_cast<int>(tooth.type)]);
            break;
        }
        case ToothType::molar : {
            state->dispatcher.trigger<event::orthodontic::set_tooth_color>(id, colors[static_cast<int>(tooth.type)]);
            break;
        }
        case ToothType::undefined : {
            state->dispatcher.trigger<event::orthodontic::set_tooth_color>(id, colors[static_cast<int>(tooth.type)]);
            break;
        }
        default: {
            break;
        }
    }
}

void orthodontic_system::on_reset_teeth_colors(const event::orthodontic::reset_teeth_colors &) {
    colors[static_cast<int>(ToothType::incisor)] = color<float>::yellow;
    colors[static_cast<int>(ToothType::canine)] = color<float>::green;
    colors[static_cast<int>(ToothType::premolar)] = color<float>::magenta;
    colors[static_cast<int>(ToothType::molar)] = color<float>::blue;
    colors[static_cast<int>(ToothType::undefined)] = color<float>::grey;
    if (state->scene.valid(jaw_id) && state->scene.has<jaw_component>(jaw_id)) {
        auto &component = state->scene.get<jaw_component>(jaw_id);
        for (auto &item : component.teeth) {
            auto id = item.second;
            if (state->scene.has<tooth_component>(id)) {
                auto &tooth = state->scene.get<tooth_component>(id);
                switch (tooth.type) {
                    case ToothType::incisor : {
                        state->dispatcher.trigger<event::orthodontic::set_tooth_color>(id,
                                                                                       colors[static_cast<int>(tooth.type)]);
                        break;
                    }
                    case ToothType::canine : {
                        state->dispatcher.trigger<event::orthodontic::set_tooth_color>(id,
                                                                                       colors[static_cast<int>(tooth.type)]);
                        break;
                    }
                    case ToothType::premolar : {
                        state->dispatcher.trigger<event::orthodontic::set_tooth_color>(id,
                                                                                       colors[static_cast<int>(tooth.type)]);
                        break;
                    }
                    case ToothType::molar : {
                        state->dispatcher.trigger<event::orthodontic::set_tooth_color>(id,
                                                                                       colors[static_cast<int>(tooth.type)]);
                        break;
                    }
                    case ToothType::undefined : {
                        state->dispatcher.trigger<event::orthodontic::set_tooth_color>(id,
                                                                                       colors[static_cast<int>(tooth.type)]);
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }
    }
}

void orthodontic_system::on_set_tooth_color(const event::orthodontic::set_tooth_color &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<material_mesh>(event.id)) return;
    auto &material = state->scene.get<material_mesh>(event.id);
    material.diffuse = event.color;
}

void orthodontic_system::on_internal_keyboard(const event::internal::keyboard &event) {
    if (state->keyboard.keys[GLFW_KEY_ESCAPE] && event.action == GLFW_PRESS) {
        consume_next_click = false;
    }
}

void orthodontic_system::on_render_gui(const event::orthodontic::render_gui &) {
    auto entity_id = state->picker.entity_id;
    if (!state->scene.valid(entity_id)) { return; }
    entt::entity current_selected_tooth = entt::null;

    static std::vector<std::string> jaw_type = jaw_type_names();
    static std::vector<std::string> jaw_age = jaw_age_type_names();
    static std::vector<std::string> tooth_type = tooth_type_names();
    static std::vector<std::string> tooth_quadrant = tooth_quadrant_names();

    if (ImGui::Button("Set Jaw Id")) {
        jaw_id = entity_id;
        if (!state->scene.has<jaw_component>(jaw_id)) {
            state->scene().emplace<jaw_component>(jaw_id);
        }
    }
    if (!state->scene.valid(jaw_id)) { return; }
    if (!state->scene.has<jaw_component>(jaw_id)) { return; }
    auto &jaw = state->scene.get<jaw_component>(jaw_id);


    int selected_jaw_type = static_cast<int>(jaw.type);
    int selected_jaw_age = static_cast<int>(jaw.age);
    int selected_tooth_type = static_cast<int>(ToothType::undefined);
    int selected_tooth_quadrant = static_cast<int>(ToothQuadrant::undefined);

    if (draw_combobox(&state->window, "JawType", selected_jaw_type, jaw_type)) {
        jaw.type = static_cast<JawType>(selected_jaw_type);
    }
    bool is_maxilla = jaw.type == JawType::maxilla;
    bool is_mandible = jaw.type == JawType::mandible;
    if (draw_combobox(&state->window, "JawAge", selected_jaw_age, jaw_age)) {
        jaw.age = static_cast<JawAge>(selected_jaw_age);
        if (jaw.age == JawAge::primary) {
            jaw.max_number_teeth = 10;
        } else if (jaw.age == JawAge::permanent) {
            jaw.max_number_teeth = 16;
        }
    }
    bool is_primary = jaw.age == JawAge::primary;
    bool is_permanent = jaw.age == JawAge::permanent;

    ImGui::Separator();
    auto dentition_button = [&](int fid_number_, float width) {
        ImVec2 button_sz(width, width);
        bool has_pushed_color = false;
        if (jaw.has_teeth(fid_number_)) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 255, 0)));
        }
        if (consume_next_click && fid_number == fid_number_) {
            has_pushed_color = true;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(255, 0, 0)));
        }
        if (ImGui::Button(std::to_string(fid_number_).c_str(), button_sz)) {
            consume_next_click = true;
            fid_number = fid_number_;
            if (jaw.has_teeth(fid_number_)) {
                current_selected_tooth = jaw.teeth[fid_number_];
            }
        }
        if (jaw.has_teeth(fid_number_)) {
            ImGui::PopStyleColor();
        }
        if (has_pushed_color) {
            ImGui::PopStyleColor();
        }
    };

    if (is_primary) {
        float width = float(state->window.widgets_width) / (jaw.max_number_teeth + 3);
        if (ImGui::CollapsingHeader("Primary Dentition")) {
            ImGui::Columns(2, "prim_dentition");
            ImGui::Separator();
            ImGui::Text("upper right - 5");
            ImGui::NextColumn();
            ImGui::Text("upper left - 6");
            ImGui::NextColumn();
            ImGui::Separator();
            if (is_mandible) {
                ImGui::PushDisabled();
            }
            dentition_button(55, width);
            ImGui::SameLine();
            dentition_button(54, width);
            ImGui::SameLine();
            dentition_button(53, width);
            ImGui::SameLine();
            dentition_button(52, width);
            ImGui::SameLine();
            dentition_button(51, width);
            ImGui::NextColumn();
            dentition_button(61, width);
            ImGui::SameLine();
            dentition_button(62, width);
            ImGui::SameLine();
            dentition_button(63, width);
            ImGui::SameLine();
            dentition_button(64, width);
            ImGui::SameLine();
            dentition_button(65, width);
            if (is_mandible) {
                ImGui::PopDisabled();
            }
            if (is_maxilla) {
                ImGui::PushDisabled();
            }
            ImGui::NextColumn();
            dentition_button(85, width);
            ImGui::SameLine();
            dentition_button(84, width);
            ImGui::SameLine();
            dentition_button(83, width);
            ImGui::SameLine();
            dentition_button(82, width);
            ImGui::SameLine();
            dentition_button(81, width);
            ImGui::NextColumn();
            dentition_button(71, width);
            ImGui::SameLine();
            dentition_button(72, width);
            ImGui::SameLine();
            dentition_button(73, width);
            ImGui::SameLine();
            dentition_button(74, width);
            ImGui::SameLine();
            dentition_button(75, width);
            if (is_maxilla) {
                ImGui::PopDisabled();
            }
            ImGui::NextColumn();
            ImGui::Separator();
            ImGui::Text("lower right - 8");
            ImGui::NextColumn();
            ImGui::Text("lower left - 7");
            ImGui::NextColumn();
            ImGui::Separator();
            ImGui::Columns(1);
        }
    } else if (is_permanent) {
        float width = float(state->window.widgets_width) / (jaw.max_number_teeth + 6);
        if (ImGui::CollapsingHeader("Permanent Dentition")) {
            ImGui::Columns(2, "prim_dentition");
            ImGui::Separator();
            ImGui::Text("upper right - 1");
            ImGui::NextColumn();
            ImGui::Text("upper left - 2");
            ImGui::NextColumn();
            ImGui::Separator();
            if (is_mandible) {
                ImGui::PushDisabled();
            }
            dentition_button(18, width);
            ImGui::SameLine();
            dentition_button(17, width);
            ImGui::SameLine();
            dentition_button(16, width);
            ImGui::SameLine();
            dentition_button(15, width);
            ImGui::SameLine();
            dentition_button(14, width);
            ImGui::SameLine();
            dentition_button(13, width);
            ImGui::SameLine();
            dentition_button(12, width);
            ImGui::SameLine();
            dentition_button(11, width);
            ImGui::NextColumn();
            dentition_button(21, width);
            ImGui::SameLine();
            dentition_button(22, width);
            ImGui::SameLine();
            dentition_button(23, width);
            ImGui::SameLine();
            dentition_button(24, width);
            ImGui::SameLine();
            dentition_button(25, width);
            ImGui::SameLine();
            dentition_button(26, width);
            ImGui::SameLine();
            dentition_button(27, width);
            ImGui::SameLine();
            dentition_button(28, width);
            if (is_mandible) {
                ImGui::PopDisabled();
            }
            ImGui::NextColumn();
            if (is_maxilla) {
                ImGui::PushDisabled();
            }
            dentition_button(48, width);
            ImGui::SameLine();
            dentition_button(47, width);
            ImGui::SameLine();
            dentition_button(46, width);
            ImGui::SameLine();
            dentition_button(45, width);
            ImGui::SameLine();
            dentition_button(44, width);
            ImGui::SameLine();
            dentition_button(43, width);
            ImGui::SameLine();
            dentition_button(42, width);
            ImGui::SameLine();
            dentition_button(41, width);
            ImGui::NextColumn();
            dentition_button(31, width);
            ImGui::SameLine();
            dentition_button(32, width);
            ImGui::SameLine();
            dentition_button(33, width);
            ImGui::SameLine();
            dentition_button(34, width);
            ImGui::SameLine();
            dentition_button(35, width);
            ImGui::SameLine();
            dentition_button(36, width);
            ImGui::SameLine();
            dentition_button(37, width);
            ImGui::SameLine();
            dentition_button(38, width);
            if (is_maxilla) {
                ImGui::PopDisabled();
            }
            ImGui::NextColumn();
            ImGui::Separator();
            ImGui::Text("lower right - 4");
            ImGui::NextColumn();
            ImGui::Text("lower left - 3");
            ImGui::NextColumn();
            ImGui::Separator();
            ImGui::Columns(1);
        }
    }
    static std::vector<int> selected_teeth;
    if (state->scene.has<tooth_component>(entity_id)) {
        auto &tooth = state->scene.get<tooth_component>(entity_id);
        if (ImGui::CollapsingHeader("Current selected tooth")) {
            ImGui::LabelText("entity_id", "%zu", size_t(entity_id));
            ImGui::LabelText("fid_number", "%d", tooth.fdi_number);
            ImGui::LabelText("number", "%d", tooth.number);
            ImGui::LabelText("type", "%s", tooth_type[static_cast<int>(tooth.type)].c_str());
            ImGui::LabelText("quadrant", "%s", tooth_quadrant[static_cast<int>(tooth.quadrant)].c_str());
        }
        ImGui::Text("Select: ");
        ImGui::SameLine();
        if (ImGui::Button("Incisors")) {
            selected_teeth = jaw.get_incisors();
        }
        ImGui::SameLine();
        if (ImGui::Button("Canines")) {
            selected_teeth = jaw.get_canines();
        }
        ImGui::SameLine();
        if (ImGui::Button("Premolars")) {
            selected_teeth = jaw.get_premolars();
        }
        ImGui::SameLine();
        if (ImGui::Button("Molars")) {
            selected_teeth = jaw.get_molars();
        }
        ImGui::SameLine();
        if (ImGui::Button("All")) {
            selected_teeth = jaw.get_all();
        }

        switch (tooth.type) {
            case ToothType::incisor: {
                ImGui::Text("Compute Incisal: ");
                ImGui::SameLine();
                ImGui::PushID("Compute Incisal");
                if (ImGui::Button("Edges")) {

                }
                ImGui::PopID();
                break;
            }
            case ToothType::canine: {
                ImGui::Text("Compute Canines: ");
                ImGui::SameLine();
                ImGui::PushID("Compute Canines");
                if (ImGui::Button("Cusp")) {

                }
                ImGui::PopID();
                break;
            }
            case ToothType::premolar: {
                ImGui::Text("Compute Premolars: ");
                ImGui::SameLine();
                ImGui::PushID("Compute Premolars");
                if (ImGui::Button("Cusp")) {

                }
                ImGui::SameLine();
                if (ImGui::Button("Grove")) {

                }
                ImGui::SameLine();
                if (ImGui::Button("Ridges")) {

                }
                ImGui::SameLine();
                if (ImGui::Button("Occlusal surface boundary")) {

                }
                ImGui::PopID();
                break;
            }
            case ToothType::molar: {
                ImGui::Text("Compute Molars: ");
                ImGui::SameLine();
                ImGui::PushID("Compute Molars");
                if (ImGui::Button("Cusps")) {

                }
                ImGui::SameLine();
                if (ImGui::Button("Grove")) {

                }
                ImGui::SameLine();
                if (ImGui::Button("Ridges")) {

                }
                ImGui::SameLine();
                if (ImGui::Button("Occlusal surface boundary")) {

                }
                ImGui::PopID();
                break;
            }
            default: {
                break;
            }
        }
    }
    if (ImGui::CollapsingHeader("Colors")) {
        bool changed = false;
        if (ImGui::Button("Reset")) {
            state->dispatcher.trigger<event::orthodontic::reset_teeth_colors>();
            changed = true;
        }
        if (ImGui::ColorEdit3("Incisor", colors[static_cast<int>(ToothType::incisor)].data()) || changed) {
            auto teeth = jaw.get_incisors();
            for (const auto &tooth : teeth) {
                state->dispatcher.trigger<event::orthodontic::set_tooth_color>(jaw.teeth[tooth],
                                                                               colors[static_cast<int>(ToothType::incisor)]);
            }
        }
        if (ImGui::ColorEdit3("Canine", colors[static_cast<int>(ToothType::canine)].data()) || changed) {
            auto teeth = jaw.get_canines();
            for (const auto &tooth : teeth) {
                state->dispatcher.trigger<event::orthodontic::set_tooth_color>(jaw.teeth[tooth],
                                                                               colors[static_cast<int>(ToothType::canine)]);
            }
        }
        if (ImGui::ColorEdit3("Premolar", colors[static_cast<int>(ToothType::premolar)].data()) || changed) {
            auto teeth = jaw.get_premolars();
            for (const auto &tooth : teeth) {
                state->dispatcher.trigger<event::orthodontic::set_tooth_color>(jaw.teeth[tooth],
                                                                               colors[static_cast<int>(ToothType::premolar)]);
            }
        }
        if (ImGui::ColorEdit3("Molar", colors[static_cast<int>(ToothType::molar)].data()) || changed) {
            auto teeth = jaw.get_molars();
            for (const auto &tooth : teeth) {
                state->dispatcher.trigger<event::orthodontic::set_tooth_color>(jaw.teeth[tooth],
                                                                               colors[static_cast<int>(ToothType::molar)]);
            }
        }
    }
}

}