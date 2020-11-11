//
// Created by alex on 14.10.20.
//

#include <map>
#include <fstream>
#include <utility>
#include "rply/rply.h"
#include "bcg_meshio.h"
#include "utils/bcg_path.h"

namespace bcg {

meshio::meshio(std::string filename, const meshio_flags &flags) : filename(std::move(filename)), flags(flags) {

}

bool meshio::read(halfedge_mesh &mesh) {
    // clear mesh before reading from file
    mesh.vertices.clear();
    mesh.edges.clear();
    mesh.halfedges.clear();
    mesh.faces.clear();

    // extract file extension
    std::string ext = path_extension(filename);

    // extension determines reader
    if (ext == ".off") {
        return read_off(mesh);
    } else if (ext == ".obj") {
        return read_obj(mesh);
    } else if (ext == ".stl") {
        return read_stl(mesh);
    } else if (ext == ".ply") {
        return read_ply(mesh);
    } else if (ext == ".pmp") {
        return read_pmp(mesh);
    } else if (ext == ".xyz") {
        return read_xyz(mesh);
    } else if (ext == ".agi") {
        return read_agi(mesh);
    }

    // we didn't find a reader module
    return false;
}

bool meshio::write(halfedge_mesh &mesh) {
    // extract file extension
    std::string ext = path_extension(filename);

    // extension determines reader
    if (ext == ".off") {
        return write_off(mesh);
    } else if (ext == ".obj") {
        return write_obj(mesh);
    } else if (ext == ".stl") {
        return write_stl(mesh);
    } else if (ext == ".ply") {
        return write_ply(mesh);
    } else if (ext == ".pmp") {
        return write_pmp(mesh);
    } else if (ext == ".xyz") {
        return write_xyz(mesh);
    }

    // we didn't find a writer module
    return false;
}

bool read_off_ascii(halfedge_mesh &mesh, FILE *in, const bool has_normals,
                    const bool has_texcoords, const bool has_colors) {
    char line[1000], *lp;
    int nc;
    unsigned int i, j, items, idx;
    unsigned int nv, nf, ne;
    float x, y, z, r, g, b;
    vertex_handle v;

    // properties
    property<VectorS<3>, 3> normals;
    property<VectorS<2>, 2> texcoords;
    property<VectorS<3>, 3> colors;
    if (has_normals) {
        normals = mesh.vertices.get_or_add<VectorS<3>, 3>("v_normal");
    }
    if (has_texcoords) {
        texcoords = mesh.vertices.get_or_add<VectorS<2>, 2>("v_tex");
    }
    if (has_colors) {
        colors = mesh.vertices.get_or_add<VectorS<3>, 3>("v_color");
    }

    // #Vertice, #Faces, #Edges
    items = fscanf(in, "%d %d %d\n", (int *) &nv, (int *) &nf, (int *) &ne);
    if (items == 0) {
        std::cerr << "OFF: fail to read numbers of elements\n";
    }

    mesh.vertices.reserve(nv);
    mesh.halfedges.reserve(2 * std::max(3 * nv, ne));
    mesh.edges.reserve(std::max(3 * nv, ne));
    mesh.faces.reserve(nf);

    // read vertices: pos [normal] [color] [texcoord]
    for (i = 0; i < nv && !feof(in); ++i) {
        // read line
        lp = fgets(line, 1000, in);
        lp = line;

        // position
        items = sscanf(lp, "%f %f %f%n", &x, &y, &z, &nc);
        assert(items == 3);
        v = mesh.add_vertex(VectorS<3>(x, y, z));
        lp += nc;

        // normal
        if (has_normals) {
            if (sscanf(lp, "%f %f %f%n", &x, &y, &z, &nc) == 3) {
                normals[v] = VectorS<3>(x, y, z);
            }
            lp += nc;
        }

        // color
        if (has_colors) {
            if (sscanf(lp, "%f %f %f%n", &r, &g, &b, &nc) == 3) {
                if (r > 1.0 || g > 1.0 || b > 1.0) {
                    r /= 255.0;
                    g /= 255.0;
                    b /= 255.0;
                }
                colors[v] = VectorS<3>(r, g, b);
            }
            lp += nc;
        }

        // tex coord
        if (has_texcoords) {
            items = sscanf(lp, "%f %f%n", &x, &y, &nc);
            assert(items == 2);
            texcoords[v][0] = x;
            texcoords[v][1] = y;
            lp += nc;
        }
    }

    // read faces: #N v[1] v[2] ... v[n-1]
    std::vector<vertex_handle> vertices;
    face_handle f;
    for (i = 0; i < nf; ++i) {
        // read line
        lp = fgets(line, 1000, in);
        lp = line;

        // #vertices
        items = sscanf(lp, "%d%n", (int *) &nv, &nc);
        assert(items == 1);
        vertices.resize(nv);
        lp += nc;

        // indices
        for (j = 0; j < nv; ++j) {
            items = sscanf(lp, "%d%n", (int *) &idx, &nc);
            assert(items == 1);
            vertices[j] = vertex_handle(idx);
            lp += nc;
        }
        if (vertices.size() == nv) {
            f = mesh.add_face(vertices);
        } else {
            std::cerr << "OFF: fail to read face " << i;
        }
    }

    return true;
}


// helper function
template<typename T>
void tfread(FILE *in, const T &t) {
    size_t num_items = fread((char *) &t, 1, sizeof(t), in);
    if (num_items == 0) {
        std::cerr << "num_items should be > 0";
        assert(num_items > 0);
    }
}

// helper function
template<typename T>
void tfwrite(FILE *out, const T &t) {
    size_t num_items = fwrite((char *) &t, 1, sizeof(t), out);
    if (num_items == 0) {
        std::cerr << "num_items should be > 0";
        assert(num_items > 0);
    }
}

bool read_off_binary(halfedge_mesh &mesh, FILE *in, const bool has_normals,
                     const bool has_texcoords, const bool has_colors) {
    int i, j, idx(0);
    int nv(0), nf(0), ne(0);
    VectorS<3> p, n, c;
    VectorS<2> t;
    vertex_handle v;

    // binary cannot (yet) read colors
    if (has_colors) {
        return false;
    }

    // properties
    property<VectorS<3>, 3> normals;
    property<VectorS<2>, 2> texcoords;
    if (has_normals) {
        normals = mesh.vertices.get_or_add<VectorS<3>, 3>("v_normal");
    }
    if (has_texcoords) {
        texcoords = mesh.vertices.get_or_add<VectorS<2>, 2>("v_tex");
    }

    // #Vertice, #Faces, #Edges
    tfread(in, nv);
    tfread(in, nf);
    tfread(in, ne);
    mesh.vertices.reserve(nv);
    mesh.halfedges.reserve(2 * std::max(3 * nv, ne));
    mesh.edges.reserve(std::max(3 * nv, ne));
    mesh.faces.reserve(nf);

    // read vertices: pos [normal] [color] [texcoord]
    for (i = 0; i < nv && !feof(in); ++i) {
        // position
        tfread(in, p);
        v = mesh.add_vertex(p);

        // normal
        if (has_normals) {
            tfread(in, n);
            normals[v] = n;
        }

        // tex coord
        if (has_texcoords) {
            tfread(in, t);
            texcoords[v][0] = t[0];
            texcoords[v][1] = t[1];
        }
    }

    // read faces: #N v[1] v[2] ... v[n-1]
    std::vector<vertex_handle> vertices;
    face_handle f;
    for (i = 0; i < nf; ++i) {
        tfread(in, nv);
        vertices.resize(nv);
        for (j = 0; j < nv; ++j) {
            tfread(in, idx);
            vertices[j] = vertex_handle(idx);
        }
        f = mesh.add_face(vertices);
    }

    return true;
}

bool meshio::read_off(halfedge_mesh &mesh) {
    char line[200];
    bool has_texcoords = false;
    bool has_normals = false;
    bool has_colors = false;
    bool has_hcoords = false;
    bool has_dim = false;
    bool is_binary = false;

    // open file (in ASCII mode)
    FILE *in = fopen(filename.c_str(), "r");
    if (!in) {
        return false;
    }

    // read header: [ST][C][N][4][n]OFF BINARY
    char *c = fgets(line, 200, in);
    assert(c != nullptr);
    c = line;
    if (c[0] == 'S' && c[1] == 'T') {
        has_texcoords = true;
        c += 2;
    }
    if (c[0] == 'C') {
        has_colors = true;
        ++c;
    }
    if (c[0] == 'N') {
        has_normals = true;
        ++c;
    }
    if (c[0] == '4') {
        has_hcoords = true;
        ++c;
    }
    if (c[0] == 'n') {
        has_dim = true;
        ++c;
    }
    if (strncmp(c, "OFF", 3) != 0) {
        fclose(in);
        return false;
    } // no OFF
    if (strncmp(c + 4, "BINARY", 6) == 0) {
        is_binary = true;
    }

    // homogeneous coords, and vertex dimension != 3 are not supported
    if (has_hcoords || has_dim) {
        fclose(in);
        return false;
    }

    // if binary: reopen file in binary mode
    if (is_binary) {
        fclose(in);
        in = fopen(filename.c_str(), "rb");
        c = fgets(line, 200, in);
        assert(c != nullptr);
    }

    // read as ASCII or binary
    bool ok = (is_binary ? read_off_binary(mesh, in, has_normals, has_texcoords, has_colors)
                         : read_off_ascii(mesh, in, has_normals, has_texcoords, has_colors));

    fclose(in);
    return ok;
}

bool meshio::read_obj(halfedge_mesh &mesh) {
    char s[200];
    float x, y, z;
    std::vector<vertex_handle> vertices;
    std::vector<VectorS<2>> all_tex_coords; //individual texture coordinates
    std::vector<int> halfedge_tex_idx; //texture coordinates sorted for halfedges
    property<VectorS<2>, 2> tex_coords = mesh.halfedges.get_or_add<VectorS<2>, 2>("v_tex");
    bool with_tex_coord = false;

    // open file (in ASCII mode)
    FILE *in = fopen(filename.c_str(), "r");
    if (!in) {
        return false;
    }

    // clear line once
    memset(&s, 0, 200);

    // parse line by line (currently only supports vertex positions & faces
    vertex_handle v;
    while (in && !feof(in) && fgets(s, 200, in)) {
        // comment
        if (s[0] == '#' || isspace(s[0])) {
            continue;
        }

            // vertex
        else if (strncmp(s, "v ", 2) == 0) {
            if (sscanf(s, "v %f %f %f", &x, &y, &z) == 3) {
                v = mesh.add_vertex({x, y, z});
            }
        }

            // normal
        else if (strncmp(s, "vn ", 3) == 0) {
            if (sscanf(s, "v %f %f %f", &x, &y, &z) == 3) {
                // problematic as it can be either a vertex property when interpolated
                // or a halfedge property for hard edges
            }
        }

            // texture coordinate
        else if (strncmp(s, "vt ", 3) == 0) {
            if (sscanf(s, "v %f %f", &x, &y) == 2) {
                all_tex_coords.emplace_back(x, y);
            }
        }

            // face
        else if (strncmp(s, "f ", 2) == 0) {
            int component(0), nv(0);
            bool end_of_vertex(false);
            char *p0, *p1(s + 1);

            vertices.clear();
            halfedge_tex_idx.clear();

            // skip white-spaces
            while (*p1 == ' ') {
                ++p1;
            }

            while (p1) {
                p0 = p1;

                // overwrite next separator

                // skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
                while (*p1 != '/' && *p1 != '\r' && *p1 != '\n' && *p1 != ' ' && *p1 != '\0') {
                    ++p1;
                }

                // detect end of vertex
                if (*p1 != '/') {
                    end_of_vertex = true;
                }

                // replace separator by '\0'
                if (*p1 != '\0') {
                    *p1 = '\0';
                    p1++; // point to next token
                }

                // detect end of line and break
                if (*p1 == '\0' || *p1 == '\n') {
                    p1 = nullptr;
                }

                // read next vertex component
                if (*p0 != '\0') {
                    switch (component) {
                        case 0: // vertex
                        {
                            vertices.emplace_back(atoi(p0) - 1);
                            break;
                        }
                        case 1: // texture coord
                        {
                            int idx = atoi(p0) - 1;
                            halfedge_tex_idx.push_back(idx);
                            with_tex_coord = true;
                            break;
                        }
                        case 2: // normal
                            break;
                    }
                }

                ++component;

                if (end_of_vertex) {
                    component = 0;
                    nv++;
                    end_of_vertex = false;
                }
            }

            auto f = mesh.add_face(vertices);

            // add texture coordinates
            if (with_tex_coord) {
                unsigned v_idx = 0;
                for (const auto h : mesh.get_halfedges(f)) {
                    tex_coords[h] = all_tex_coords[halfedge_tex_idx[v_idx]];
                    ++v_idx;
                }
            }
        }
        // clear line
        memset(&s, 0, 200);
    }

    // if there are no textures, delete texture property!
    if (!with_tex_coord) {
        mesh.halfedges.remove(tex_coords);
    }

    fclose(in);
    return true;
}


class CmpVec {
public:
    explicit CmpVec(bcg_scalar_t eps = scalar_eps) : eps_(eps) {}

