//
// Created by alex on 29.11.20.
//

#ifndef BCG_GRAPHICS_BCG_SELECTION_H
#define BCG_GRAPHICS_BCG_SELECTION_H

#include <unordered_map>
#include "mesh/bcg_mesh.h"

namespace bcg{
struct selected_points{
    std::vector<VectorS<3>> selected;
};
struct selected_vertices{
    std::unordered_map<size_t, vertex_handle> selected;
};
struct selected_edges{
    std::unordered_map<size_t, edge_handle> selected;
};
struct selected_faces{
    std::unordered_map<size_t, face_handle> selected;
};
}

#endif //BCG_GRAPHICS_BCG_SELECTION_H
