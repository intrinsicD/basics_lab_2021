//
// Created by alex on 16.10.20.
//

#ifndef BCG_GRAPHICS_BCG_OPENGL_H
#define BCG_GRAPHICS_BCG_OPENGL_H

#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "bcg_library/math/bcg_linalg.h"

// forward declaration
struct GLFWwindow;

namespace bcg {

#ifndef BCG_GL_INVALID_ID
#define BCG_GL_INVALID_ID (unsigned int)(-1)
#endif

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

    explicit ogl_handle(unsigned int handle);

    explicit ogl_handle(std::string name);

    ogl_handle(unsigned int handle, std::string name);

    bool is_valid() const;

    operator bool() const;

    operator unsigned int() const;
};

struct glsl_shader : public ogl_handle {
    unsigned int type;
    std::string source, filename;

    glsl_shader();

    glsl_shader(unsigned int type, std::string name);

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

    explicit glsl_program(std::string name);

    explicit glsl_program(unsigned int handle);

    explicit glsl_program(unsigned int handle, std::string name);

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
                       const char *geometry_shader_source=nullptr, const char *tess_control_shader_source=nullptr,
                       const char *tess_eval_shader_source=nullptr);

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

    void set_uniform_u(const char *name, unsigned int x) const;

    void set_uniform_u(const char *name, unsigned int x, unsigned int y) const;

    void set_uniform_u(const char *name, unsigned int x, unsigned int y, unsigned int z) const;

    void set_uniform_u(const char *name, unsigned int x, unsigned int y, unsigned int z, unsigned int w) const;

    void set_uniform_1u(const char *name, int count, const unsigned int *data) const;

    void set_uniform_2u(const char *name, int count, const unsigned int *data) const;

    void set_uniform_3u(const char *name, int count, const unsigned int *data) const;

    void set_uniform_4u(const char *name, int count, const unsigned int *data) const;

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

    explicit ogl_texture(std::string name);

    ogl_texture(unsigned int target, std::string name);

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

struct ogl_buffer_object : public ogl_handle {
    unsigned int target;
    int capacity;
    int num_elements;
    size_t size_bytes;
    int dims;
    bool dynamic;

    ogl_buffer_object();

    explicit ogl_buffer_object(std::string name);

    ogl_buffer_object(unsigned int target, std::string name);

    ogl_buffer_object(unsigned int handle, unsigned int target, std::string name);

    size_t get_buffer_size_gpu() const;

    void create();

    void destroy();

    void bind() const;

    void release() const;

    void upload(const void *data, size_t size, size_t dims, size_t offset = 0, bool dynamic = false);

    void download(bcg_scalar_t *data, size_t size_bytes, size_t offset_bytes = 0);
};

struct ogl_vertex_buffer : public ogl_buffer_object {
    unsigned int normalized, type;

    ogl_vertex_buffer();

    explicit ogl_vertex_buffer(std::string name);

    ogl_vertex_buffer(unsigned int handle, std::string name);

    void upload(const std::vector<bcg_scalar_t> &data, size_t offset = 0, bool dynamic = false);

    void upload(const std::vector<VectorS<2>> &data, size_t offset = 0, bool dynamic = false);

    void upload(const std::vector<VectorS<3>> &data, size_t offset = 0, bool dynamic = false);

    void upload(const std::vector<VectorS<4>> &data, size_t offset = 0, bool dynamic = false);

    void upload(const void *data, size_t size, size_t dims, size_t offset = 0, bool dynamic = false);

    void download(std::vector<bcg_scalar_t> &data, size_t offset = 0);

    void download(std::vector<VectorS<2>> &data, size_t offset = 0);

    void download(std::vector<VectorS<3>> &data, size_t offset = 0);

    void download(std::vector<VectorS<4>> &data, size_t offset = 0);

    void download(bcg_scalar_t *data, size_t size_bytes, size_t offset_bytes = 0);
};

struct ogl_element_buffer : public ogl_buffer_object {
    ogl_element_buffer();

