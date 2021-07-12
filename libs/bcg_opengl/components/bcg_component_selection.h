//
// Created by alex on 29.11.20.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_SELECTION_H
#define BCG_GRAPHICS_BCG_COMPONENT_SELECTION_H

#include <unordered_map>
#include <utility>
#include "mesh/bcg_mesh.h"
#include "components/bcg_component_proxy_point_cloud.h"
#include "utils/bcg_stl_utils.h"

namespace bcg {

template<typename T>
struct select {
    T element;
    size_t index_in_numbering;
};
struct selected_points {
    std::vector<VectorS<3>> selected;
};
/*struct selected_vertices {
    std::unordered_map<size_t, select<vertex_handle>> selected;
    std::vector<size_t> ordering;
};*/
struct selected_edges {
    std::unordered_map<size_t, edge_handle> selected;
};
struct selected_faces {
    std::unordered_map<size_t, face_handle> selected;
};
}

#endif //BCG_GRAPHICS_BCG_COMPONENT_SELECTION_H
