//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_GPU_H
#define BCG_GRAPHICS_BCG_EVENTS_GPU_H

#include <vector>
#include <string>
#include "entt/entt.hpp"
#include "bcg_opengl/renderers/bcg_attribute.h"
#include "bcg_property.h"
#include "color/bcg_colormap.h"

namespace bcg::event::gpu{

struct update_property{
    entt::entity id;
    property_container *container;
    attribute attrib;
    colormap::base_colormap color_map;
};
struct update_vertex_attributes {
    entt::entity id;
    std::vector<attribute> attributes;
};
struct update_vertex_colors_scalarfield {
    entt::entity id;
    property_container *container;
    attribute color;
    colormap::base_colormap color_map;
    bcg_scalar_t min, max;
};
struct update_edge_attributes {
    entt::entity id;
    std::vector<attribute> attributes;
};
struct update_face_attributes {
    entt::entity id;
    std::vector<attribute> attributes;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_GPU_H