    bool operator()(const VectorS<3> &v0, const VectorS<3> &v1) const {
        if (std::abs(v0[0] - v1[0]) <= eps_) {
            if (std::abs(v0[1] - v1[1]) <= eps_) {
                return (v0[2] < v1[2] - eps_);
            } else {
                return (v0[1] < v1[1] - eps_);
            }
        } else {
            return (v0[0] < v1[0] - eps_);
        }
    }

private:
    bcg_scalar_t eps_;
};

bool meshio::read_stl(halfedge_mesh &mesh) {
    char line[100], *c;
    unsigned int i, nT(0);
    Vector<float, 3> p;
    vertex_handle v;
    std::vector<vertex_handle> vertices(3);


    CmpVec comp(scalar_min);
    std::map<VectorS<3>, vertex_handle, CmpVec> vMap(comp);
    std::map<VectorS<3>, vertex_handle, CmpVec>::iterator vMapIt;

    // open file (in ASCII mode)
    FILE *in = fopen(filename.c_str(), "r");
    if (!in) return false;

    // ASCII or binary STL?
    c = fgets(line, 6, in);
    assert(c != nullptr);
    const bool binary = ((strncmp(line, "SOLID", 5) != 0) && (strncmp(line, "solid", 5) != 0));

    // parse binary STL
    if (binary) {
        size_t n_items(0);
        // re-open file in binary mode
        fclose(in);
        in = fopen(filename.c_str(), "rb");
        if (!in) {
            return false;
        }

        // skip dummy header
        n_items = fread(line, 1, 80, in);
        assert(n_items > 0);

        // read number of triangles
        tfread(in, nT);

        // read triangles
        while (nT) {
            // skip triangle normal
            n_items = fread(line, 1, 12, in);
            assert(n_items > 0);
            // triangle's vertices
            for (i = 0; i < 3; ++i) {
                tfread(in, p);

                // has vector been referenced before?
                if ((vMapIt = vMap.find(p.cast<bcg_scalar_t>())) == vMap.end()) {
                    // No : add vertex and remember idx/vector mapping
                    v = mesh.add_vertex(p.cast<bcg_scalar_t>());
                    vertices[i] = v;
                    vMap[p.cast<bcg_scalar_t>()] = v;
                } else {
                    // Yes : get index from map
                    vertices[i] = vMapIt->second;
                }
            }

            // Add face only if it is not degenerated
            face_handle f;
            if ((vertices[0] != vertices[1]) &&
                (vertices[0] != vertices[2]) &&
                (vertices[1] != vertices[2])) {
                f = mesh.add_face(vertices);
            }

            n_items = fread(line, 1, 2, in);
            assert(n_items > 0);
            --nT;
        }
    }

        // parse ASCII STL
    else {
        // parse line by line
        while (in && !feof(in) && fgets(line, 100, in)) {
            // skip white-space
            for (c = line; isspace(*c) && *c != '\0'; ++c) {}

            // face begins
            if ((strncmp(c, "outer", 5) == 0) || (strncmp(c, "OUTER", 5) == 0)) {
                // read three vertices
                for (i = 0; i < 3; ++i) {
                    // read line
                    c = fgets(line, 100, in);
                    assert(c != nullptr);

                    // skip white-space
                    for (c = line; isspace(*c) && *c != '\0'; ++c) {}

                    // read x, y, z
                    sscanf(c + 6, "%f %f %f", &p[0], &p[1], &p[2]);

                    // has vector been referenced before?
                    if ((vMapIt = vMap.find(p.cast<bcg_scalar_t>())) == vMap.end()) {
                        // No : add vertex and remember idx/vector mapping
                        v = mesh.add_vertex(p.cast<bcg_scalar_t>());
                        vertices[i] = v;
                        vMap[p.cast<bcg_scalar_t>()] = v;
                    } else {
                        // Yes : get index from map
                        vertices[i] = vMapIt->second;
                    }
                }

                // Add face only if it is not degenerated
                face_handle f;
                if ((vertices[0] != vertices[1]) &&
                    (vertices[0] != vertices[2]) &&
                    (vertices[1] != vertices[2]))
                    f = mesh.add_face(vertices);
            }
        }
    }

    fclose(in);
    return true;
}


// helper to assemble vertex data
static int vertexCallback(p_ply_argument argument) {
    long idx;
    void *pdata;
    ply_get_argument_user_data(argument, &pdata, &idx);

    auto *mesh = (halfedge_mesh *) pdata;
    auto point = mesh->object_properties.get<VectorS<3>, 3>("v_position");

    point[0][idx] = (bcg_scalar_t) ply_get_argument_value(argument);

    vertex_handle v;
    if (idx == 2) {
        v = mesh->add_vertex(point[0]);
    }

    return 1;
}

//-----------------------------------------------------------------------------

// helper to assemble face data
static int faceCallback(p_ply_argument argument) {
    long length, value_index;
    void *pdata;
    long idata;
    ply_get_argument_user_data(argument, &pdata, &idata);
    ply_get_argument_property(argument, nullptr, &length, &value_index);

    auto *mesh = (halfedge_mesh *) pdata;
    auto vertices = mesh->object_properties.get<std::vector<vertex_handle>, 1>("vertices");

    if (value_index == 0) {
        vertices[0].clear();
    }

    int idx = (int)ply_get_argument_value(argument);

    vertices[0].push_back(idx);

    face_handle f;
    if (value_index == length - 1) {
        f = mesh->add_face(vertices[0]);
    }

    return 1;
}

bool meshio::read_ply(halfedge_mesh &mesh) {
    // add object properties to hold temporary data
    auto point = mesh.object_properties.add<VectorS<3>, 3>("v_position");
    point.resize(1);
    auto vertices = mesh.object_properties.add<std::vector<vertex_handle>, 1>("vertices");

    // open file, read header
    p_ply ply = ply_open(filename.c_str(), nullptr, 0, nullptr);

    if (!ply) {
        return false;
    }

    if (!ply_read_header(ply)) {
        return false;
    }

    // setup callbacks for basic properties
    ply_set_read_cb(ply, "vertex", "x", vertexCallback, &mesh, 0);
    ply_set_read_cb(ply, "vertex", "y", vertexCallback, &mesh, 1);
    ply_set_read_cb(ply, "vertex", "z", vertexCallback, &mesh, 2);

    ply_set_read_cb(ply, "face", "vertex_indices", faceCallback, &mesh, 0);

    // read the data
    if (!ply_read(ply)) {
        return false;
    }

    ply_close(ply);

    // clean-up properties
    mesh.object_properties.remove(point);
    mesh.object_properties.remove(vertices);

    return mesh.faces.size() > 0;
}

bool meshio::read_pmp(halfedge_mesh &mesh) {
    // open file (in binary mode)
    FILE *in = fopen(filename.c_str(), "rb");
    if (!in) {
        return false;
    }

    // how many elements?
    unsigned int nv(0), ne(0), nh(0), nf(0);
    tfread(in, nv);
    tfread(in, ne);
    tfread(in, nf);
    nh = 2 * ne;

    // texture coordinates?
    bool has_htex(flags.use_halfedge_texcoords);
    tfread(in, has_htex);

    // resize containers
    mesh.vertices.resize(nv);
    mesh.halfedges.resize(nh);
    mesh.edges.resize(ne);
    mesh.faces.resize(nf);

    // get properties
    auto vconn = mesh.vertices.get_or_add<halfedge_mesh::vertex_connectivity, 1>("v_connectivity");
    auto hconn = mesh.halfedges.get_or_add<halfedge_mesh::halfedge_connectivity, 1>("h_connectivity");
    auto fconn = mesh.faces.get_or_add<halfedge_mesh::face_connectivity, 1>("f_connectivity");
    auto point = mesh.vertices.get_or_add<VectorS<3>, 3>("v_position");

    // read properties from file
    size_t nvc = fread((char *) vconn.data(), sizeof(halfedge_mesh::vertex_connectivity), nv, in);
    size_t nhc = fread((char *) hconn.data(), sizeof(halfedge_mesh::halfedge_connectivity), nh, in);
    size_t nfc = fread((char *) fconn.data(), sizeof(halfedge_mesh::face_connectivity), nf, in);
    size_t np = fread((char *) point.data(), sizeof(VectorS<3>), nv, in);
    if (nvc != nv) {
        std::cerr << "nvc != nv";
        fclose(in);
        return false;
    }
    if (nhc != nh) {
        std::cerr << "nhc != nh";
        fclose(in);
        return false;
    }
    if (nfc != nf) {
        std::cerr << "nfc != nf";
        fclose(in);
        return false;
    }
    if (np != nv) {
        std::cerr << "np != nv";
        fclose(in);
        return false;
    }

    // read texture coordiantes
    if (has_htex) {
        auto htex = mesh.halfedges.get_or_add<VectorS<2>, 2>("v_tex");
        size_t nhtc = fread((char *) htex.data(), sizeof(VectorS<2>), nh, in);
        if (nhtc != nh) {
            std::cerr << "nhtc != nh";
            fclose(in);
            return false;
        }
    }

    fclose(in);
    return true;
}

bool meshio::read_xyz(halfedge_mesh &mesh) {
    // open file (in ASCII mode)
    FILE *in = fopen(filename.c_str(), "r");
    if (!in) {
        return false;
    }

    // add normal property
    // \todo this adds property even if no normals present. change it.
    auto vnormal = mesh.vertices.get_or_add<VectorS<3>, 3>("v_normal");

    char line[200];
    bcg_scalar_t x, y, z;
    bcg_scalar_t nx, ny, nz;
    int n;
    vertex_handle v;

    // read data
    while (in && !feof(in) && fgets(line, 200, in)) {
        n = sscanf(line, "%lf %lf %lf %lf %lf %lf", reinterpret_cast<double *>(&x), reinterpret_cast<double *>(&y),
                   reinterpret_cast<double *>(&z), reinterpret_cast<double *>(&nx), reinterpret_cast<double *>(&ny),
                   reinterpret_cast<double *>(&nz));
        if (n >= 3) {
            v = mesh.add_vertex({x, y, z});
            if (n >= 6) {
                vnormal[v] = {nx, ny, nz};
            }
        }
    }

    fclose(in);
    return true;
}

bool meshio::read_agi(halfedge_mesh &mesh) {
    // open file (in ASCII mode)
    FILE *in = fopen(filename.c_str(), "r");
    if (!in) {
        return false;
    }

    // add normal property
    auto normal = mesh.vertices.get_or_add<VectorS<3>, 3>("v_normal");
    auto color = mesh.vertices.get_or_add<VectorS<3>, 3>("v_color");

    char line[200];
    bcg_scalar_t x, y, z;
    bcg_scalar_t nx, ny, nz;
    bcg_scalar_t r, g, b;
    int n;
    vertex_handle v;

    // read data
    while (in && !feof(in) && fgets(line, 200, in)) {
        n = sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf", reinterpret_cast<double *>(&x),
                   reinterpret_cast<double *>(&y), reinterpret_cast<double *>(&z), reinterpret_cast<double *>(&r),
                   reinterpret_cast<double *>(&g), reinterpret_cast<double *>(&b),
                   reinterpret_cast<double *>(&nx), reinterpret_cast<double *>(&ny), reinterpret_cast<double *>(&nz));
        if (n >= 3) {
            v = mesh.add_vertex({x, y, z});
            if (n >= 6) {
                color[v] = {r / 255.0, g / 255.0, b / 255.0};
                if (n >= 9) {
                    normal[v] = {nx, ny, nz};
                }
            }
        }
    }

