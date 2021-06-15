//
// Created by alex on 15.06.21.
//

#ifndef BCG_GRAPHICS_BCG_JAW_H
#define BCG_GRAPHICS_BCG_JAW_H

#include "mesh/bcg_mesh.h"
#include "parameterized_plane/bcg_parameterized_plane.h"

namespace bcg{

halfedge_graph mesh_archform(halfedge_mesh &mesh);

ParameterizedPlane mesh_occlusal_plane(halfedge_mesh &mesh);

}


#endif //BCG_GRAPHICS_BCG_JAW_H
