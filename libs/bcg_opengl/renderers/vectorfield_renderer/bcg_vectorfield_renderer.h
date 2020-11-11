//
// Created by alex on 11.11.20.
//

#ifndef BCG_GRAPHICS_BCG_VECTORFIELD_RENDERER_H
#define BCG_GRAPHICS_BCG_VECTORFIELD_RENDERER_H

#include "renderers/bcg_renderer.h"
#include "bcg_events_vectorfield_renderer.h"
#include "events/bcg_events.h"

namespace bcg {

struct vectorfield_renderer : public renderer {
    explicit vectorfield_renderer(viewer_state *state);

private:
    void on_startup(const event::internal::startup &event);

    void on_shutdown(const event::internal::shutdown &event);

    void on_enqueue(const event::vectorfield_renderer::enqueue &event);

    void on_setup_for_rendering(const event::vectorfield_renderer::setup_for_rendering &event);

    void on_begin_frame(const event::internal::begin_frame &event) override;

    void on_render(const event::internal::render &event) override;

    void on_end_frame(const event::internal::end_frame &event) override;

    void on_set_position_attribute(const event::vectorfield_renderer::set_position_attribute &event);

    void on_set_vector_attribute(const event::vectorfield_renderer::set_vector_attribute &event);

    void on_set_color_attribute(const event::vectorfield_renderer::set_color_attribute &event);
};

}

#endif //BCG_GRAPHICS_BCG_VECTORFIELD_RENDERER_H
