//
// Created by alex on 14.10.20.
//

#include "bcg_point_cloudio.h"
#include "../utils/bcg_path.h"
#include "../utils/bcg_file.h"
#include "../utils/bcg_string_utils.h"
#include "../bcg_colors.h"
#include "../../exts/rply/rply.h"

namespace bcg {

point_cloudio::point_cloudio(const std::string &filename, const point_cloudio_flags &flags) : filename(filename),
                                                                                              flags(flags) {

}

bool point_cloudio::read(point_cloud &pc) {
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

bool point_cloudio::write(point_cloud &pc) {
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

bool point_cloudio::read_pts(point_cloud &pc) {
    auto processText = [](const char *, size_t) {

    };

    auto colors = pc.vertices.get_or_add<vec3f, 3>("color", color::white);
    auto intensities = pc.vertices.get_or_add<float, 1>("intensity", 1.0);
    vertex_handle v;
    auto insertLambda = [&v, &pc, &colors, &intensities](float result, size_t lineCount, size_t floatPerLineCount) {
        if (lineCount == 0) {
            pc.vertices.reserve(result);
        } else {
            if (floatPerLineCount == 0) {
                v = pc.add_vertex(zero3f);
            }
            if (floatPerLineCount < 3) {
                pc.positions[v][floatPerLineCount] = result;
            } else if (floatPerLineCount < 4) {
                intensities[v] = float(result);
            } else if (floatPerLineCount < 7) {
                colors[v][floatPerLineCount - 4] = float(result) / 255.0f;
            }
        }
    };

    std::string txt;
    file_stream(filename).load_text(txt);
    if (txt.empty()) return false;
    parse_numbers<float>(txt, processText, insertLambda);
    return true;
}


bool point_cloudio::read_xyz(point_cloud &pc) {
    int skipCount = -1;
    auto processText = [&skipCount](const char *start, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            if (*(start + i) == 'L') {
                skipCount = 1;
                return;
            }
        }
    };
    vertex_handle v;
    auto insertLambda = [&v, &pc, &skipCount](float result, size_t, size_t floatPerLineCount) {
        if (skipCount > 0) {
            skipCount -= 1;
            return;
        }
        if (floatPerLineCount == 0) {
            v = pc.add_vertex(zero3f);
        }
        if (floatPerLineCount < 3) {
            pc.positions.back()[floatPerLineCount] = result;
        }
    };

    std::string txt;
    file_stream(filename).load_text(txt);
    if (txt.empty()) return false;
    parse_numbers<float>(txt, processText, insertLambda);
    return true;
}


bool point_cloudio::read_pwn(point_cloud &pc) {
    FILE *in = fopen(filename.c_str(), "r");

    if (in == NULL) {
        fclose(in);
        return false;
    }

    int N_;
    int res = fscanf(in, "%d", &N_);
    size_t size = static_cast<unsigned int>(N_);
    if (res == 0) {
        fclose(in);
        return false;
    }

    pc.vertices.reserve(size);

    vec3f p;
    vertex_handle v;
    for (unsigned int i = 0; i < size; ++i) {
        res = fscanf(in, "%f %f %f", &p[0], &p[1], &p[2]);
        v = pc.add_vertex(p);
    }

    auto normals = pc.vertices.get_or_add<vec3f, 3>("normal");
    for (unsigned int i = 0; i < size; ++i) {
        res = fscanf(in, "%f %f %f", &normals[i][0], &normals[i][1], &normals[i][2]);
        normals[i] = normalize(normals[i]);
    }

    fclose(in);
    return true;
}


bool point_cloudio::read_pb(point_cloud &pc) {
    std::ifstream i;
    i.open(filename.c_str(), std::ios::in | std::ios::binary);

    if (i.fail() || !i.is_open()) return false;

    unsigned int size;
    i.read((char *) &size, sizeof(size));

    pc.vertices.reserve(size);

    float s;
    // points
    vec3f p;
    vertex_handle v;
    for (size_t k = 0; k < size; ++k) {
        for (unsigned int j = 0; j < 3; ++j) {
            i.read((char *) &s, sizeof(float));
            p[j] = s;
        }
        v = pc.add_vertex(p);
    }

    if (i.fail()) {
        auto normals = pc.vertices.get_or_add<vec3f, 3>("normal");
        for (size_t k = 0; k < size; ++k) {
            for (unsigned int j = 0; j < 3; ++j) {
                i.read((char *) &s, sizeof(float));
                normals[k][j] = s;
            }
            normals[k] = normalize(normals[k]);
        }
    }

    if (i.fail()) {
        auto colors = pc.vertices.get_or_add<vec3f, 3>("color");
        for (size_t k = 0; k < size; ++k) {
            for (unsigned int j = 0; j < 3; ++j) {
                i.read((char *) &s, sizeof(float));
                colors[k][j] = s;
            }
        }
    }
    return true;
}


bool point_cloudio::read_csv(point_cloud &pc) {
    auto processText = [](const char *, size_t) {

    };
    vertex_handle v;
    auto insertLambda = [&v, &pc](float result, size_t, size_t floatPerLineCount) {
        if (floatPerLineCount == 0) {
            v = pc.add_vertex(zero3f);
        }
        if (floatPerLineCount < 3) {
            pc.positions[v][floatPerLineCount] = result;
        }
    };

    std::string txt;
    file_stream(filename).load_text(txt);
    if (txt.empty()) return false;
    parse_numbers<float>(txt, processText, insertLambda);
    return true;
}


bool point_cloudio::read_3d(point_cloud &pc) {
    auto processText = [](const char *, size_t) {

    };

    std::vector<float> position, intensity;
    auto intensities = pc.vertices.get_or_add<float, 1>("intensity");
    vertex_handle v;
    auto insertLambda = [&v, &pc, &intensities](float result, size_t, size_t floatPerLineCount) {
        if (floatPerLineCount == 0) {
            v = pc.add_vertex(zero3f);
        }
        if (floatPerLineCount < 3) {
            pc.positions[v][floatPerLineCount] = result;
        } else if (floatPerLineCount < 4) {
            intensities[v] = result;
        }
    };

    std::string txt;
    file_stream(filename).load_text(txt);
    if (txt.empty()) return false;
    parse_numbers<float>(txt, processText, insertLambda);
    return true;
}


bool point_cloudio::read_txt(point_cloud &pc) {
    auto processText = [](const char *, size_t) {

    };

    auto colors = pc.vertices.get_or_add<vec3f, 3>("color");
    auto reflectances = pc.vertices.get_or_add<float, 1>("reflectance");
    vertex_handle v;
    auto insertLambda = [&v, &pc, &colors, &reflectances](float result, size_t lineCount,
                                                      size_t floatPerLineCount) {
        if (lineCount == 0) {
            pc.vertices.reserve(result);
        } else {
            if (floatPerLineCount == 0) {
                v = pc.add_vertex(zero3f);
            }
            if (floatPerLineCount < 3) {
                pc.positions[v][floatPerLineCount] = result;
            } else if (floatPerLineCount < 6) {
                colors[v][floatPerLineCount - 3] = result;
            } else if (floatPerLineCount < 7) {
                reflectances[v] = result;
            }
        }
    };

    std::string txt;
    file_stream(filename).load_text(txt);
    if (txt.empty()) return false;
    parse_numbers<float>(txt, processText, insertLambda);
    return true;
}


// helper to assemble vertex data
static int PCvertexCallback(p_ply_argument argument) {
    long idx;
    void *pdata;
    ply_get_argument_user_data(argument, &pdata, &idx);

    auto *mesh = (point_cloud *) pdata;
    auto point = mesh->object_properties.get<vec3f, 3>("point");

    point[0][idx] = (float) ply_get_argument_value(argument);

    vertex_handle v;
    if (idx == 2) {
        v = mesh->add_vertex(point[0]);
    }

    return 1;
}

bool point_cloudio::read_ply(point_cloud &pc) {
    // add object properties to hold temporary data
    auto point = pc.object_properties.add<vec3f, 3>("point");
    point.resize(1);

    // open file, read header
    p_ply ply = ply_open(filename.c_str(), nullptr, 0, nullptr);

    if (!ply)
        return false;

    if (!ply_read_header(ply))
        return false;

    // setup callbacks for basic properties
    ply_set_read_cb(ply, "vertex", "x", PCvertexCallback, &pc, 0);
    ply_set_read_cb(ply, "vertex", "y", PCvertexCallback, &pc, 1);
    ply_set_read_cb(ply, "vertex", "z", PCvertexCallback, &pc, 2);

    // read the data
    if (!ply_read(ply))
        return false;

    ply_close(ply);

    // clean-up properties
    pc.object_properties.remove(point);

    return pc.vertices.size() > 0;
}

}