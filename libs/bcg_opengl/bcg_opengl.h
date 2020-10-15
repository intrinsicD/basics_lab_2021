//
// Created by alex on 16.10.20.
//

#ifndef BCG_GRAPHICS_BCG_OPENGL_H
#define BCG_GRAPHICS_BCG_OPENGL_H

#include <string>
#include <array>
#include <vector>

namespace bcg {

#ifndef BCG_GL_INVALID_ID
#define BCG_GL_INVALID_ID (unsigned int)(-1)
#endif

// forward declaration
struct GLFWwindow;

bool init_glad(std::string &error);

void assert_ogl_error();

void ogl_version(int &major, int &minor);

std::string ogl_version_string();

std::string glsl_version_string();

std::string ogl_renderer_string();


struct ogl_types {
    enum Type {
        TYPE_UBYTE = 0,
        TYPE_USHORT,
        TYPE_HALF,
        TYPE_FLOAT,
        TYPE_INT8,
        TYPE_UINT8,
        TYPE_INT16,
        TYPE_UINT16,
        TYPE_INT32,
        TYPE_UINT32,
    };

    // Returns the OpenGL type identifier that corresponds to unsigned byte.
    static unsigned int GetGLType(const unsigned char *);

    // Returns the OpenGL type identifier that corresponds to unsigned short.
    static unsigned int GetGLType(const unsigned short *);

    // Returns the OpenGL type identifier that corresponds to float.
    static unsigned int GetGLType(const float *);

    // Returns the OpenGL type identifier that corresponds to byte.
    static unsigned int GetGLType(const signed char *);

    // Returns the OpenGL type identifier that corresponds to short.
    static unsigned int GetGLType(const short *);

    // Returns the OpenGL type identifier that corresponds to int.
    static unsigned int GetGLType(const int *);

    // Returns the OpenGL type identifier that corresponds to unsigned int.
    static unsigned int GetGLType(const unsigned int *);

    // Returns the Type that corresponds to unsigned byte.
    static Type GetType(const unsigned char *);

    // Returns the Type that corresponds to unsigned short.
    static Type GetType(const unsigned short *);

    // Returns the Type that corresponds to float.
    static Type GetType(const float *);

    // Returns the Type that corresponds to byte.
    static Type GetType(const signed char *);

    // Returns the Type that corresponds to short.
    static Type GetType(const short *);

    // Returns the Type that corresponds to int.
    static Type GetType(const int *);

    // Returns the Type that corresponds to unsigned int.
    static Type GetType(const unsigned int *);
};

unsigned int TextureFormat(ogl_types::Type type, int numChannels);

unsigned int TextureDataFormat(ogl_types::Type type, int numChannels);

std::string GlfwKeyToString(int key);

struct ogl_handle {
    unsigned int handle = BCG_GL_INVALID_ID;
    std::string name;

    ogl_handle();

    ogl_handle(unsigned int handle);

    ogl_handle(unsigned int handle, std::string name);

    bool is_valid() const;

    operator bool() const;
};

struct glsl_shader : public ogl_handle {
    unsigned int type;
    std::string source, filename;

    glsl_shader();

    glsl_shader(unsigned int handle, unsigned int type);

    glsl_shader(unsigned int handle, unsigned int type, std::string name);

    void create();

    void destroy();

    void replace_source(int count, const char **string, const int *length);

    bool compile_file(const char *filename, unsigned int type);

    bool compile_file(const char *filename, unsigned int type, const char *prepend_source);

    bool compile_file(const char *filename, unsigned int type, int prepend_count, const char **prepend_sources);

    bool load_source(const char *filename);

    bool compile(const char *source, unsigned int type);

    bool compile(const char *source, unsigned int type, const char *prepend_source);

    bool compile(const char *source, unsigned int type, int prepend_count, const char **prepend_sources);

    int get_compile_status() const;

    int get_info_log_length() const;

    std::string get_info_log(int length) const;

    void check_compile_status() const;

    bool check_eq_shader_type(unsigned int type) const;
};

struct glsl_program : public ogl_handle {
    glsl_program();

    explicit glsl_program(unsigned int handle);

    explicit glsl_program(unsigned int handle, unsigned int type, std::string name);

    void create();

    void destroy();

    void attach(const glsl_shader &shader) const;

    void detach(const glsl_shader &shader) const;

    bool build(const std::vector<glsl_shader> &shaders);

    bool build(const glsl_shader *vertex_shader, const glsl_shader *fragment_shader, const glsl_shader *geometry_shader,
               const glsl_shader *tess_control_shader = nullptr, const glsl_shader *tess_eval_shader = nullptr);

    bool build_files(const char *vertex_shader_file, const char *fragment_shader_file, const char *geometry_shader_file,
                     const char *tess_control_shader_file, const char *tess_eval_shader_file);

    bool build_files(const char *vertex_shader_file, const char *fragment_shader_file, const char *geometry_shader_file,
                     const char *tess_control_shader_file, const char *tess_eval_shader_file,
                     const char *prepend_source);

    bool build_files(const char *vertex_shader_file, const char *fragment_shader_file, const char *geometry_shader_file,
                     const char *tess_control_shader_file, const char *tess_eval_shader_file,
                     int prepend_count, const char **prepend_sources);

    bool build_sources(const char *vertex_shader_source, const char *fragment_shader_source,
                       const char *geometry_shader_source, const char *tess_control_shader_source,
                       const char *tess_eval_shader_source);

    bool build_sources(const char *vertex_shader_source, const char *fragment_shader_source,
                       const char *geometry_shader_source, const char *tess_control_shader_source,
                       const char *tess_eval_shader_source, const char *prepend_source);

