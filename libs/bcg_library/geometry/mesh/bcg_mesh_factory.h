//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FACTORY_H
#define BCG_GRAPHICS_BCG_MESH_FACTORY_H

#include "bcg_mesh.h"
#include "triangle/bcg_triangle.h"
#include "quad/bcg_quad.h"
#include "aligned_box/bcg_aligned_box.h"
#include "parameterized_plane/bcg_parameterized_plane.h"

namespace bcg{
    struct mesh_factory{
        mesh_factory() = default;

        halfedge_mesh make_triangle();

        halfedge_mesh make_triangle(const triangle3 &triangle);

        halfedge_mesh make_quad();

        halfedge_mesh make_quad(const quad3 &quad);

        halfedge_mesh make_parameterized_plane();

        halfedge_mesh make_parameterized_plane(const ParameterizedPlane3 &plane);

        halfedge_mesh make_icosahedron();

        halfedge_mesh make_tetrahedron();

        halfedge_mesh make_hexahedron();

        halfedge_mesh make_octahedron();

        halfedge_mesh make_dodecahedron();

        halfedge_mesh make_box();

        halfedge_mesh make_box(const aligned_box3 &aabb);
    };
}

#endif //BCG_GRAPHICS_BCG_MESH_FACTORY_H
