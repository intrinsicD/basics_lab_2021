//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MESHIO_H
#define BCG_GRAPHICS_BCG_MESHIO_H

#include <string>

#include "bcg_mesh.h"

namespace bcg {

struct io_flags {
    io_flags() = default;

    bool use_binary = false;             //!< read / write binary format
    bool use_vertex_normals = false;     //!< read / write vertex normals
    bool use_vertex_colors = false;      //!< read / write vertex colors
    bool use_vertex_texcoords = false;   //!< read / write vertex texcoords
    bool use_face_normals = false;       //!< read / write face normals
    bool use_face_colors = false;        //!< read / write face colors
    bool use_halfedge_texcoords = false; //!< read / write halfedge texcoords
};

struct meshio {
    meshio(const std::string &filename, const io_flags &flags);

    bool read(halfedge_mesh &mesh);

    bool write(halfedge_mesh &mesh);

private:

    bool read_off(halfedge_mesh &mesh);

    bool read_obj(halfedge_mesh &mesh);

    bool read_stl(halfedge_mesh &mesh);

    bool read_ply(halfedge_mesh &mesh);

    bool read_pmp(halfedge_mesh &mesh);

    bool read_xyz(halfedge_mesh &mesh);

    bool read_agi(halfedge_mesh &mesh);

    bool write_off(const halfedge_mesh &mesh);

    bool write_off_binary(const halfedge_mesh &mesh);

    bool write_obj(const halfedge_mesh &mesh);

    bool write_stl(const halfedge_mesh &mesh);

    bool write_ply(const halfedge_mesh &mesh);

    bool write_pmp(const halfedge_mesh &mesh);

    bool write_xyz(const halfedge_mesh &mesh);

private:
    std::string filename;
    io_flags flags;
};

}

#endif //BCG_GRAPHICS_BCG_MESHIO_H
