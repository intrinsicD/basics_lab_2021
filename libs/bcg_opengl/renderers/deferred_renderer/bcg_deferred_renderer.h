//
// Created by alex on 15.12.20.
//

#ifndef BCG_GRAPHICS_BCG_DEFERRED_RENDERER_H
#define BCG_GRAPHICS_BCG_DEFERRED_RENDERER_H


#include "renderers/bcg_renderer.h"
#include "bcg_events_deferred_renderer.h"
#include "events/bcg_events.h"

namespace bcg {

struct deferred_renderer : public renderer {
    explicit deferred_renderer(viewer_state *state);

private:
    void on_startup(const event::internal::startup &event);

    void on_shutdown(const event::internal::shutdown &event);

    void on_enqueue(const event::deferred_renderer::enqueue &event);

    void on_setup_for_rendering(const event::deferred_renderer::setup_for_rendering &event);

    void on_begin_frame(const event::internal::begin_frame &event) override;

    void on_render(const event::internal::render &event) override;

    void on_end_frame(const event::internal::end_frame &event) override;

    ogl_framebuffer gBuffer;
    ogl_renderbuffer rboDepth;

    ogl_vertex_array quad_vao;
    ogl_vertex_buffer quad_vbo;
};

}

#endif //BCG_GRAPHICS_BCG_DEFERRED_RENDERER_H