    explicit ogl_element_buffer(std::string name);

    ogl_element_buffer(unsigned int handle, std::string name);

    void upload(const std::vector<bcg_index_t> &data, size_t offset = 0, bool dynamic = false);

    void upload(const std::vector<VectorI<2>> &data, size_t offset = 0, bool dynamic = false);

    void upload(const std::vector<VectorI<3>> &data, size_t offset = 0, bool dynamic = false);

    void upload(const void *data, size_t size, int dims, size_t offset = 0, bool dynamic = false);

    void download(std::vector<bcg_index_t> &data, size_t offset = 0);

    void download(std::vector<VectorI<2>> &data, size_t offset = 0);

    void download(std::vector<VectorI<3>> &data, size_t offset = 0);

    void download(bcg_index_t *data, size_t size_bytes, size_t offset_bytes = 0);
};


struct ogl_vertex_array : public ogl_handle {
    mutable std::unordered_map<unsigned int, std::string> captured_attributes;

    ogl_vertex_array();

    explicit ogl_vertex_array(std::string name);

    ogl_vertex_array(unsigned int handle, std::string name);

    void create();

    void destroy();

    void bind() const;

    void release() const;

    void enable_attribute(unsigned int index, const ogl_vertex_buffer &buffer) const;

    void disable_attribute(unsigned int index) const;

    void capture_vertex_buffer(unsigned int index, const ogl_vertex_buffer &buffer);
};

struct ogl_shape{
    std::unordered_map<std::string, ogl_vertex_buffer> vertex_buffers;
    ogl_element_buffer edge_buffer, triangle_buffer, adjacency_buffer;
    int num_vertices;
    int num_edges;
    int num_faces;
};

struct ogl_renderbuffer : public ogl_handle {
    ogl_renderbuffer();

    explicit ogl_renderbuffer(std::string name);

    ogl_renderbuffer(unsigned int handle, std::string name);

    void create();

    void destroy();

    void bind() const;

    void release() const;

    void storage(unsigned int internal_format, int width, int height) const;
};

struct ogl_framebuffer : public ogl_handle {
    int width, height;
    static inline unsigned int bound_framebuffer_id = 0;

    std::vector<ogl_texture> textures;
    bool has_depth_buffer;

    ogl_framebuffer();

    explicit ogl_framebuffer(std::string name);

    ogl_framebuffer(unsigned int handle, std::string name);

    void create();

    void destroy();

    void bind();

    void release() const;

    void attach_texture(const ogl_texture &texture, unsigned int attachment);

    void attach_renderbuffer(const ogl_renderbuffer &renderbuffer, unsigned int attachment);

    bool check() const;

    void oepngl_draw_buffers();

    void copy_to_default_framebuffer();
};

struct ogl_state {
    ogl_state();

    ogl_state(const ogl_state &) = default;

    void startup();

    bool gl_blend;
    bool gl_clip_distance;
    bool gl_color_logic_op;
    bool gl_cull_face;
#ifdef GL_VERSION_4_3
    bool gl_debug_output;
    bool gl_debug_output_synchonous;
#endif
    bool gl_depth_clamp;
    bool gl_depth_test; //default enabled
    bool gl_depth_mask; //default enabled
    bool gl_dither; //default enabled
    bool gl_framebuffer_srgb;
    bool gl_line_smooth;
    bool gl_multisample; //default enabled
    bool gl_polygon_offset_fill;
    bool gl_polygon_offset_line;
    bool gl_polygon_offset_point;
    bool gl_polygon_smooth;
#ifdef GL_VERSION_3_1
    bool gl_primitive_restart;
#endif
#ifdef GL_VERSION_4_3
    bool gl_primitive_restart_fixed_index;
#endif
    bool gl_rasterizer_discard;
    bool gl_sample_alpha_to_converge;
    bool gl_sample_alpha_to_one;
    bool gl_sample_converge;
    bool gl_sample_shading;
    bool gl_sample_mask;
    bool gl_scissor_test;
    bool gl_stencil_test;
#ifdef GL_VERSION_3_2
    bool gl_texture_cube_map_seamless;
#endif
    bool gl_program_point_size;

