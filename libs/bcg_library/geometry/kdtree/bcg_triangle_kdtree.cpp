//
// Created by alex on 29.11.20.
//

#include "bcg_triangle_kdtree.h"
#include "aligned_box/bcg_aligned_box.h"

namespace bcg {

triangle_kdtree::triangle_kdtree(const halfedge_mesh &mesh, unsigned int max_faces, unsigned int max_depth) {
    // init
    root = new Node();
    root->faces = new Triangles();
    auto points = mesh.positions;

    // collect triangles
    tri t;
    root->faces->reserve(mesh.num_faces());
    for (const auto f : mesh.faces) {
        auto vfit = mesh.get_vertices(f);

        t.points[0] = points[*vfit];
        ++vfit;
        t.points[1] = points[*vfit];
        ++vfit;
        t.points[2] = points[*vfit];
        t.f = f;
        root->faces->push_back(t);
    }

    // call recursive helper
    build_recurse(root, max_faces, max_depth);
}

//-----------------------------------------------------------------------------

unsigned int triangle_kdtree::build_recurse(Node *node, unsigned int max_faces, unsigned int depth) {
    // should we stop at this level ?
    if ((depth == 0) || (node->faces->size() <= max_faces)) {
        return depth;
    }

    unsigned int i;

    // compute bounding box
    aligned_box3 bbox;
    for (const auto &t : *node->faces) {
        for (i = 0; i < 3; ++i) {
            bbox.grow(t.points[i]);
        }
    }

    // split longest side of bounding box
    VectorS<3> bb = bbox.diagonal();
    bcg_scalar_t length = bb[0];
    int axis = 0;
    if (bb[1] > length) {
        axis = 1;
        length = bb[axis];
    }
    if (bb[2] > length) {
        axis = 2;
    }

#if 1
    // split in the middle
    bcg_scalar_t split = bbox.center()[axis];
#else
    // find split position as median
    std::vector<bcg_scalar_t> v;
    v.reserve(node->faces->size() * 3);
    for (fit = node->faces->begin(); fit != fend; ++fit){
        for (i = 0; i < 3; ++i){
            v.push_back(fit->p[i][axis]]);
        }
    }
    std::sort(v.begin(), v.end());
    split = v[v.size() / 2];
#endif

    // create children
    auto *left = new Node();
    left->faces = new Triangles();
    left->faces->reserve(node->faces->size() / 2);
    auto *right = new Node();
    right->faces = new Triangles;
    right->faces->reserve(node->faces->size() / 2);

    // partition for left and right child
    for (const auto &t : *node->faces) {
        bool l = false, r = false;

        if (t.points[0][axis] <= split) {
            l = true;
        } else {
            r = true;
        }
        if (t.points[1][axis] <= split) {
            l = true;
        } else {
            r = true;
        }
        if (t.points[2][axis] <= split) {
            l = true;
        } else {
            r = true;
        }

        if (l) {
            left->faces->push_back(t);
        }

        if (r) {
            right->faces->push_back(t);
        }
    }

    // stop here?
    if (left->faces->size() == node->faces->size() || right->faces->size() == node->faces->size()) {
        // compact my memory
        node->faces->shrink_to_fit();

        // delete new nodes
        delete left;
        delete right;

        // return tree depth
        return depth;
    }

        // or recurse further?
    else {
        // free my memory
        delete node->faces;
        node->faces = nullptr;

        // store internal data
        node->axis = axis;
        node->split = split;
        node->left_child = left;
        node->right_child = right;

        // recurse to childen
        int depthLeft = build_recurse(node->left_child, max_faces, depth - 1);
        int depthRight = build_recurse(node->right_child, max_faces, depth - 1);

        return std::min(depthLeft, depthRight);
    }
}

//-----------------------------------------------------------------------------

triangle_kdtree::NearestNeighbor triangle_kdtree::nearest(const VectorS<3> &p) const {
    NearestNeighbor data;
    data.result.distance = std::numeric_limits<bcg_scalar_t>::max();
    data.tests = 0;
    nearest_recurse(root, p, data);
    return data;
}

//-----------------------------------------------------------------------------

void triangle_kdtree::nearest_recurse(Node *node, const VectorS<3> &point,
                                     NearestNeighbor &data) const {
    // terminal node?
    if (!node->left_child) {
        triangle_distance distance;

        for (const auto &t : *node->faces) {
            auto result = distance(t, point);
            ++data.tests;
            if (result.distance< data.result.distance) {
                data.result = result;
                data.face = t.f;
            }
        }
    }

        // non-terminal node
    else {
        bcg_scalar_t dist = point[node->axis] - node->split;

        if (dist <= 0.0) {
            nearest_recurse(node->left_child, point, data);
            if (fabs(dist) < data.result.distance) {
                nearest_recurse(node->right_child, point, data);
            }
        } else {
            nearest_recurse(node->right_child, point, data);
            if (fabs(dist) < data.result.distance) {
                nearest_recurse(node->left_child, point, data);
            }
        }
    }
}
}