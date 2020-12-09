//
// Created by alex on 28.10.20.
//

#include "bcg_viewer_state.h"

#include <utility>
#include <iostream>
#include "glad/glad.h"

#include "events/bcg_events_internal.h"
#include "bcg_opengl/systems/bcg_camera_system.h"
#include "bcg_opengl/systems/bcg_gpu_system.h"
#include "bcg_opengl/systems/bcg_picker_system.h"
#include "bcg_opengl/systems/bcg_point_cloud_system.h"
#include "bcg_opengl/systems/bcg_graph_system.h"
#include "bcg_opengl/systems/bcg_mesh_system.h"
#include "bcg_opengl/systems/bcg_mouse_system.h"
#include "bcg_opengl/systems/bcg_keyboard_system.h"
#include "bcg_opengl/systems/bcg_transform_system.h"
#include "bcg_opengl/systems/bcg_aligned_box_system.h"
#include "bcg_opengl/systems/bcg_loading_system.h"
#include "bcg_opengl/systems/bcg_bezier_curve_system.h"
#include "bcg_opengl/systems/bcg_hierarchy_system.h"
#include "bcg_opengl/systems/bcg_spatial_index_system.h"
#include "bcg_opengl/systems/bcg_correspondence_system.h"
#include "bcg_opengl/systems/bcg_registration_system.h"
#include "bcg_opengl/renderers/bcg_render_system.h"

#include "geometry/curve/bcg_curve_bezier.h"

