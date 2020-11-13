//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_CURVE_RENDERER_H
#define BCG_GRAPHICS_BCG_CURVE_RENDERER_H

#include "renderers/bcg_renderer.h"
#include "bcg_events_curve_renderer.h"
#include "events/bcg_events.h"

namespace bcg {

struct curve_renderer : public renderer {
    explicit curve_renderer(viewer_state *state);

private:
    void on_startup(const event::internal::startup &event);

    void on_shutdown(const event::internal::shutdown &event);

    void on_enqueue(const event::curve_renderer::enqueue &event);

    void on_setup_for_rendering(const event::curve_renderer::setup_for_rendering &event);

    void on_begin_frame(const event::internal::begin_frame &event) override;

    void on_render(const event::internal::render &event) override;

    void on_end_frame(const event::internal::end_frame &event) override;

    void on_set_position_attribute(const event::curve_renderer::set_position_attribute &event);
};

}

#endif //BCG_GRAPHICS_BCG_CURVE_RENDERER_H
