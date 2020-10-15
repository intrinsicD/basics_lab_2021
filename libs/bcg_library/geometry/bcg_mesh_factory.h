//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FACTORY_H
#define BCG_GRAPHICS_BCG_MESH_FACTORY_H

#include "bcg_mesh.h"
#include "triangle/bcg_triangle.h"
#include "quad/bcg_quad.h"

namespace bcg{
    struct mesh_factory{
        mesh_factory() = default;

        halfedge_mesh make_triangle();

        halfedge_mesh make_triangle(const triangle3 &triangle);

        halfedge_mesh make_quad();

        halfedge_mesh make_quad(const quad3 &quad);

        halfedge_mesh make_icosahedron();
    };
}

#endif //BCG_GRAPHICS_BCG_MESH_FACTORY_H
