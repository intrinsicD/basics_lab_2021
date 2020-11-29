//
// Created by alex on 28.11.20.
//

#ifndef BCG_GRAPHICS_BCG_TRIANGLE_KDTREE_H
#define BCG_GRAPHICS_BCG_TRIANGLE_KDTREE_H

#include "mesh/bcg_mesh.h"
#include "distance_query/bcg_distance_triangle_point.h"

namespace bcg {

struct triangle_kdtree {
    explicit triangle_kdtree(const halfedge_mesh &mesh, unsigned int max_faces = 10,
                             unsigned int max_depth = 30);

    //! destructur
    ~triangle_kdtree() { delete root; }

    //! nearest neighbor information
    struct NearestNeighbor {
        distance_point3_triangle3::result result;
        face_handle face;
        int tests;
    };

    //! Return handle of the nearest neighbor
    NearestNeighbor nearest(const VectorS<3> &p) const;

private:
    // triangle stores corners and face handle
    struct tri : public triangle3 {
        tri() = default;

        tri(const VectorS<3> &x0, const VectorS<3> &x1, const VectorS<3> &x2, face_handle ff)
                : triangle3(x0, x1, x2), f(ff) {}

        face_handle f;
    };

    // vector of Triangle
    typedef std::vector<tri> Triangles;

    // Node of the tree: contains parent, children and splitting plane
    struct Node {
        Node() : faces(nullptr), left_child(nullptr), right_child(nullptr) {};

        ~Node() {
            delete faces;
            delete left_child;
            delete right_child;
        }

        unsigned char axis;
        bcg_scalar_t split;
        Triangles *faces;
        Node *left_child;
        Node *right_child;
    };

    // Recursive part of build()
    unsigned int build_recurse(Node *node, unsigned int max_handles,
                               unsigned int depth);

    // Recursive part of nearest()
    void nearest_recurse(Node *node, const VectorS<3> &point,
                         NearestNeighbor &data) const;

private:
    Node *root;
};


}

#endif //BCG_GRAPHICS_BCG_TRIANGLE_KDTREE_H
