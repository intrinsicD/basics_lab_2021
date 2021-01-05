//
// Created by alex on 28.12.20.
//

#include "bcg_mesh_dec.h"
#include "bcg_mesh_vertex_area_barycentric.h"
#include "bcg_mesh_vertex_area_voronoi.h"
#include "bcg_mesh_edge_cotan.h"
#include "bcg_mesh_face_areas.h"

namespace bcg {

dec_operators mesh_dec(halfedge_mesh &mesh, size_t parallel_grain_size){
    dec_operators operators;
    size_t nVerts = mesh.num_vertices();
    size_t nEdges = mesh.num_edges();
    size_t nFaces = mesh.num_faces();


    if(mesh.is_delaunay_mesh()){
        //vertexDual circumcentric
        vertex_voronoi_areas(mesh, parallel_grain_size);
        operators.vertexDualAreas = mesh.vertices.get<bcg_scalar_t, 1>("v_voronoi_area");
    }else{
        //vertexDual barycentric
        vertex_barycentric_areas(mesh, parallel_grain_size);
        operators.vertexDualAreas = mesh.vertices.get<bcg_scalar_t, 1>("v_barycentric_area");
    }

    edge_cotans(mesh, parallel_grain_size);
    operators.edgeCotanWeights = mesh.edges.get<bcg_scalar_t, 1>("e_cotan");

    face_areas(mesh, parallel_grain_size);
    operators.faceAreas = mesh.faces.get<bcg_scalar_t, 1>("f_area");

    { // Hodge 0
        VectorS<-1> hodge0V(nVerts);
        for (const auto v : mesh.vertices) {
            double primalArea = 1.0;
            double dualArea = vertexDualAreas[v];
            double ratio = dualArea / primalArea;
            operators.hodge0V[v] = ratio;
        }

        operators.hodge0 = hodge0V.asDiagonal();
    }

    { // Hodge 1
        VectorS<-1> hodge1V(nEdges);
        for (const auto e : mesh.edges) {
            double ratio = edgeCotanWeights[e];
            hodge1V[e] = ratio;
        }

        operators.hodge1 = hodge1V.asDiagonal();
    }

    { // Hodge 2
        VectorS<-1> hodge2V(nFaces);
        for (const auto f : mesh.faces) {
            double primalArea = faceAreas[f];
            double dualArea = 1.0;
            double ratio = dualArea / primalArea;

            hodge2V[f] = ratio;
        }
        operators.hodge2 = hodge2V.asDiagonal();
    }

    { // D0
        operators.d0 = SparseMatrix<bcg_scalar_t>(nEdges, nVerts);
        std::vector<Eigen::Triplet<bcg_scalar_t>> tripletList;

        for (const auto e : mesh.edges) {
            halfedge_handle he = mesh.halfedge_graph::get_halfedge(e, 0);
            vertex_handle vTail = mesh.get_vertex(e, 0);
            vertex_handle vHead = mesh.get_vertex(e, 1);

            tripletList.emplace_back(e, vHead, 1.0);
            tripletList.emplace_back(e, vTail, -1.0);
        }

        operators.d0.setFromTriplets(tripletList.begin(), tripletList.end());
    }

    { // D1
        operators.d1 = SparseMatrix<bcg_scalar_t>(nFaces, nEdges);
        std::vector<Eigen::Triplet<bcg_scalar_t>> tripletList;

        for (const auto f : mesh.faces) {
            for (const auto he : mesh.get_halfedges(f)) {
                edge_handle e = mesh.get_edge(he);
                double sign = (he == mesh.halfedge_graph::get_halfedge(e, 0)) ? (1.0) : (-1.0);
                tripletList.emplace_back(f, e, sign);
            }
        }

        operators.d1.setFromTriplets(tripletList.begin(), tripletList.end());
    }
}

}