    fclose(in);
    return true;
}

bool meshio::write_off(const halfedge_mesh &mesh) {
    if (flags.use_binary) {
        return write_off_binary(mesh);
    }

    FILE *out = fopen(filename.c_str(), "w");
    if (!out) {
        return false;
    }

    bool has_normals = false;
    bool has_texcoords = false;
    bool has_colors = false;

    auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    auto texcoords = mesh.vertices.get<VectorS<3>, 2>("v_tex");
    auto colors = mesh.vertices.get<VectorS<3>, 3>("v_color");

    if (normals && flags.use_vertex_normals) {
        has_normals = true;
    }
    if (texcoords && flags.use_vertex_texcoords) {
        has_texcoords = true;
    }
    if (colors && flags.use_vertex_colors) {
        has_colors = true;
    }

    // header
    if (has_texcoords) {
        fprintf(out, "ST");
    }
    if (has_colors) {
        fprintf(out, "C");
    }
    if (has_normals) {
        fprintf(out, "N");
    }
    fprintf(out, "OFF\n%zu %zu 0\n", mesh.num_vertices(), mesh.num_faces());

    // vertices, and optionally normals and texture coordinates
    auto position = mesh.vertices.get<VectorS<3>, 3>("v_position");
    for (const auto v : mesh.vertices) {
        fprintf(out, "%.10f %.10f %.10f", position[v][0], position[v][1], position[v][2]);

        if (has_normals) {
            fprintf(out, " %.10f %.10f %.10f", normals[v][0], normals[v][1], normals[v][2]);
        }

        if (has_colors) {
            fprintf(out, " %.10f %.10f %.10f", colors[v][0], colors[v][1], colors[v][2]);
        }

        if (has_texcoords) {
            fprintf(out, " %.10f %.10f", texcoords[v][0], texcoords[v][1]);
        }

        fprintf(out, "\n");
    }

    // faces
    for (const auto f : mesh.faces) {
        int nv = (int) mesh.get_valence(f);
        fprintf(out, "%d", nv);
        for (const auto fv : mesh.get_vertices(f)) {
            fprintf(out, " %zu", fv.idx);
        }
        fprintf(out, "\n");
    }

    fclose(out);
    return true;
}

