//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_RENDERER_H
#define BCG_GRAPHICS_BCG_RENDERER_H

#include <unordered_map>
#include <vector>
#include <string>

#include "bcg_opengl/systems/bcg_systems.h"
#include "bcg_opengl/bcg_opengl.h"
#include "entt/entt.hpp"

namespace bcg {

struct viewer_state;

struct renderer : public system {
    explicit renderer(viewer_state *state = nullptr);

    renderer(std::string name, viewer_state *state);

    ~renderer() override = default;

    static std::vector<VectorS<3>>
    map_to_colors(property_container *container, const std::string &property_name, colormap::base_colormap color_map = colormap::jet());

protected:
    virtual void on_begin_frame(const event::internal::begin_frame &);

    virtual void on_render(const event::internal::render &);

    virtual void on_end_frame(const event::internal::end_frame &);


    std::unordered_map<std::string, glsl_program> programs;
    std::vector<entt::entity> entities_to_draw;
    ogl_state gl_state;
};

}

#endif //BCG_GRAPHICS_BCG_RENDERER_H
