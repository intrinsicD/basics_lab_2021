//
// Created by alex on 11.05.21.
//

#include "bcg_graph_spring_embedding.h"
#include "bcg_property_map_eigen.h"
//Todo make attration force more consistent over neighbors and repulsion force using kd_tree locally

namespace bcg {

void graph_random_embedding_2D(halfedge_graph &graph) {
    graph_random_embedding_3D(graph);
    Map(graph.positions).col(2).setZero();
}

void graph_random_embedding_3D(halfedge_graph &graph) {
    auto positions = graph.positions;
    Map(positions) = MatrixS<-1, 3>::Random();
    positions.set_dirty();
}

VectorS<3>
eades_forces(halfedge_graph &graph, vertex_handle v, bcg_scalar_t c_rep, bcg_scalar_t c_spring, bcg_scalar_t l) {
    VectorS<3> rep_force = VectorS<3>::Zero();
    VectorS<3> spring_force = VectorS<3>::Zero();
    bcg_scalar_t rep_weight_sum = 0;
    bcg_scalar_t spring_weight_sum = 0;
    for (const auto &vv : graph.get_vertices(v)) {
        VectorS<3> diff = graph.positions[vv] - graph.positions[v];
        bcg_scalar_t sq_norm = diff.squaredNorm();
        if(sq_norm > scalar_eps && l > scalar_eps){
            bcg_scalar_t rep_weight =  c_rep / sq_norm;
            rep_force += rep_weight * diff;
            rep_weight_sum += rep_weight;
            bcg_scalar_t spring_weight = c_spring * std::log(std::sqrt(sq_norm) / l);
            spring_force -= spring_weight * diff;
            spring_weight_sum += spring_weight;
        }
    }
    return rep_force / rep_weight_sum + spring_force / spring_weight_sum;
}

void graph_spring_embedding::iterate(halfedge_graph &graph){
    auto positions = graph.positions;
    auto spring_forces = graph.vertices.get_or_add<VectorS<3>, 3>("v_spring_forces");
    int iterations = 0;
    bcg_scalar_t max_force_magnitude = scalar_max;
    while (iterations < num_steps && max_force_magnitude > force_eps) {
        bcg_scalar_t max_force_magnitude_next = -scalar_max;
        for (const auto v : graph.vertices) {
            spring_forces[v] = eades_forces(graph, v, repulsion_coefficient, spring_coefficient, target_edge_length);
            bcg_scalar_t spring_force_norm = spring_forces[v].norm();
            if (spring_force_norm > max_force_magnitude_next){
                max_force_magnitude_next = spring_force_norm;
            }
        }

        if (annealing && num_steps > 1) {
            force_coefficient *= std::exp(-(bcg_scalar_t(iterations) / bcg_scalar_t(num_steps)));
        }
        Map(positions).array() += MapConst(spring_forces).array() * force_coefficient;
        ++iterations;
        max_force_magnitude = max_force_magnitude_next;
    }
    positions.set_dirty();
    spring_forces.set_dirty();
}

}