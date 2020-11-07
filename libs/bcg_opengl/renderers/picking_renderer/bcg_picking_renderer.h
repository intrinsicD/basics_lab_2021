//
// Created by alex on 06.11.20.
//

#ifndef BCG_GRAPHICS_BCG_PICKING_RENDERER_H
#define BCG_GRAPHICS_BCG_PICKING_RENDERER_H

#include "renderers/bcg_renderer.h"
#include "bcg_events_picking_renderer.h"
#include "events/bcg_events.h"

namespace bcg {

struct picking_renderer : public renderer {
    explicit picking_renderer(viewer_state *state);

private:
    void on_startup(const event::internal::startup &event);

    void on_enqueue(const event::picking_renderer::enqueue &event);

    void on_begin_frame(const event::internal::begin_frame &event) override;

    void on_mouse_button(const event::mouse::button &event);

    void on_end_frame(const event::internal::end_frame &event) override;
};

}

#endif //BCG_GRAPHICS_BCG_PICKING_RENDERER_H