bool meshio::write_off_binary(const halfedge_mesh &mesh) {
    FILE *out = fopen(filename.c_str(), "w");
    if (!out) {
        return false;
    }

    fprintf(out, "OFF BINARY\n");
    fclose(out);

    bcg_index_t nv = (int) mesh.num_vertices();
    bcg_index_t nf = (int) mesh.num_faces();
    bcg_index_t ne = 0;

    out = fopen(filename.c_str(), "ab");
    tfwrite(out, nv);
    tfwrite(out, nf);
    tfwrite(out, ne);
    auto positions = mesh.vertices.get<VectorS<3>, 3>("v_position");
    for (const auto v : mesh.vertices) {
        tfwrite(out, positions[v]);
    }

    for (const auto f : mesh.faces) {
        tfwrite(out, (bcg_index_t) mesh.get_valence(f));
        for (const auto fv : mesh.get_vertices(f)) {
            tfwrite(out, (bcg_index_t) fv.idx);
        }
    }
    fclose(out);
    return true;
}

bool meshio::write_obj(const halfedge_mesh &mesh) {
    FILE *out = fopen(filename.c_str(), "w");
    if (!out) {
        return false;
    }

    // comment
    fprintf(out, "# OBJ export from SurfaceMesh\n");

    //vertices
    auto positions = mesh.vertices.get<VectorS<3>, 3>("v_position");
    for (const auto v : mesh.vertices) {
        fprintf(out, "v %.10f %.10f %.10f\n", positions[v][0], positions[v][1], positions[v][2]);
    }

    //normals
    auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    if (normals) {
        for (const auto v : mesh.vertices) {
            fprintf(out, "vn %.10f %.10f %.10f\n", normals[v][0], normals[v][1], normals[v][2]);
        }
    }

    // optional texture coordinates
    // do we have them?
    std::vector<std::string> hprops = mesh.halfedges.properties_names();
    bool with_tex_coord = false;
    auto hpropEnd = hprops.end();
    auto hpropStart = hprops.begin();
    while (hpropStart != hpropEnd) {
        if (0 == (*hpropStart).compare("v_tex")) {
            with_tex_coord = true;
        }
        ++hpropStart;
    }

    //if so then add
    if (with_tex_coord) {
        auto texCoord = mesh.halfedges.get<VectorS<2>, 2>("v_tex");
        for (const auto h: mesh.halfedges) {
            fprintf(out, "vt %.10f %.10f \n", texCoord[h][0], texCoord[h][1]);
        }
    }

    //faces
    for (const auto f : mesh.faces) {
        fprintf(out, "f");
        auto fvit = mesh.get_vertices(f);
        auto fvend = fvit;
        auto fhit = mesh.get_halfedges(f);
        do {
            if (with_tex_coord) {
                // write vertex index, texCoord index and normal index
                fprintf(out, " %lu/%lu/%lu", (*fvit).idx + 1, (*fhit).idx + 1,
                        (*fvit).idx + 1);
                ++fhit;
            } else {
                // write vertex index and normal index
                fprintf(out, " %lu//%lu", (*fvit).idx + 1, (*fvit).idx + 1);
            }
        } while (++fvit != fvend);
        fprintf(out, "\n");
    }

    fclose(out);
    return true;
}

