//
// Created by alex on 19.04.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FACE_QUADRIC_H
#define BCG_GRAPHICS_BCG_MESH_FACE_QUADRIC_H

#include "bcg_mesh.h"
#include "bcg_mesh_vertex_quadric.h"
#include "math/bcg_probabilistic_quadric.h"

namespace bcg {

enum class MeshFaceQuadricType {
    point,
    plane,
    triangle,
    global_isotropic_probabilistic_plane,
    global_anisotropic_probabilistic_plane,
    global_isotropic_probabilistic_triangle,
    global_anisotropic_probabilistic_triangle,
    local_isotropic_probabilistic_plane,
    local_anisotropic_probabilistic_plane,
    local_isotropic_probabilistic_triangle,
    local_anisotropic_probabilistic_triangle,
    __last__
};

std::vector<std::string> mesh_face_quadric_type_names();


void mesh_face_point_quadric(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

void mesh_face_plane_quadric(halfedge_mesh &mesh, property<VectorS<3>, 3> normals, size_t parallel_grain_size = 1024);

void
mesh_face_triangle_quadric(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

//global isotropic sigmas
void mesh_face_probabilistic_plane_quadric_isotropic(halfedge_mesh &mesh, property<VectorS<3>, 3> normals,
                                                     bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                                     size_t parallel_grain_size = 1024);

//global anisotropic sigmas
void mesh_face_probabilistic_plane_quadric_anisotropic(halfedge_mesh &mesh, property<VectorS<3>, 3> normals,
                                                       const MatrixS<3, 3> &sigma_p,
                                                       const MatrixS<3, 3> &sigma_n,
                                                       size_t parallel_grain_size = 1024);

//global isotropic sigmas
void mesh_face_probabilistic_triangle_quadric_isotropic(halfedge_mesh &mesh,
                                                        bcg_scalar_t sigma_p, size_t parallel_grain_size = 1024);

//global anisotropic sigmas
void mesh_face_probabilistic_triangle_quadric_anisotropic(halfedge_mesh &mesh,
                                                          const MatrixS<3, 3> &sigma_p, const MatrixS<3, 3> &sigma_q,
                                                          const MatrixS<3, 3> &sigma_r,
                                                          size_t parallel_grain_size = 1024);

//local isotropic sigmas
void mesh_face_probabilistic_plane_quadric_isotropic(halfedge_mesh &mesh, property<VectorS<3>, 3> normals,
                                                     property<bcg_scalar_t, 1> sigma_p,
                                                     property<bcg_scalar_t, 1> sigma_n,
                                                     size_t parallel_grain_size = 1024);

//local isotropic sigmas
void mesh_face_probabilistic_triangle_quadric_isotropic(halfedge_mesh &mesh,
                                                        property<bcg_scalar_t, 1> sigma_p,
                                                        size_t parallel_grain_size = 1024);

//local anisotropic sigmas
void mesh_face_probabilistic_plane_quadric_anisotropic(halfedge_mesh &mesh, property<VectorS<3>, 3> normals,
                                                       property<MatrixS<3, 3>, 1> sigma_p,
                                                       property<MatrixS<3, 3>, 1> sigma_n,
                                                       size_t parallel_grain_size = 1024);

//local anisotropic sigmas
void mesh_face_probabilistic_triangle_quadric_anisotropic(halfedge_mesh &mesh,
                                                          property<MatrixS<3, 3>, 1> sigma_p,
                                                          size_t parallel_grain_size = 1024);

//----------------------------------------------------------------------------------------------------------------------

using quadric = pq::quadric<pq::math<bcg_scalar_t, VectorS<3>, VectorS<3>, MatrixS<3, 3>>>;

void mesh_face_quadric_error(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                             size_t parallel_grain_size = 1024);

void mesh_face_quadric_minimizer(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                                 size_t parallel_grain_size = 1024);

void mesh_face_quadric_neighbors_sum(halfedge_mesh &mesh, property<quadric, 1> quadrics, Params params = Params(),
                                     size_t parallel_grain_size = 1024);

void mesh_face_quadric_neighbors_avg(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                                     Params params = Params(),
                                     size_t parallel_grain_size = 1024);

void mesh_face_quadric_neighbors_sum_to_vertices(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                                                 Params params = Params(),
                                                 size_t parallel_grain_size = 1024);

void mesh_face_quadric_neighbors_avg_to_vertices(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                                                 Params params = Params(),
                                                 size_t parallel_grain_size = 1024);

void mesh_face_quadric_extract_normals(halfedge_mesh &mesh, property<quadric, 1> quadrics,
                                       size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_FACE_QUADRIC_H
