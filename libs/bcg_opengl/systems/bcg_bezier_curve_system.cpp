//
// Created by alex on 18.11.20.
//

#include "bcg_bezier_curve_system.h"
#include "bcg_viewer_state.h"
#include "bcg_entity_info.h"
#include "renderers/curve_renderer/bcg_material_curve.h"
#include "renderers/curve_renderer/bcg_curve_renderer.h"
#include "renderers/picking_renderer/bcg_picking_renderer.h"
#include "renderers/points_renderer/bcg_points_renderer.h"
#include "renderers/graph_renderer/bcg_graph_renderer.h"
#include "geometry/curve/bcg_curve_bezier.h"

namespace bcg {

bezier_curve_system::bezier_curve_system(viewer_state *state) : system("bezier_curve_system", state) {
    state->dispatcher.sink<event::internal::startup>().connect<&bezier_curve_system::on_startup>(this);
    state->dispatcher.sink<event::curve::enable_2d_drawing>().connect<&bezier_curve_system::on_enable_2d_drawing>(this);
    state->dispatcher.sink<event::curve::enable_3d_drawing>().connect<&bezier_curve_system::on_enable_3d_drawing>(this);
    state->dispatcher.sink<event::curve::disable_drawing>().connect<&bezier_curve_system::on_disable_drawing>(this);
    state->dispatcher.sink<event::curve::setup>().connect<&bezier_curve_system::on_setup>(this);
}

void bezier_curve_system::on_startup(const event::internal::startup &event) {
    mode = curve_mode::disabled;
}

void bezier_curve_system::on_enable_2d_drawing(const event::curve::enable_2d_drawing &event) {
    if (state->scene.valid(current_curve_id)) {
        auto *curve = state->scene.try_get<curve_bezier>(current_curve_id);
        if (curve && curve->is_3d && !curve->finished) return;
    }
    mode = curve_mode::drawing_2d;
    state->dispatcher.sink<event::mouse::button>().connect<&bezier_curve_system::on_mouse_button>(this);
    state->dispatcher.sink<event::mouse::motion>().connect<&bezier_curve_system::on_mouse_motion>(this);
    state->dispatcher.sink<event::curve::make>().connect<&bezier_curve_system::on_make>(this);
}

void bezier_curve_system::on_enable_3d_drawing(const event::curve::enable_3d_drawing &event) {
    if (state->scene.valid(current_curve_id)) {
        auto *curve = state->scene.try_get<curve_bezier>(current_curve_id);
        if (curve && !curve->is_3d && !curve->finished) return;
    }
    mode = curve_mode::drawing_3d;
    state->dispatcher.sink<event::mouse::button>().connect<&bezier_curve_system::on_mouse_button>(this);
    state->dispatcher.sink<event::mouse::motion>().connect<&bezier_curve_system::on_mouse_motion>(this);
    state->dispatcher.sink<event::curve::make>().connect<&bezier_curve_system::on_make>(this);
}

void bezier_curve_system::on_disable_drawing(const event::curve::disable_drawing &event) {
    if (state->scene.valid(current_curve_id)) {
        auto *curve = state->scene.try_get<curve_bezier>(current_curve_id);
        curve->finished = true;
    }
    mode = curve_mode::disabled;
    state->dispatcher.sink<event::mouse::button>().disconnect<&bezier_curve_system::on_mouse_button>(this);
    state->dispatcher.sink<event::mouse::motion>().disconnect<&bezier_curve_system::on_mouse_motion>(this);
    state->dispatcher.sink<event::curve::make>().disconnect<&bezier_curve_system::on_make>(this);
}

void bezier_curve_system::on_setup(const event::curve::setup &event) {
    if (!state->scene.valid(event.id)) return;
    auto &curve = state->scene.get<curve_bezier>(event.id);
    switch (mode) {
        case curve_mode::drawing_2d: {
            curve.is_3d = false;
            break;
        }
        case curve_mode::drawing_3d: {
            curve.is_3d = true;
            break;
        }
        case curve_mode::disabled:{
            break;
        }
    }
    entity_info info;
    info.entity_name = "bezier_curve";
    info.filename = "generated";
    state->scene.emplace_or_replace<entity_info>(event.id, info);
    current_curve_id = event.id;
    state->picker.entity_id = event.id;
}

void bezier_curve_system::on_make(const event::curve::make &event) {
    auto id = state->scene.create();
    state->scene.emplace<curve_bezier>(id);
    state->dispatcher.trigger<event::curve::setup>(id);
}

void bezier_curve_system::on_mouse_button(const event::mouse::button &event) {
    if (state->mouse.is_captured_by_gui) return;
    if (!state->scene.valid(current_curve_id)) return;
    auto *curve = state->scene.try_get<curve_bezier>(current_curve_id);
    if (!curve || curve->finished) return;

    VectorS<3> point = get_point();
    if(point.squaredNorm() == 0) return;
    auto n = curve->control_points().size();
    if (state->mouse.left) {
        if (curve->points_clicked == 0) {
            auto v0 = curve->add_vertex(point);
            auto v1 = curve->add_vertex(point);
            auto v2 = curve->add_vertex(point);
            auto v3 = curve->add_vertex(point);
            curve->add_edge(v0, v1);
            curve->add_edge(v1, v2);
            curve->add_edge(v2, v3);
            ++curve->points_clicked;
        } else if (curve->points_clicked == 1) {
            curve->positions[n - 3] = point;
            curve->positions[n - 2] = point;
            curve->positions[n - 1] = point;
            ++curve->points_clicked;
        }else if (curve->points_clicked >= 2) {
            int e = curve->points_clicked % 2;
            if (e == 0) {
                curve->positions[n - 2] = point;
                curve->positions[n - 1] = point;
                ++curve->points_clicked;
            } else if (e == 1) {
                curve->positions[n - 1] = point;
                VectorS<3> d = (curve->positions[n - 1] - curve->positions[n - 2]) / 5.0;
                curve->positions[n - 1] -= d;
                auto v0 = curve->add_vertex(point+d);
                auto v1 = curve->add_vertex(point+d);
                auto v2 = curve->add_vertex(point+d);
                curve->add_edge(n - 1, v0);
                curve->add_edge(v0, v1);
                curve->add_edge(v1, v2);
                ++curve->points_clicked;
            }
        }
    } else if (state->mouse.right) {
        curve->finished = true;
        if (curve->points_clicked == 1) {
            curve->positions[n - 3] = point;
            curve->positions[n - 2] = point;
            curve->positions[n - 1] = point;
            ++curve->points_clicked;
        }else if (curve->points_clicked >= 2) {
            auto e = curve->points_clicked % 2;
            if (e == 0) {
                curve->positions[n - 2] = point;
                curve->positions[n - 1] = point;
                ++curve->points_clicked;
            } else if (e == 1) {
                curve->positions[n - 1] = point;
                ++curve->points_clicked;
            }
        }
    }
    if (!curve->positions.empty()) {
        curve->positions.set_dirty();
        auto &material = state->scene.get_or_emplace<material_curve>(current_curve_id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(current_curve_id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges", 0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(current_curve_id, edge_attributes);
        if (curve->points_clicked == 1) {
            state->scene.emplace_or_replace<event::points_renderer::enqueue>(current_curve_id);
            state->scene.emplace_or_replace<event::picking_renderer::enqueue>(current_curve_id);
            state->scene.emplace_or_replace<event::curve_renderer::enqueue>(current_curve_id);
            state->scene.emplace_or_replace<event::graph_renderer::enqueue>(current_curve_id);
        }
        state->dispatcher.trigger<event::curve_renderer::setup_for_rendering>(current_curve_id);
        state->dispatcher.trigger<event::picking_renderer::setup_for_rendering>(current_curve_id);
        state->dispatcher.trigger<event::points_renderer::setup_for_rendering>(current_curve_id);
        state->dispatcher.trigger<event::graph_renderer::setup_for_rendering>(current_curve_id);
    }
}

void bezier_curve_system::on_mouse_motion(const event::mouse::motion &event) {
    if (state->mouse.is_captured_by_gui) return;
    if (!state->scene.valid(current_curve_id)) return;
    auto *curve = state->scene.try_get<curve_bezier>(current_curve_id);
    if (!curve || curve->finished) return;

    VectorS<3> point = get_point();

    auto n = curve->control_points().size();
    if (state->mouse.is_moving) {
        if (curve->points_clicked == 1) {
            curve->positions[n - 3] = point;
            curve->positions[n - 2] = point;
            curve->positions[n - 1] = point;
        }else if (curve->points_clicked >= 2) {
            auto e = curve->points_clicked % 2;
            if (e == 0) {
                curve->positions[n - 2] = point;
                curve->positions[n - 1] = point;
            } else if (e == 1) {
                curve->positions[n - 1] = point;
            }
        }
    }
    if (!curve->positions.empty()) {
        curve->positions.set_dirty();
        auto &material = state->scene.get<material_curve>(current_curve_id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(current_curve_id, material.attributes);
        state->dispatcher.trigger<event::curve_renderer::setup_for_rendering>(current_curve_id);
        state->dispatcher.trigger<event::picking_renderer::setup_for_rendering>(current_curve_id);
        state->dispatcher.trigger<event::points_renderer::setup_for_rendering>(current_curve_id);
        state->dispatcher.trigger<event::graph_renderer::setup_for_rendering>(current_curve_id);
    }
}

VectorS<3> bezier_curve_system::get_point() const{
    if(mode == curve_mode::drawing_2d){
        const auto &vp = state->window.framebuffer_viewport;
        int x = static_cast<int>( state->mouse.cursor_position[0] * state->window.high_dpi_scaling);
        int y = static_cast<int>( vp[3] - state->mouse.cursor_position[1] * state->window.high_dpi_scaling);

        float zf = 0.999885023f;
        float xf = float(x - vp[0]) / ((float) vp[2]) * 2.0f - 1.0f; //ndc
        float yf = float(y - vp[1]) / ((float) vp[3]) * 2.0f - 1.0f; //ndc
        zf = zf * 2.0f - 1.0f;
        VectorS<4> p = (state->cam.projection_matrix * state->cam.view_matrix()).inverse() *
                VectorS<4>(xf, yf, zf, 1.0);
        p /= p[3];
        return p.head<3>();
    }else{
        return state->picker.model_space_point;
    }
}

}