//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_RENDERER_H
#define BCG_GRAPHICS_BCG_MESH_RENDERER_H

#include "renderers/bcg_renderer.h"
#include "bcg_events_mesh_renderer.h"
#include "events/bcg_events.h"

namespace bcg {

struct mesh_renderer : public renderer {
    explicit mesh_renderer(viewer_state *state);

private:
    void on_startup(const event::internal::startup &event);

    void on_shutdown(const event::internal::shutdown &event);

    void on_enqueue(const event::mesh_renderer::enqueue &event);

    void on_set_material(const event::mesh_renderer::set_material &event);

    void on_begin_frame(const event::internal::begin_frame &event) override;

    void on_render(const event::internal::render &event) override;

    void on_end_frame(const event::internal::end_frame &event) override;

    void on_set_position_attribute(const event::mesh_renderer::set_position_attribute &event);

    void on_set_normal_attribute(const event::mesh_renderer::set_normal_attribute &event);

    void on_set_color_attribute(const event::mesh_renderer::set_color_attribute &event);
};

}

#endif //BCG_GRAPHICS_BCG_MESH_RENDERER_H
