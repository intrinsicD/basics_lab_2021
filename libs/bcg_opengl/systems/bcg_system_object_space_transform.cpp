//
// Created by alex on 21.06.21.
//

#include "bcg_system_object_space_transform.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_transform_object_space.h"
#include "guis/bcg_gui_guizmo.h"

namespace bcg {

system_object_space_transform::system_object_space_transform(viewer_state *state) : system(
        "system_object_space_transform",
        state) {
    state->dispatcher.sink<event::transform::object_space::init>().connect<&system_object_space_transform::init>(this);
    state->dispatcher.sink<event::transform::object_space::remove>().connect<&system_object_space_transform::remove>(
            this);
    state->dispatcher.sink<event::transform::object_space::set>().connect<&system_object_space_transform::set>(this);
    state->dispatcher.sink<event::transform::object_space::set_identity>().connect<&system_object_space_transform::set_identity>(
            this);
    state->dispatcher.sink<event::transform::object_space::pre_transform>().connect<&system_object_space_transform::pre_transform>(
            this);
    state->dispatcher.sink<event::transform::object_space::pre_translate>().connect<&system_object_space_transform::pre_translate>(
            this);
    state->dispatcher.sink<event::transform::object_space::pre_scale>().connect<&system_object_space_transform::pre_scale>(
            this);
    state->dispatcher.sink<event::transform::object_space::pre_uniform_scale>().connect<&system_object_space_transform::pre_uniform_scale>(
            this);
    state->dispatcher.sink<event::transform::object_space::pre_rotate>().connect<&system_object_space_transform::pre_rotate>(
            this);
    state->dispatcher.sink<event::transform::object_space::post_transform>().connect<&system_object_space_transform::post_transform>(
            this);
    state->dispatcher.sink<event::transform::object_space::post_translate>().connect<&system_object_space_transform::post_translate>(
            this);
    state->dispatcher.sink<event::transform::object_space::post_scale>().connect<&system_object_space_transform::post_scale>(
            this);
    state->dispatcher.sink<event::transform::object_space::post_uniform_scale>().connect<&system_object_space_transform::post_uniform_scale>(
            this);
    state->dispatcher.sink<event::transform::object_space::post_rotate>().connect<&system_object_space_transform::post_rotate>(
            this);
    state->dispatcher.sink<event::transform::object_space::render_gui>().connect<&system_object_space_transform::render_gui>(
            this);
}

system_object_space_transform::~system_object_space_transform() {
    state->dispatcher.sink<event::transform::object_space::init>().disconnect<&system_object_space_transform::init>(
            this);
    state->dispatcher.sink<event::transform::object_space::remove>().disconnect<&system_object_space_transform::remove>(
            this);
    state->dispatcher.sink<event::transform::object_space::set>().disconnect<&system_object_space_transform::set>(this);
    state->dispatcher.sink<event::transform::object_space::set_identity>().disconnect<&system_object_space_transform::set_identity>(
            this);
    state->dispatcher.sink<event::transform::object_space::pre_transform>().disconnect<&system_object_space_transform::pre_transform>(
            this);
    state->dispatcher.sink<event::transform::object_space::pre_translate>().disconnect<&system_object_space_transform::pre_translate>(
            this);
    state->dispatcher.sink<event::transform::object_space::pre_scale>().disconnect<&system_object_space_transform::pre_scale>(
            this);
    state->dispatcher.sink<event::transform::object_space::pre_uniform_scale>().disconnect<&system_object_space_transform::pre_uniform_scale>(
            this);
    state->dispatcher.sink<event::transform::object_space::pre_rotate>().disconnect<&system_object_space_transform::pre_rotate>(
            this);
    state->dispatcher.sink<event::transform::object_space::post_transform>().disconnect<&system_object_space_transform::post_transform>(
            this);
    state->dispatcher.sink<event::transform::object_space::post_translate>().disconnect<&system_object_space_transform::post_translate>(
            this);
    state->dispatcher.sink<event::transform::object_space::post_scale>().disconnect<&system_object_space_transform::post_scale>(
            this);
    state->dispatcher.sink<event::transform::object_space::post_uniform_scale>().disconnect<&system_object_space_transform::post_uniform_scale>(
            this);
    state->dispatcher.sink<event::transform::object_space::post_rotate>().disconnect<&system_object_space_transform::post_rotate>(
            this);
    state->dispatcher.sink<event::transform::object_space::render_gui>().disconnect<&system_object_space_transform::render_gui>(
            this);
}

void system_object_space_transform::init(const event::transform::object_space::init &event) {
    if (state->scene.has<object_space_transform>(event.id)) return;
    set_identity({event.id});
}

void system_object_space_transform::remove(const event::transform::object_space::remove &event) {
    if (!state->scene.valid(event.id)) return;
    state->scene().remove_if_exists<object_space_transform>(event.id);
}

void system_object_space_transform::set(const event::transform::object_space::set &event) {
    if (!state->scene.valid(event.id)) return;
    state->scene().emplace_or_replace<object_space_transform>(event.id, event.transformation);
}

void system_object_space_transform::set_identity(const event::transform::object_space::set_identity &event) {
    set({event.id, Transform::Identity()});
}

void system_object_space_transform::pre_transform(const event::transform::object_space::pre_transform &event) {
    if (!state->scene.valid(event.id)) return;
    Transform &transformation = state->scene.get<object_space_transform>(event.id);
    transformation = transformation * event.transform;
}

void system_object_space_transform::pre_translate(const event::transform::object_space::pre_translate &event) {
    if (!state->scene.valid(event.id)) return;
    Transform &transformation = state->scene.get<object_space_transform>(event.id);
    transformation = transformation * Translation(event.translation);
}

void system_object_space_transform::pre_scale(const event::transform::object_space::pre_scale &event) {
    if (!state->scene.valid(event.id)) return;
    Transform &transformation = state->scene.get<object_space_transform>(event.id);
    transformation = transformation * Scaling(event.scaling);
}

void system_object_space_transform::pre_uniform_scale(const event::transform::object_space::pre_uniform_scale &event) {
    if (!state->scene.valid(event.id)) return;
    Transform &transformation = state->scene.get<object_space_transform>(event.id);
    transformation = transformation * Scaling(event.scaling, event.scaling, event.scaling);
}

void system_object_space_transform::pre_rotate(const event::transform::object_space::pre_rotate &event) {
    if (!state->scene.valid(event.id)) return;
    Transform &transformation = state->scene.get<object_space_transform>(event.id);
    auto angle = event.angle_axis.norm();
    transformation = transformation * Rotation(angle, event.angle_axis / angle);
}

void system_object_space_transform::post_transform(const event::transform::object_space::post_transform &event) {
    if (!state->scene.valid(event.id)) return;
    Transform &transformation = state->scene.get<object_space_transform>(event.id);
    transformation = event.transform * transformation;
}

void system_object_space_transform::post_translate(const event::transform::object_space::post_translate &event) {
    if (!state->scene.valid(event.id)) return;
    Transform &transformation = state->scene.get<object_space_transform>(event.id);
    transformation = Translation(event.translation) * transformation;
}

void system_object_space_transform::post_scale(const event::transform::object_space::post_scale &event) {
    if (!state->scene.valid(event.id)) return;
    Transform &transformation = state->scene.get<object_space_transform>(event.id);
    transformation = Scaling(event.scaling) * transformation;
}

void
system_object_space_transform::post_uniform_scale(const event::transform::object_space::post_uniform_scale &event) {
    if (!state->scene.valid(event.id)) return;
    Transform &transformation = state->scene.get<object_space_transform>(event.id);
    transformation = Scaling(event.scaling, event.scaling, event.scaling) * transformation;
}

void system_object_space_transform::post_rotate(const event::transform::object_space::post_rotate &event) {
    if (!state->scene.valid(event.id)) return;
    Transform &transformation = state->scene.get<object_space_transform>(event.id);
    auto angle = event.angle_axis.norm();
    transformation = Rotation(angle, event.angle_axis / angle) * transformation;
}

void system_object_space_transform::render_gui(const event::transform::object_space::render_gui &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<object_space_transform>(event.id)) return;

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoBackground;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

    bool open_ptr = true;
    ImGui::Begin("TransparentObjectTransformWindow", &open_ptr, window_flags);
    ImGui::Text("Object Space Transform");
    ImGui::Separator();
    ImGui::LabelText("Selected Entity", "%zu", size_t(event.id));
    gui_guizmo(state, event.id, state->scene.get<object_space_transform>(event.id));
    auto win_size = ImGui::GetWindowSize();
    ImVec2 pos(float(state->window.width - win_size[0]), float(state->gui.menu_height));
    ImGui::SetWindowPos(pos);
    ImGui::End();
}

}