    //------------------------------------------------------------------------------------------------------------------

    unsigned int blend_func_sfactor, blend_func_dfactor;
    unsigned int logic_op_opcode;
    unsigned int cull_face_mode, front_face_mode;
    double depth_range_nearVal, depth_range_farVal;
    unsigned int depth_func;
    float polygon_offset_factor, polygon_offset_units;
#ifdef GL_VERSION_3_1
    unsigned int primitive_restart_index;
#endif
    float sample_coverage_value;
    unsigned char sample_coverage_invert;
    float multisample_value;
    unsigned int sample_mask_number;
    unsigned int sample_maks;
    int scissor_x, scissor_y;
    int scissor_width = 1, scissor_height;
    unsigned int stencil_func;
    int stencil_ref;
    unsigned int stencil_mask;
    unsigned int stencil_sfail;
    unsigned int stencil_dpfail;
    unsigned int stencil_dppass;
    float point_size_value;
    unsigned int polygon_mode_face;
    unsigned int polygon_mode;

    //------------------------------------------------------------------------------------------------------------------

    void set_blend(bool enabled);

    void set_blend_func_factors(unsigned int sfactor, unsigned int dfactor);

    void set_color_logic_op(bool enabled);

    void set_logic_op_opcode(unsigned int opcode);

    void set_cull_face(bool enabled);

    void set_cullface_mode(unsigned int mode);

    void set_frontface_mode(unsigned int mode);

    void set_depth_clamp(bool enabled);

    void set_depth_test(bool enabled);

    void set_depth_mask(bool enabled);

    void set_depth_range_values(double near, double far);

    void set_depth_func(unsigned int func);

    void set_dither(bool enabled);

    void set_framebuffer_srgb(bool enabled);

    void set_line_smooth(bool enabled);

    void set_multisample(bool enabled);

    void set_multisample_value(float value);

    void set_polygon_offset_fill(bool enabled);

    void set_polygon_offset_point(bool enabled);

    void set_polygon_offset_line(bool enabled);

    void set_polygon_smooth(bool enabled);

    void set_polygon_offset_factor(float factor, float units);

    void set_polygon_mode(unsigned int mode);

    void set_rasterizer_discard(bool enabled);

    void set_alpha_to_coverage(bool enabled);

    void set_alpha_to_one(bool enabled);

    void set_sample_converge(bool enabled);

    void set_sample_converge_value(float value, unsigned char invert);

    void set_sample_shading(bool enabled);

    void set_sample_mask(bool enabled);

    void set_sample_mask_number(unsigned int number, unsigned int masks);

    void set_scissor_test(bool enabled);

    void set_scissor_values(int x, int y, int width, int height);

    void set_stencil_test(bool enabled);

    void set_stencil_func(unsigned int func, int ref, unsigned int mask);

    void set_stencil_op(unsigned int sfail, unsigned int dpfail, unsigned int dppass);

    void set_program_point_size(bool enabled);

    void set_point_size(float size);

#ifdef GL_VERSION_3_1

    void set_primitive_restart(bool enabled);

    void set_primitive_restart_index(unsigned int index);

#endif

#ifdef GL_VERSION_3_2

    void set_texture_cube_map_seamless(bool enabled);

#endif

#ifdef GL_VERSION_4_3

    void set_debug_output(bool enabled);

    void set_debug_output_synchonous(bool enabled);

    void set_primitive_restart_fixed_index(bool enabled);

#endif

    void print(std::ostream &stream) const;

    friend std::ostream &operator<<(std::ostream &stream, const ogl_state &state);

};

GLenum string_to_ogl_enum(const std::string &name);

std::string ogl_enum_to_string(size_t value);


}

#endif //BCG_GRAPHICS_BCG_OPENGL_H