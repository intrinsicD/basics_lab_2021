//
// Created by alex on 02.03.21.
//

#include "bcg_mesh_normal_filtering.h"
#include "tbb/tbb.h"

namespace bcg{

void tutorial(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto positions = mesh.positions;

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

    //positions.set_dirty() // to upload positions or other property to gpu and update visuals
}

void mesh_normal_filtering(halfedge_mesh &mesh, const mesh_nf_parameters &params, size_t parallel_grain_size){

}

}