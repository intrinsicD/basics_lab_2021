//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FACTORY_H
#define BCG_GRAPHICS_BCG_MESH_FACTORY_H

#include "bcg_mesh.h"
#include "triangle/bcg_triangle.h"

namespace bcg{
    struct mesh_factory{
        mesh_factory() = default;

        halfedge_mesh make_triangle();

        halfedge_mesh make_triangle(const triangle3 &triangle);
    };
}

#endif //BCG_GRAPHICS_BCG_MESH_FACTORY_H
