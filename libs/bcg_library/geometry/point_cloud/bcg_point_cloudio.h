//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUDIO_H
#define BCG_GRAPHICS_BCG_POINT_CLOUDIO_H

#include "bcg_point_cloud.h"

namespace bcg {

struct point_cloudio_flags {
    point_cloudio_flags() = default;

    bool use_binary = false;             //!< read / write binary format
    bool use_vertex_normals = false;     //!< read / write vertex normals
    bool use_vertex_colors = false;      //!< read / write vertex colors
};

struct point_cloudio {
    point_cloudio(const std::string &filename, const point_cloudio_flags &flags);

    bool read(point_cloud &pc);

    //TODO implement point cloud writing!
    bool write(point_cloud &pc);

    bool write(vertex_container *vertices);

    static std::vector<std::string> formats();

private:

    bool read_pts(point_cloud &pc);

    bool read_xyz(point_cloud &pc);

    bool read_pwn(point_cloud &pc);

    bool read_pb(point_cloud &pc);

    bool read_csv(point_cloud &pc);

    bool read_3d(point_cloud &pc);

    bool read_txt(point_cloud &pc);

    bool read_ply(point_cloud &pc);

private:
    std::string filename;
    point_cloudio_flags flags;
};

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUDIO_H
