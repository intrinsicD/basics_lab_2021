//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_TEST_RENDERER_H
#define BCG_GRAPHICS_BCG_TEST_RENDERER_H

#include "renderers/bcg_renderer.h"
#include "bcg_events_test_renderer.h"
#include "events/bcg_events.h"

namespace bcg {

struct test_renderer : public renderer {
    explicit test_renderer(viewer_state *state);

private:
    void on_startup(const event::internal::startup &event);

    void on_enqueue(const event::test_renderer::enqueue &event);

    void on_begin_frame(const event::internal::begin_frame &event) override;

    void on_render(const event::internal::render &event) override;

    void on_end_frame(const event::internal::end_frame &event) override;
};

}

#endif //BCG_GRAPHICS_BCG_TEST_RENDERER_H
