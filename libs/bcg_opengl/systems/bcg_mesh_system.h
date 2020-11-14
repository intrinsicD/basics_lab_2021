//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_SYSTEM_H
#define BCG_GRAPHICS_BCG_MESH_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct mesh_system : public system {
    explicit mesh_system(viewer_state *state);

    void on_setup_mesh(const event::mesh::setup &event);

    //------------------------------------------------------------------------------------------------------------------

    void on_make_triangle(const event::mesh::make_triangle &event);

    void on_make_quad(const event::mesh::make_quad &event);

    //------------------------------------------------------------------------------------------------------------------

    void on_vertex_normal_uniform(const event::mesh::vertex_normals::uniform &event);

    void on_vertex_normal_area(const event::mesh::vertex_normals::area &event);

    void on_vertex_normal_angle(const event::mesh::vertex_normals::angle &event);

    void on_vertex_normal_area_angle(const event::mesh::vertex_normals::area_angle &event);

    //------------------------------------------------------------------------------------------------------------------

    void on_dihedral_angle(const event::mesh::edge::dihedral_angle &event);
};

}

#endif //BCG_GRAPHICS_BCG_MESH_SYSTEM_H
