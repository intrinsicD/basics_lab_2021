//
// Created by alex on 31.05.21.
//

#include "bcg_gui_mesh_factory.h"
#include "viewer/bcg_viewer_state.h"
#include "mesh/bcg_mesh_factory.h"

namespace bcg{

void gui_mesh_factory(viewer_state *state){
    mesh_factory factory;
    if(ImGui::Button("Triangle")){
        auto mesh = factory.make_triangle();
        auto id = state->scene.create();
        state->scene().emplace<halfedge_mesh>(id, mesh);
        state->dispatcher.trigger<event::mesh::setup>(id, "");
    }
    if(ImGui::Button("Quad")){
        auto mesh = factory.make_quad();
        auto id = state->scene.create();
        state->scene().emplace<halfedge_mesh>(id, mesh);
        state->dispatcher.trigger<event::mesh::setup>(id, "");
    }
    if(ImGui::Button("Parameterized Plane")){
        auto mesh = factory.make_parameterized_plane();
        auto id = state->scene.create();
        state->scene().emplace<halfedge_mesh>(id, mesh);
        state->dispatcher.trigger<event::mesh::setup>(id, "");
    }
    if(ImGui::Button("Icosahedron")){
        auto mesh = factory.make_icosahedron();
        auto id = state->scene.create();
        state->scene().emplace<halfedge_mesh>(id, mesh);
        state->dispatcher.trigger<event::mesh::setup>(id, "");
    }
    if(ImGui::Button("Tetrahedron")){
        auto mesh = factory.make_tetrahedron();
        auto id = state->scene.create();
        state->scene().emplace<halfedge_mesh>(id, mesh);
        state->dispatcher.trigger<event::mesh::setup>(id, "");
    }
    if(ImGui::Button("Hexahedron")){
        auto mesh = factory.make_hexahedron();
        auto id = state->scene.create();
        state->scene().emplace<halfedge_mesh>(id, mesh);
        state->dispatcher.trigger<event::mesh::setup>(id, "");
    }
    if(ImGui::Button("Octahedron")){
        auto mesh = factory.make_octahedron();
        auto id = state->scene.create();
        state->scene().emplace<halfedge_mesh>(id, mesh);
        state->dispatcher.trigger<event::mesh::setup>(id, "");
    }
    if(ImGui::Button("Dodecahedron")){
        auto mesh = factory.make_dodecahedron();
        auto id = state->scene.create();
        state->scene().emplace<halfedge_mesh>(id, mesh);
        state->dispatcher.trigger<event::mesh::setup>(id, "");
    }
    if(ImGui::Button("Box")){
        auto mesh = factory.make_box();
        auto id = state->scene.create();
        state->scene().emplace<halfedge_mesh>(id, mesh);
        state->dispatcher.trigger<event::mesh::setup>(id, "");
    }
}

}