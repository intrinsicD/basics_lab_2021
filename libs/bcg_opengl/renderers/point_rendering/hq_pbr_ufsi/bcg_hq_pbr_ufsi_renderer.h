//
// Created by alex on 29.04.21.
//

#ifndef BCG_GRAPHICS_BCG_HQ_PBR_UFSI_RENDERER_H
#define BCG_GRAPHICS_BCG_HQ_PBR_UFSI_RENDERER_H

#include "bcg_opengl/renderers/bcg_renderer.h"
#include "bcg_hq_pbr_ufsi_renderer_events.h"

namespace bcg {

struct hq_pbr_ufsi_renderer : public renderer {
    explicit hq_pbr_ufsi_renderer(viewer_state *state);

private:
    void on_startup(const event::internal::startup &event);

    void on_shutdown(const event::internal::shutdown &event);

    void on_enqueue(const event::hq_pbr_ufsi_renderer::enqueue &event);

    void on_setup_material(const event::hq_pbr_ufsi_renderer::setup_material &event);

    void on_begin_frame(const event::internal::begin_frame &event) override;

    void on_render(const event::internal::render &event) override;

    void on_end_frame(const event::internal::end_frame &event) override;

    void on_set_position(const event::hq_pbr_ufsi_renderer::set_position &event);

    void on_set_color(const event::hq_pbr_ufsi_renderer::set_color &event);

    void on_set_normal(const event::hq_pbr_ufsi_renderer::set_normal &event);

    void on_set_uniform_point_size(const event::internal::uniform_point_size &event);
};


}

#endif //BCG_GRAPHICS_BCG_HQ_PBR_UFSI_RENDERER_H
