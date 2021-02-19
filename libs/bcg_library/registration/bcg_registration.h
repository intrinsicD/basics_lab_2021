//
// Created by alex on 07.12.20.
//

#ifndef BCG_GRAPHICS_BCG_REGISTRATION_H
#define BCG_GRAPHICS_BCG_REGISTRATION_H

#include <vector>
#include <string>
#include "entt/entt.hpp"

namespace bcg{
enum class RegistrationMethod{
    rigid_icp_point2point = 0,
    rigid_icp_point2plane,
    coherent_point_drift_rigid,
    coherent_point_drift_affine,
    coherent_point_drift_nonrigid,
    coherent_point_drift_nonrigid2,
    coherent_point_drift_bayes,
    __last__
};

inline std::vector<std::string> registration_names() {
    std::vector<std::string> names(static_cast<int>(RegistrationMethod::__last__));
    names[static_cast<int>(RegistrationMethod::rigid_icp_point2point)] = "rigid_icp_point2point";
    names[static_cast<int>(RegistrationMethod::rigid_icp_point2plane)] = "rigid_icp_point2plane";
    names[static_cast<int>(RegistrationMethod::coherent_point_drift_rigid)] = "coherent_point_drift_rigid";
    names[static_cast<int>(RegistrationMethod::coherent_point_drift_affine)] = "coherent_point_drift_affine";
    names[static_cast<int>(RegistrationMethod::coherent_point_drift_nonrigid)] = "coherent_point_drift_nonrigid";
    names[static_cast<int>(RegistrationMethod::coherent_point_drift_nonrigid2)] = "coherent_point_drift_nonrigid2";
    names[static_cast<int>(RegistrationMethod::coherent_point_drift_bayes)] = "coherent_point_drift_bayes";
    return names;
}
struct registration{
    RegistrationMethod method;
    std::vector<float> errors;

    int max_iterations;
    int current_iteration;
};

}

#endif //BCG_GRAPHICS_BCG_REGISTRATION_H
