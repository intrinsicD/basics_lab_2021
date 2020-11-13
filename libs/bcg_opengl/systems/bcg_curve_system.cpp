//
// Created by alex on 13.11.20.
//

#include "bcg_curve_system.h"
#include "bcg_viewer_state.h"
#include "bcg_entity_info.h"
#include "geometry/curve/bcg_curve.h"
#include "geometry/aligned_box/bcg_aligned_box.h"
#include "math/bcg_matrix_map_eigen.h"

namespace bcg {

curve_system::curve_system(viewer_state *state) : system("curve_system", state) {
    state->dispatcher.sink<event::internal::startup>().connect<&curve_system::on_startup>(this);
    state->dispatcher.sink<event::curve::disable_curve_mode>().connect<&curve_system::on_disable_mode>(this);
    state->dispatcher.sink<event::curve::enable_plane_curve_mode>().connect<&curve_system::on_enable_plane_mode>(this);
    state->dispatcher.sink<event::curve::enable_surface_curve_mode>().connect<&curve_system::on_enable_surface_mode>(
            this);
    state->dispatcher.sink<event::curve::setup_curve>().connect<&curve_system::on_setup_curve>(this);
    state->dispatcher.sink<event::curve::new_curve>().connect<&curve_system::on_make_curve>(this);
    state->dispatcher.sink<event::curve::delete_curve>().connect<&curve_system::on_delete_curve>(this);
}

void curve_system::on_startup(const event::internal::startup &event) {

}

void curve_system::on_disable_mode(const event::curve::disable_curve_mode &event) {
    state->dispatcher.sink<event::mouse::button>().disconnect<&curve_system::on_mouse_button>(this);
    state->dispatcher.sink<event::mouse::motion>().disconnect<&curve_system::on_mouse_motion>(this);
    mode = curve_mode::DISABLED;
}

void curve_system::on_enable_plane_mode(const event::curve::enable_plane_curve_mode &event) {
    state->dispatcher.sink<event::mouse::button>().connect<&curve_system::on_mouse_button>(this);
    state->dispatcher.sink<event::mouse::motion>().connect<&curve_system::on_mouse_motion>(this);
    mode = curve_mode::PLANE_MODE;
}

void curve_system::on_enable_surface_mode(const event::curve::enable_surface_curve_mode &event) {
    state->dispatcher.sink<event::mouse::button>().connect<&curve_system::on_mouse_button>(this);
    state->dispatcher.sink<event::mouse::motion>().connect<&curve_system::on_mouse_motion>(this);
    mode = curve_mode::SURFACE_MODE;
}

void curve_system::on_setup_curve(const event::curve::setup_curve &event){
    auto &curve3d = state->scene.get<curve<3>>(event.id);

    state->dispatcher.trigger<event::transform::add>(event.id);

    aligned_box3 aabb(curve3d.control_points());
    state->scene.emplace<entity_info>(event.id, event.filename, "mesh", aabb.center(), aabb.halfextent().maxCoeff());

    Map(curve3d.m_control_points) =
            (MapConst(curve3d.control_points()).rowwise() - aabb.center().transpose()) / aabb.halfextent().maxCoeff();

    state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(event.id);
    state->dispatcher.trigger<event::aligned_box::add>(event.id);
    state->scene.emplace_or_replace<event::picking_renderer::enqueue>(event.id);
    state->scene.emplace_or_replace<event::mesh_renderer::enqueue>(event.id);
    state->picker.entity_id = event.id;
    std::cout << mesh << "\n";
}

void curve_system::on_make_curve(const event::curve::new_curve &event) {
    switch (mode) {
        case curve_mode::PLANE_MODE : {
            curve<3> curve;
            auto id = state->scene.create();
            state->scene.emplace<curve<>>();
            return;
        }
        case curve_mode::SURFACE_MODE : {
            curve<3> curve;
            return;
        }
        default : {

        }
    }
}

void curve_system::on_delete_curve(const event::curve::delete_curve &event) {

}

void curve_system::on_mouse_button(const event::mouse::button &event) {

}

void curve_system::on_mouse_motion(const event::mouse::motion &event) {

}

}