bool meshio::write_stl(const halfedge_mesh &mesh) {
    if (!mesh.is_triangle_mesh()) {
        std::cerr << "write_stl: not a triangle mesh!\n";
        return false;
    }

    auto fnormals = mesh.faces.get<VectorS<3>, 3>("f_normal");
    if (!fnormals) {
        std::cerr << "write_stl: no face normals present!\n";
        return false;
    }

    std::ofstream ofs(filename.c_str());
    auto positions = mesh.vertices.get<VectorS<3>, 3>("v_position");

    ofs << "solid stl" << std::endl;
    ofs.precision(10);
    ofs.setf(std::ios::fixed);
    VectorS<3> n, p;
    for (const auto f : mesh.faces) {
        n = fnormals[f];
        ofs << "  facet normal ";
        ofs << n[0] << " " << n[1] << " " << n[2] << std::endl;
        ofs << "    outer loop" << std::endl;
        for (const auto v : mesh.get_vertices(f)) {
            p = positions[v];
            ofs << "      vertex ";
            ofs << p[0] << " " << p[1] << " " << p[2] << std::endl;
        }
        ofs << "    endloop" << std::endl;
        ofs << "  endfacet" << std::endl;
    }
    ofs << "endsolid" << std::endl;
    ofs.close();
    return true;
}