    bool build_sources(const char *vertex_shader_source, const char *fragment_shader_source,
                       const char *geometry_shader_source, const char *tess_control_shader_source,
                       const char *tess_eval_shader_source, int prepend_count,
                       const char **prepend_sources);

#if GL_VERSION_4_3

    bool build_compute_file(const char *compute_shader_file);

#endif

    bool link() const;

    bool get_link_status() const;

    int get_info_log_length() const;

    std::string get_info_log(int length) const;

    void check_link_status() const;

    void bind() const;

    void release() const;

    void print_active_attrinutes() const;

    void print_active_uniforms() const;

    unsigned int get_attribute_location(const char *name) const;

    int get_uniform_location(const char *name) const;

    int get_uniform_block_location(const char *name) const;

    int get_uniform_block_size(int index) const;

    void set_uniform_f(const char *name, float x) const;

    void set_uniform_f(const char *name, float x, float y) const;

    void set_uniform_f(const char *name, float x, float y, float z) const;

    void set_uniform_f(const char *name, float x, float y, float z, float w) const;

    void set_uniform_1f(const char *name, int count, const float *data) const;

    void set_uniform_2f(const char *name, int count, const float *data) const;

    void set_uniform_3f(const char *name, int count, const float *data) const;

    void set_uniform_4f(const char *name, int count, const float *data) const;

    void set_uniform_i(const char *name, int x) const;

    void set_uniform_i(const char *name, int x, int y) const;

    void set_uniform_i(const char *name, int x, int y, int z) const;

    void set_uniform_i(const char *name, int x, int y, int z, int w) const;

    void set_uniform_1i(const char *name, int count, const int *data) const;

    void set_uniform_2i(const char *name, int count, const int *data) const;

    void set_uniform_3i(const char *name, int count, const int *data) const;

    void set_uniform_4i(const char *name, int count, const int *data) const;

#ifdef GL_VERSION_3_0

    void set_uniform_u(const char *name, GLuint x) const;

    void set_uniform_u(const char *name, GLuint x, GLuint y) const;

    void set_uniform_u(const char *name, GLuint x, GLuint y, GLuint z) const;

    void set_uniform_u(const char *name, GLuint x, GLuint y, GLuint z, GLuint w) const;

    void set_uniform_1u(const char *name, int count, const GLuint *data) const;

    void set_uniform_2u(const char *name, int count, const GLuint *data) const;

    void set_uniform_3u(const char *name, int count, const GLuint *data) const;

    void set_uniform_4u(const char *name, int count, const GLuint *data) const;

#endif
#ifdef GL_VERSION_4_0

    void set_uniform_d(const char *name, double x) const;

    void set_uniform_d(const char *name, double x, double y) const;

    void set_uniform_d(const char *name, double x, double y, double z) const;

    void set_uniform_d(const char *name, double x, double y, double z, double w) const;

    void set_uniform_1d(const char *name, int count, const double *data) const;

    void set_uniform_2d(const char *name, int count, const double *data) const;

    void set_uniform_3d(const char *name, int count, const double *data) const;

    void set_uniform_4d(const char *name, int count, const double *data) const;

#endif

    void set_uniform_matrix_2f(const char *name, const float *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_3f(const char *name, const float *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_4f(const char *name, const float *m, int count = 1, bool transpose = false) const;

#ifdef GL_VERSION_2_1

    void set_uniform_matrix_2x3f(const char *name, const float *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_2x4f(const char *name, const float *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_3x2f(const char *name, const float *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_3x4f(const char *name, const float *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_4x2f(const char *name, const float *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_4x3f(const char *name, const float *m, int count = 1, bool transpose = false) const;

#endif
#ifdef GL_VERSION_4_0

    void set_uniform_matrix_2d(const char *name, const double *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_3d(const char *name, const double *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_4d(const char *name, const double *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_2x3d(const char *name, const double *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_2x4d(const char *name, const double *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_3x2d(const char *name, const double *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_3x4d(const char *name, const double *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_4x2d(const char *name, const double *m, int count = 1, bool transpose = false) const;

    void set_uniform_matrix_4x3d(const char *name, const double *m, int count = 1, bool transpose = false) const;

#endif
};

struct ogl_texture : public ogl_handle {
    unsigned int target;
    int width, height, channels;
    int unit, mipmap_level;
    unsigned int internal_format, format, type;
    float border_color_rgba[4];

    ogl_texture();

    ogl_texture(std::string name);

    ogl_texture(unsigned int target, std::string name);

    ogl_texture(unsigned int handle, unsigned int target);

    ogl_texture(unsigned int handle, unsigned int target, std::string name);

    void create();

    void destroy();

    void bind() const;

    void release() const;

    void activate(int unit);

    void activate() const;

    void set_wrap_repeat();

    void set_wrap_mirror_repeat();

    void set_wrap_clamp_to_edge();

    void set_wrap_clamp_to_border();

    void set_filter_linear();

    void set_filter_nearest();

    void generate_mipmaps();

    void set_mipmaps_filter_nearest_nearest();

    void set_mipmaps_filter_linear_nearest();

    void set_mipmaps_filter_nearest_linear();

    void set_mipmaps_filter_linear_linear();

    void set_image_data(unsigned int target, int mipmap_level, unsigned int internal_format, int width, int height,
                        unsigned int format, unsigned int type, void *data);

    void update_data(void *data, int width, int height);

    void download_data(void *data);

    void resize(int width, int height);
};

}

#endif //BCG_GRAPHICS_BCG_OPENGL_H
