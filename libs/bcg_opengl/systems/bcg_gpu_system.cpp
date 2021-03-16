//
// Created by alex on 29.10.20.
//

#include "bcg_gpu_system.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_library/math/matrix/bcg_matrix_map_eigen.h"
#include "geometry/bcg_property_map_eigen.h"
#include "geometry/curve/bcg_curve_bezier.h"
#include "bcg_library/geometry/mesh/bcg_mesh.h"
#include "utils/bcg_string_utils.h"
#include "renderers/bcg_renderer.h"

namespace bcg {

gpu_system::gpu_system(viewer_state *state) : system("gpu_system", state) {
    state->dispatcher.sink<event::gpu::update_property>().connect<&gpu_system::on_update_property>(this);

    state->dispatcher.sink<event::gpu::update_vertex_attributes>().connect<&gpu_system::on_update_vertex_attributes>(
            this);
    state->dispatcher.sink<event::gpu::update_edge_attributes>().connect<&gpu_system::on_update_edge_attributes>(
            this);
    state->dispatcher.sink<event::gpu::update_face_attributes>().connect<&gpu_system::on_update_face_attributes>(
            this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&gpu_system::on_shutdown>(this);
}

void gpu_system::on_update_property(const event::gpu::update_property &event) {
    if (!state->scene.valid(event.id)) return;
    if (!event.container->has(event.attrib.property_name)) return;

    if (!event.container->get_base_ptr(event.attrib.property_name)->is_dirty() && !event.attrib.update) return;
    auto &shape = state->scene.get_or_emplace<ogl_shape>(event.id);
    ogl_buffer_object *buffer = nullptr;
    if (event.attrib.property_name == "edges") {
        buffer = &shape.edge_buffer;
    } else if (event.attrib.property_name == "triangles") {
        buffer = &shape.triangle_buffer;
    } else if (event.attrib.property_name == "triangles_adjacency") {
        buffer = &shape.adjacency_buffer;
    } else {
        buffer = &shape.vertex_buffers[event.attrib.buffer_name];
    }
    if (buffer == nullptr) return;

    if (!buffer->is_valid()) {
        buffer->name = event.attrib.buffer_name;
        buffer->create();
    }
    buffer->bind();

    auto *base_ptr = event.container->get_base_ptr(event.attrib.property_name);
    if (event.attrib.shader_attribute_name == "color" && base_ptr->dims() == 1) {
        auto colors = renderer::map_to_colors(event.container, event.attrib.property_name, event.color_map);
        buffer->upload(colors[0].data(), base_ptr->size(), 3, 0, true);
    } else {
        if (base_ptr->void_ptr() != nullptr) {
            if(base_ptr->type() == property_types::Type::DOUBLE || base_ptr->type() == property_types::Type::FLOAT){
                Matrix<float, -1, -1> DATAF = Map<bcg_scalar_t>(base_ptr).transpose().cast<float>();
                if(event.attrib.shader_attribute_name == "color"){
                    DATAF.array() -= DATAF.array().minCoeff();
                    DATAF.array() /= DATAF.array().maxCoeff();
                }
                if(event.attrib.shader_attribute_name == "point_size"){
                    DATAF.array() -= DATAF.array().minCoeff();
                    DATAF.array() /= DATAF.array().maxCoeff();
                    DATAF.array() *= (state->config.max_point_size - 1);
                    DATAF.array() += 1.0;
                }
                buffer->upload((const void *) DATAF.data(), base_ptr->size(), base_ptr->dims(), 0, true);
            }else{
                buffer->upload(base_ptr->void_ptr(), base_ptr->size(), base_ptr->dims(), 0, true);
            }
        } else {
            std::cerr << "vector of bools?\n";
        }
    }
    base_ptr->set_clean();
    buffer->release();
}

void gpu_system::on_update_vertex_attributes(const event::gpu::update_vertex_attributes &event) {
    if (!state->scene.valid(event.id)) return;

    property_container *vertices = state->get_vertices(event.id);
    if (vertices) {
        auto &shape = state->scene.get_or_emplace<ogl_shape>(event.id);
        shape.num_vertices = vertices->size();
        auto &attributes = event.attributes;

        colormap::jet color_map;

        for (auto &attribute : attributes) {
            if (attribute.index < 0) continue;
            state->dispatcher.trigger<event::gpu::update_property>(event.id, vertices, attribute, color_map);
        }
    }
}

void gpu_system::on_update_edge_attributes(const event::gpu::update_edge_attributes &event) {
    if (!state->scene.valid(event.id)) return;

    property_container *edges = state->get_edges(event.id);
    property_container *halfedges = state->get_halfedges(event.id);

    if (edges && halfedges) {
        const auto &attributes = event.attributes;
        for (const auto &attribute : attributes) {

            if (halfedges->get<halfedge_graph::halfedge_connectivity, 4>("h_connectivity").is_dirty() &&
                attribute.property_name == "edges") {
                property<VectorI<2>, 2> property;
                if (state->scene.all_of<halfedge_mesh>(event.id)) {
                    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
                    property = mesh.edges.get_or_add<VectorI<2>, 2>("edges");
                    property.vector() = mesh.get_connectivity();
                } else if (state->scene.all_of<halfedge_graph>(event.id)) {
                    auto &graph = state->scene.get<halfedge_graph>(event.id);
                    property = graph.edges.get_or_add<VectorI<2>, 2>("edges");
                    property.vector() = graph.get_connectivity();
                } else if (state->scene.all_of<curve_bezier>(event.id)) {
                    auto &curve = state->scene.get<curve_bezier>(event.id);
                    property = curve.edges.get_or_add<VectorI<2>, 2>("edges");
                    property.vector() = curve.get_connectivity();
                }

                halfedges->get<halfedge_graph::halfedge_connectivity, 4>("h_connectivity").set_clean();
            }
            if (attribute.buffer_name == "e_position") {
                property<VectorS<3>, 3> property;
                if (state->scene.all_of<halfedge_mesh>(event.id)) {
                    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
                    property = mesh.edges.get_or_add<VectorS<3>, 3>("e_position");
                    auto connectivity = mesh.get_connectivity();
                    for (const auto e : mesh.edges) {
                        property[e] =
                                (mesh.positions[connectivity[e.idx][0]] + mesh.positions[connectivity[e.idx][1]]) / 2.0;
                    }
                } else if (state->scene.all_of<halfedge_graph>(event.id)) {
                    auto &graph = state->scene.get<halfedge_graph>(event.id);
                    property = graph.edges.get_or_add<VectorS<3>, 3>("e_position");
                    auto connectivity = graph.get_connectivity();
                    for (const auto e : graph.edges) {
                        property[e] = (graph.positions[connectivity[e.idx][0]] +
                                       graph.positions[connectivity[e.idx][1]]) / 2.0;
                    }
                }
            }

            state->dispatcher.trigger<event::gpu::update_property>(event.id, edges, attribute,
                                                                   colormap::base_colormap());
        }
    }
}

void gpu_system::on_update_face_attributes(const event::gpu::update_face_attributes &event) {
    if (!state->scene.valid(event.id)) return;

    property_container *faces = state->get_faces(event.id);

    if (faces) {
        const auto &attributes = event.attributes;
        for (const auto &attribute : attributes) {
            if (faces->get<halfedge_mesh::face_connectivity, 1>("f_connectivity").is_dirty()) {
                auto &mesh = state->scene.get<halfedge_mesh>(event.id);
                if (attribute.property_name == "triangles") {
                    auto property = mesh.faces.get_or_add<VectorI<3>, 3>("triangles");
                    property = mesh.get_triangles();
                }
                if (attribute.property_name == "triangles_adjacency") {
                    auto property = mesh.faces.get_or_add<VectorI<6>, 6>("triangles_adjacency");
                    property = mesh.get_triangles_adjacencies();
                }
                faces->get<halfedge_mesh::face_connectivity, 1>("f_connectivity").set_clean();
            }

            if (attribute.buffer_name == "f_position") {
                property<VectorS<3>, 3> property;
                if (state->scene.all_of<halfedge_mesh>(event.id)) {
                    auto &mesh = state->scene.get<halfedge_mesh>(event.id);
                    property = mesh.faces.get_or_add<VectorS<3>, 3>("f_position");
                    auto triangles = mesh.get_triangles();
                    for (const auto f : mesh.faces) {
                        property[f] = (mesh.positions[triangles[f.idx][0]]
                                       + mesh.positions[triangles[f.idx][1]]
                                       + mesh.positions[triangles[f.idx][2]]) / 3.0;
                    }
                }
            }

            state->dispatcher.trigger<event::gpu::update_property>(event.id, faces, attribute,
                                                                   colormap::base_colormap());
        }

    }
}

void gpu_system::on_shutdown(const event::internal::shutdown &event) {
    auto view = state->scene.view<ogl_shape>();
    for (const auto id : view) {
        auto &shape = view.get<ogl_shape>(id);
        for (auto &item : shape.vertex_buffers) {
            item.second.destroy();
        }
        shape.edge_buffer.destroy();
        shape.triangle_buffer.destroy();
        shape.adjacency_buffer.destroy();
    }
}

}