namespace bcg {

bool viewer_systems::has(const std::string &name) const {
    return systems.find(name) != systems.end();
}

std::unique_ptr<system> &viewer_systems::operator[](const std::string &name) {
    return systems[name];
}

const std::unique_ptr<system> &viewer_systems::operator[](const std::string &name) const {
    return systems.at(name);
}

glsl_program viewer_shaders::load(std::string name,
                                  std::string vertex_shader_file,
                                  std::string fragment_shader_file,
                                  std::string *geometry_shader_file,
                                  std::string *tess_control_shader_file,
                                  std::string *tess_eval_shader_file) {
    glsl_program program(std::move(name));
    std::vector<glsl_shader> shaders;
    shaders.push_back(load_shader(std::move(vertex_shader_file), GL_VERTEX_SHADER));
    shaders.push_back(load_shader(std::move(fragment_shader_file), GL_FRAGMENT_SHADER));
    if (geometry_shader_file) {
        shaders.push_back(load_shader(*geometry_shader_file, GL_GEOMETRY_SHADER));
    }
    if (tess_control_shader_file) {
        shaders.push_back(load_shader(*tess_control_shader_file, GL_TESS_CONTROL_SHADER));
    }
    if (tess_eval_shader_file) {
        shaders.push_back(load_shader(*tess_eval_shader_file, GL_TESS_EVALUATION_SHADER));
    }

    std::cout << vertex_shader_file;

    program.build(shaders);

    for (const auto &shader : shaders) {
        watcher.watch(shader.filename, [shader, program]() {
            glsl_shader new_shader = shader;
            program.detach(shader);
            new_shader.compile_file(shader.filename.c_str(), shader.type);
            std::cout << "reload program: " << shader.filename << "\n";
            if (!program.is_valid()) return;
            program.attach(new_shader);
            if (program.link()) return;
        });
    }
    programs[name] = program;
    return programs[name];
}

glsl_shader viewer_shaders::load_shader(std::string filename, unsigned int type) const {
    glsl_shader shader;
    shader.compile_file(filename.c_str(), type);
    return shader;
}

glsl_program &viewer_shaders::operator[](const std::string &name) {
    return programs[name];
}

const glsl_program &viewer_shaders::operator[](const std::string &name) const {
    return programs.at(name);
}

viewer_shaders::viewer_shaders(viewer_state *state) : state(state) {
    this->state->dispatcher.sink<event::internal::update>().connect<&viewer_shaders::on_update>(this);
}

void viewer_shaders::on_update(const event::internal::update &){
    watcher.trigger();
}

viewer_state::viewer_state() : shaders(this){
    systems["camera_system"] = std::make_unique<camera_system>(this);
    systems["loading_system"] = std::make_unique<loading_system>(this);
    systems["point_cloud_system"] = std::make_unique<point_cloud_system>(this);
    systems["graph_system"] = std::make_unique<graph_system>(this);
    systems["picker_system"] = std::make_unique<picker_system>(this);
    systems["mesh_system"] = std::make_unique<mesh_system>(this);
    systems["render_system"] = std::make_unique<render_system>(this);
    systems["gpu_system"] = std::make_unique<gpu_system>(this);
    systems["mouse_system"] = std::make_unique<mouse_system>(this);
    systems["keyboard_system"] = std::make_unique<keyboard_system>(this);
    systems["transform_system"] = std::make_unique<transform_system>(this);
    systems["aligned_box_system"] = std::make_unique<aligned_box_system>(this);
    systems["bezier_curve_system"] = std::make_unique<bezier_curve_system>(this);
    systems["hierarchy_system"] = std::make_unique<hierarchy_system>(this);
    systems["spatial_index_system"] = std::make_unique<spatial_index_system>(this);
    systems["correspondence_system"] = std::make_unique<correspondence_system>(this);
    systems["registration_system"] = std::make_unique<registration_system>(this);
}

VectorS<4> viewer_state::get_viewport() const{
    Vector<int, 4> vp(Vector<int, 4>::Zero());
    glGetIntegerv(GL_VIEWPORT, vp.data());
    return vp.cast<bcg_scalar_t>();
}

vertex_container *viewer_state::get_vertices(entt::entity id) {
    vertex_container *vertices = nullptr;
    if (scene.valid(id)) {
        auto *pc = scene.try_get<point_cloud>(id);
        if (pc) {
            vertices = &pc->vertices;
        } else {
            auto *graph = scene.try_get<halfedge_graph>(id);
            if (graph) {
                vertices = &graph->vertices;
            } else {
                auto *curve = scene.try_get<curve_bezier>(id);
                if (curve) {
                    vertices = &curve->vertices;
                } else {
                    auto *mesh = scene.try_get<halfedge_mesh>(id);
                    if (mesh) {
                        vertices = &mesh->vertices;
                    }
                }
            }
        }
    }
    return vertices;
}

halfedge_container *viewer_state::get_halfedges(entt::entity id) {
    halfedge_container *halfedges = nullptr;
    if (scene.valid(id)) {
        auto *graph = scene.try_get<halfedge_graph>(id);
        if (graph) {
            halfedges = &graph->halfedges;
        } else {
            auto *curve = scene.try_get<curve_bezier>(id);
            if (curve) {
                halfedges = &curve->halfedges;
            } else {
                auto *mesh = scene.try_get<halfedge_mesh>(id);
                if (mesh) {
                    halfedges = &mesh->halfedges;
                }
            }
        }
    }
    return halfedges;
}

edge_container *viewer_state::get_edges(entt::entity id) {
    edge_container *edges = nullptr;
    if (scene.valid(id)) {
        auto *graph = scene.try_get<halfedge_graph>(id);
        if (graph) {
            edges = &graph->edges;
        } else {
            auto *curve = scene.try_get<curve_bezier>(id);
            if (curve) {
                edges = &curve->edges;
            } else {
                auto *mesh = scene.try_get<halfedge_mesh>(id);
                if (mesh) {
                    edges = &mesh->edges;
                }
            }
        }
    }
    return edges;
}

face_container *viewer_state::get_faces(entt::entity id) {
    face_container *faces = nullptr;
    if (scene.valid(id)) {
        auto *mesh = scene.try_get<halfedge_mesh>(id);
        if (mesh) {
            faces = &mesh->faces;
        }
    }
    return faces;
}

}