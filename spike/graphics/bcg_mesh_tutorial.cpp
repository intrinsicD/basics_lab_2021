//
// Created by alex on 02.03.21.
//

#include "geometry/mesh/bcg_mesh_face_normals.h"
#include "geometry/mesh/bcg_mesh_vertex_normals.h"
#include "bcg_property_map_eigen.h"
#include "bcg_mesh_tutorial.h"
#include "tbb/tbb.h"

namespace bcg {

void tutorial(halfedge_mesh &mesh, size_t parallel_grain_size) {
    auto positions = mesh.positions;
    auto new_positions = mesh.vertices.get_or_add<VectorS<3>, 3>("v_new_position");

    auto v_normals = mesh.vertices.get_or_add<VectorS<3>, 3>("v_normal");
    auto normals = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal");
    auto new_normals = mesh.faces.get_or_add<VectorS<3>, 3>("f_new_normal");


    /* you can get or add properties to
     * vertices:
     *      auto property = mesh.vertices.get<VectorS<3>, 3>("v_name"); // is nullptr if non-existent
     *      auto property = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_name"); // is constructed if non-existent
     * halfedges:
     *      auto property = mesh.halfedges.get<VectorS<3>, 3>("v_name"); // is nullptr if non-existent
     *      auto property = mesh.halfedges.get_or_add<bcg_scalar_t, 1>("v_name"); // is constructed if non-existent
     * edges:
     *      auto property = mesh.edges.get<VectorS<3>, 3>("v_name"); // is nullptr if non-existent
     *      auto property = mesh.edges.get_or_add<bcg_scalar_t, 1>("v_name"); // is constructed if non-existent
     * faces:
     *      auto property = mesh.faces.get<VectorS<3>, 3>("v_name"); // is nullptr if non-existent
     *      auto property = mesh.faces.get_or_add<bcg_scalar_t, 1>("v_name"); // is constructed if non-existent
     * */


    /// iterate over all vertices
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);

                    /* you can query the adjacent primitives of v as follows
                     * vertices:
                     *      for(const auto vv : mesh.halfedge_graph::get_vertices(v)){}
                     * halfedges:
                     *      for(const auto hv : mesh.halfedge_graph::get_halfedges(v)){}
                     * faces:
                     *      for(const auto fv : mesh.get_faces(v)){}
                     *
                     * */
                }
            }
    );
    /// iterate over all faces
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);

                    new_normals[f] = VectorS<3>::Zero();
                    int count = 0;
                    for (const auto hf : mesh.get_halfedges(f)) {
                        if (!mesh.is_boundary(hf)) {
                            auto oh = mesh.get_opposite(hf);
                            auto ff = mesh.get_face(oh);
                            new_normals[f] += normals[ff];
                            ++count;
                        }
                    }
                    new_normals[f] /= count;

                    /* you can query the adjacent primitives of f as follows
                     * vertices:
                     *      for(const auto vf : mesh.get_vertices(f)){}
                     * halfedges:
                     *      for(const auto hf : mesh.get_halfedges(f)){}
                     *
                     * the opposite halfedge of h is queried with
                     *      auto oh = mesh.get_opposite(h);
                     * check if a vertex/halfedge/edge/face is on a boundary if the mesh has a boundary
                     *      if(mesh.is_boundary(v)){}
                     *      if(mesh.is_boundary(h)){}
                     *      if(mesh.is_boundary(e)){}
                     *      if(mesh.is_boundary(f)){}
                     * */
                }
            }
    );

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);

                    v_normals[v].setZero();
                    for(const auto fv : mesh.get_faces(v)){
                        v_normals[v] += normals[fv];
                    }
                    v_normals[v].normalize();

                    /* you can query the adjacent primitives of v as follows
                     * vertices:
                     *      for(const auto vv : mesh.halfedge_graph::get_vertices(v)){}
                     * halfedges:
                     *      for(const auto hv : mesh.halfedge_graph::get_halfedges(v)){}
                     * faces:
                     *      for(const auto fv : mesh.get_faces(v)){}
                     *
                     * */
                }
            }
    );
    Map(normals) = MapConst(new_normals);
    normals.set_dirty();
    v_normals.set_dirty();
    //positions.set_dirty() // to upload positions or other property to gpu and update visuals
}

void mesh_geodesic_approx(halfedge_mesh &mesh, const mesh_tutorial_parameters &params, size_t parallel_grain_size) {
    tutorial(mesh, parallel_grain_size);
    //test
}

}