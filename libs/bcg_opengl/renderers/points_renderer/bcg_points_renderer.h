//
// Created by alex on 29.10.20.
//

#ifndef BCG_GRAPHICS_BCG_POINTS_RENDERER_H
#define BCG_GRAPHICS_BCG_POINTS_RENDERER_H

#include "renderers/bcg_renderer.h"
#include "bcg_points_renderer_events.h"
#include "systems/bcg_events.h"

namespace bcg {

struct points_renderer : public renderer {
    explicit points_renderer(viewer_state *state);

private:
    void on_startup(const event::startup &event);

    void on_enqueue(const event::points_renderer::enqueue &event);

    void on_begin_frame(const event::begin_frame &event) override;

    void on_render(const event::render &event) override;

    void on_end_frame(const event::end_frame &event) override;
};

}

#endif //BCG_GRAPHICS_BCG_POINTS_RENDERER_H