bool meshio::write_ply(const halfedge_mesh &mesh) {
    e_ply_storage_mode mode = flags.use_binary ? PLY_LITTLE_ENDIAN : PLY_ASCII;
    p_ply ply = ply_create(filename.c_str(), mode, nullptr, 0, nullptr);

    ply_add_comment(ply, "File written with bcg-library");
    ply_add_element(ply, "vertex", mesh.num_vertices());
    ply_add_scalar_property(ply, "x", PLY_FLOAT);
    ply_add_scalar_property(ply, "y", PLY_FLOAT);
    ply_add_scalar_property(ply, "z", PLY_FLOAT);
    ply_add_element(ply, "face", mesh.num_faces());
    ply_add_property(ply, "vertex_indices", PLY_LIST, PLY_UCHAR, PLY_INT);
    ply_write_header(ply);

    // write vertices
    auto positions = mesh.vertices.get<VectorS<3>, 3>("v_position");
    for (const auto v : mesh.vertices) {
        ply_write(ply, positions[v][0]);
        ply_write(ply, positions[v][1]);
        ply_write(ply, positions[v][2]);
    }

    // write faces
    for (const auto f : mesh.faces) {
        ply_write(ply, mesh.get_valence(f));
        for (const auto fv : mesh.get_vertices(f)) {
            ply_write(ply, fv.idx);
        }
    }

    ply_close(ply);
    return true;
}

