//
// Created by alex on 14.10.20.
//

#include "bcg_point_cloudio.h"
#include "../utils/bcg_path.h"

namespace bcg{

point_cloudio::point_cloudio(const std::string &filename, const point_cloudio_flags &flags) : filename(filename), flags(flags){

}

bool point_cloudio::read(point_cloud &pc){
    // clear point_cloud before reading from file
    pc.vertices.clear();

    // extract file extension
    std::string ext = path_extension(filename);
    
    // extension determines reader
    if (ext == ".pts") {
        return read_pts(pc);
    } else if (ext == ".xyz") {
        return read_xyz(pc);
    } else if (ext == ".pwn") {
        return read_pwn(pc);
    } else if (ext == ".pb") {
        return read_pb(pc);
    } else if (ext == ".csv") {
        return read_csv(pc);
    } else if (ext == ".3d") {
        return read_3d(pc);
    } else if (ext == ".txt") {
        return read_txt(pc);
    } else if (ext == ".ply") {
        return read_ply(pc);
    }

    // we didn't find a reader module
    return false;
}

bool point_cloudio::write(point_cloud &pc){
// extract file extension
    std::string ext = path_extension(filename);

    //TODO implement write functions for pointclouds
    // extension determines reader
    /*if (ext == ".pts") {
        return write_pts(pc);
    } else if (ext == ".xyz") {
        return write_xyz(pc);
    } else if (ext == ".pwn") {
        return write_pwn(pc);
    } else if (ext == ".pb") {
        return write_pb(pc);
    } else if (ext == ".csv") {
        return write_csv(pc);
    } else if (ext == ".3d") {
        return write_3d(pc);
    } else if (ext == ".txt") {
        return write_txt(pc);
    }*/

    // we didn't find a writer module
    return false;
}

}