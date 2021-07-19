//
// Created by alex on 25.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MARCHING_CUBES_H
#define BCG_GRAPHICS_BCG_MARCHING_CUBES_H

#include <functional>
#include <unordered_map>
#include "grid/bcg_occupancy_grid.h"
#include "mesh/bcg_mesh.h"

namespace bcg {

struct marching_cubes : private occupancy_grid3 {
    marching_cubes();

    std::function<double(const Vector<double, 3> &)> implicit_function;

    double avg_sdf;
    double min_sdf;
    double max_sdf;

    static double hearts_function(const Vector<double, 3> &p);

    std::function<double(const Vector<double, 3> &)> convert_point_cloud_ellipse(property<VectorS<3>, 3> points);

    std::function<double(const Vector<double, 3> &)> convert_point_cloud_cassini(property<VectorS<3>, 3> points);

    halfedge_mesh
    reconstruct(bcg_scalar_t isovalue, const VectorS<3> &min, const VectorS<3> &max, const VectorI<3> &dims);

    void compute_vertex_normals(halfedge_mesh &mesh);
};

}

#endif //BCG_GRAPHICS_BCG_GUI_MARCHING_CUBES_H