bool meshio::write_pmp(const halfedge_mesh &mesh) {
// open file (in binary mode)
    FILE *out = fopen(filename.c_str(), "wb");
    if (!out) {
        return false;
    }

    // get properties
    auto vconn = mesh.vertices.get<halfedge_mesh::vertex_connectivity, 1>("v_connectivity");
    auto hconn = mesh.halfedges.get<halfedge_mesh::halfedge_connectivity, 1>("h_connectivity");
    auto fconn = mesh.faces.get<halfedge_mesh::face_connectivity, 1>("f_connectivity");
    auto point = mesh.vertices.get<VectorS<3>, 3>("v_position");
    auto htex = mesh.halfedges.get<VectorS<2>, 2>("v_tex");

    // how many elements?
    unsigned int nv = (unsigned int) mesh.num_vertices();
    unsigned int ne = (unsigned int) mesh.num_edges();
    unsigned int nh = (unsigned int) mesh.num_halfedges();
    unsigned int nf = (unsigned int) mesh.num_faces();

    // write header
    tfwrite(out, nv);
    tfwrite(out, ne);
    tfwrite(out, nf);
    tfwrite(out, (bool) htex);

    // write properties to file
    fwrite((char *) vconn.data(), sizeof(halfedge_mesh::vertex_connectivity), nv, out);
    fwrite((char *) hconn.data(), sizeof(halfedge_mesh::halfedge_connectivity), nh, out);
    fwrite((char *) fconn.data(), sizeof(halfedge_mesh::face_connectivity), nf, out);
    fwrite((char *) point.data(), sizeof(VectorS<3>), nv, out);

    // texture coordinates
    if (htex) {
        fwrite((char *) htex.data(), sizeof(VectorS<2>), nh, out);
    }

    fclose(out);
    return true;
}

bool meshio::write_xyz(const halfedge_mesh &mesh) {
    std::ofstream ofs(filename);
    if (!ofs) {
        return false;
    }

    auto positions = mesh.vertices.get<VectorS<3>, 3>("v_position");
    auto vnormal = mesh.vertices.get<VectorS<3>, 3>("v_normal");

    for (const auto v : mesh.vertices) {
        ofs << positions[v].transpose();
        ofs << " ";
        if (vnormal) {
            ofs << vnormal[v].transpose();
        }
        ofs << std::endl;
    }

    ofs.close();
    return true;
}
}