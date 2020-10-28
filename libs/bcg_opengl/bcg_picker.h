//
// Created by alex on 26.10.20.
//

#ifndef BCG_GRAPHICS_BCG_PICKER_H
#define BCG_GRAPHICS_BCG_PICKER_H

#include "bcg_linalg.h"

namespace bcg{

struct viewer_picker{
    size_t entity_id, vertex_id, edge_id, face_id;
    VectorS<3> model_space_point;
    VectorS<3> world_space_point;
    VectorS<3> view_space_point;
};

}

#endif //BCG_GRAPHICS_BCG_PICKER_H
