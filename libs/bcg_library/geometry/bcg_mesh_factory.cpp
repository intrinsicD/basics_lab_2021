//
// Created by alex on 14.10.20.
//

#include "bcg_mesh_factory.h"

namespace bcg{

halfedge_mesh mesh_factory::make_triangle(){
    return make_triangle(triangle3(zero3f, {0, 1, 0}, {1, 0, 0}));
}

halfedge_mesh mesh_factory::make_triangle(const triangle3 &triangle) {
    halfedge_mesh mesh;
    for(const auto &point : triangle.points){
        mesh.add_vertex(point);
    }

    mesh.add_triangle(0, 1, 2);
    return mesh;
}

}