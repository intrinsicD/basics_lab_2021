//
// Created by alex on 08.07.21.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_SELECTION_VERTEX_OVERLAY_H
#define BCG_GRAPHICS_BCG_COMPONENT_SELECTION_VERTEX_OVERLAY_H

#include "utils/bcg_stl_utils.h"

namespace bcg {

struct component_selection_vertex_overlay {
    entt::entity parent_id = entt::null;
    entt::entity overlay_id = entt::null;
    vertex_container *parent_vertices;
    point_cloud *overlay_point_cloud;
    size_t count_selections;

    property<VectorS<3>, 3> parent_positions;
    property<bool, 1> parent_selected;
    property<VectorS<3>, 3> positions;
    property<size_t, 1> insertion_order;
    property<size_t, 1> index_order;
    property<size_t, 1> parent_indices;
    property<size_t, 1> overlay_indices;

    component_selection_vertex_overlay() = default;

    component_selection_vertex_overlay(entt::entity parent_id,
                                       entt::entity overlay_id,
                                       vertex_container *parent_vertices,
                                       point_cloud *overlay_point_cloud)
            : parent_id(parent_id),
              overlay_id(overlay_id),
              parent_vertices(parent_vertices),
              overlay_point_cloud(overlay_point_cloud),
              count_selections(0) {
        parent_positions = parent_vertices->get<VectorS<3>, 3>("v_position");
        parent_selected = parent_vertices->get_or_add<bool, 1>("v_selected");
        overlay_indices = parent_vertices->get_or_add<size_t, 1>("v_overlay_indices");
        positions = overlay_point_cloud->vertices.get_or_add<VectorS<3>, 3>("v_position");
        insertion_order = overlay_point_cloud->vertices.get_or_add<size_t, 1>("v_insertion_order");
        index_order = overlay_point_cloud->vertices.get_or_add<size_t, 1>("v_index_order");
        parent_indices = overlay_point_cloud->vertices.get_or_add<size_t, 1>("v_parent_indices");
    }

    void clear() {
        overlay_point_cloud->vertices.clear();
        parent_selected.reset(false);
        count_selections = 0;
    }

    bool select(vertex_handle parent_vertex_index) {
        auto iter = std::find(parent_indices.begin(), parent_indices.end(), parent_vertex_index);
        if (iter == parent_indices.end()) {
            vertex_handle v = overlay_point_cloud->add_vertex(parent_positions[parent_vertex_index]);
            insertion_order.back() = count_selections++;

            parent_indices[v] = parent_vertex_index;
            overlay_indices[parent_vertex_index] = v;

            parent_selected[parent_vertex_index] = true;
            index_order.back() = v;

            positions.set_dirty();
            insertion_order.set_dirty();
            parent_selected.set_dirty();
            index_order.set_dirty();
            return true;
        }
        return false;
    }

    bool deselect(vertex_handle parent_vertex_index) {
        auto iter = std::find(parent_indices.begin(), parent_indices.end(), parent_vertex_index);
        if (iter != parent_indices.end()) {
            if(!overlay_point_cloud->empty()){
                overlay_point_cloud->delete_vertex(overlay_indices[*iter]);
                overlay_point_cloud->garbage_collection();
                for(const auto v : overlay_point_cloud->vertices){
                    overlay_indices[parent_indices[v]] = v;
                }
            }
            parent_selected[parent_vertex_index] = false;

            positions.set_dirty();
            insertion_order.set_dirty();
            parent_selected.set_dirty();
            index_order.set_dirty();
            return true;
        }
        return false;
    }

    void sort_index_order_ascending(std::vector<bcg_scalar_t> &weighting) {
        for (const auto v : overlay_point_cloud->vertices) {
            index_order[v] = v;
        }
        sort_by_first(weighting, index_order.vector(), false);
    }

    void sort_index_order_descending(std::vector<bcg_scalar_t> &weighting) {
        for (const auto v : overlay_point_cloud->vertices) {
            index_order[v] = v;
        }
        sort_by_first(weighting, index_order.vector(), true);
    }
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_SELECTION_VERTEX_OVERLAY_H
