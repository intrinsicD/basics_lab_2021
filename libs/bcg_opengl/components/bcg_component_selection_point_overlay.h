//
// Created by alex on 08.07.21.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_SELECTION_POINT_OVERLAY_H
#define BCG_GRAPHICS_BCG_COMPONENT_SELECTION_POINT_OVERLAY_H

#include "utils/bcg_stl_utils.h"

namespace bcg {

struct component_selection_point_overlay {
    entt::entity parent_id = entt::null;
    entt::entity overlay_id = entt::null;
    point_cloud *overlay_point_cloud;
    size_t count_selections;

    property<VectorS<3>, 3> positions;
    property<size_t, 1> insertion_order;

    component_selection_point_overlay() = default;

    component_selection_point_overlay(entt::entity parent_id,
                                       entt::entity overlay_id,
                                       point_cloud *overlay_point_cloud)
            : parent_id(parent_id),
              overlay_id(overlay_id),
              overlay_point_cloud(overlay_point_cloud),
              count_selections(0) {
        positions = overlay_point_cloud->vertices.get_or_add<VectorS<3>, 3>("v_position");
        insertion_order = overlay_point_cloud->vertices.get_or_add<size_t, 1>("v_insertion_order");
    }

    void clear() {
        overlay_point_cloud->vertices.clear();
        count_selections = 0;
    }

    bool select(const VectorS<3> &point) {
        auto v = overlay_point_cloud->add_vertex(point);
        insertion_order[v] = count_selections++;
        return false;
    }
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_SELECTION_POINT_OVERLAY_H
