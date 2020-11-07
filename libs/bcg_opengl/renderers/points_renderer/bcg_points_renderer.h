//
// Created by alex on 29.10.20.
//

#ifndef BCG_GRAPHICS_BCG_POINTS_RENDERER_H
#define BCG_GRAPHICS_BCG_POINTS_RENDERER_H

#include "renderers/bcg_renderer.h"
#include "bcg_events_points_renderer.h"
#include "events/bcg_events.h"

namespace bcg {

struct points_renderer : public renderer {
    explicit points_renderer(viewer_state *state);

private:
    void on_startup(const event::internal::startup &event);

    void on_enqueue(const event::points_renderer::enqueue &event);

    void on_begin_frame(const event::internal::begin_frame &event) override;

    void on_render(const event::internal::render &event) override;

    void on_end_frame(const event::internal::end_frame &event) override;

    void on_point_size(const event::internal::point_size &event);

    void on_set_point_size(const event::internal::set_point_size &event);
};

}

#endif //BCG_GRAPHICS_BCG_POINTS_RENDERER_H
