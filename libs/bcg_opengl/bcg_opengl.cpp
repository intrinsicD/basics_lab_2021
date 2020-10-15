//
// Created by alex on 16.10.20.
//

#include <cassert>
#include <stdexcept>
#include <string>
#include <fstream>
#include <iostream>
#include "exts/glad/glad.h"
#include "GLFW/glfw3.h"
#include "bcg_opengl.h"

#ifdef _WIN32
#undef near
#undef far
#endif

namespace bcg {

bool init_glad(std::string &error) {
    if (!gladLoadGL()) {
        error = "Cannot initialize OpenGL context.";
        return false;
    }
    return true;
}


GLenum _assert_ogl_error() {
    auto error_code = glGetError();
    if (error_code != GL_NO_ERROR) {
        std::string error = "";
        switch (error_code) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM\n"
                        "An unacceptable value is specified for an enumerated argument. \n"
                        "The offending command is ignored and has no other side effect than to set the error flag.";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE\n"
                        "A numeric argument is out of range. \n"
                        "The offending command is ignored and has no other side effect than to set the error flag.";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION\n"
                        "The specified operation is not allowed in the current state. \n"
                        "The offending command is ignored and has no other side effect than to set the error flag.";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW\n"
                        "An attempt has been made to perform an operation that would cause an internal stack to overflow.";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW\n"
                        "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY\n"
                        "There is not enough memory left to execute the command. \n"
                        "The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n"
                        "Results of a GL operation are undefined only if GL_OUT_OF_MEMORY has occurred.\n"
                        "In all other cases, the command generating the error is ignored and has no effect on the GL state or frame buffer contents";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION\n"
                        "The framebuffer object is not complete. \n"
                        "The offending command is ignored and has no other side effect than to set the error flag.";
                break;
        }
        printf("\n    OPENGL ERROR: %s\n\n", error.c_str());
    }
    return error_code;
}

void assert_ogl_error() { assert(_assert_ogl_error() == GL_NO_ERROR); }

void ogl_version(int &major, int &minor) {
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
}

std::string ogl_version_string() {
    const unsigned char *version = glGetString(GL_VERSION);
    if (version) {
        return std::string(reinterpret_cast<const char *>(version));
    } else {
        int versionMajor = 0, versionMinor = 0;
#ifdef GL_VERSION_3_0
        ogl_version(versionMajor, versionMinor);
#endif
        if (versionMajor > 0 && versionMajor < 100) {
            return std::to_string(versionMajor) + "." + std::to_string(versionMinor);
        } else {
            return "Unknown";
        }
    }
}

std::string glsl_version_string() {
    return reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
}

std::string ogl_renderer_string() {
    const GLubyte *renderer = glGetString(GL_RENDERER);
    return std::string(reinterpret_cast<const char *>(renderer));
}


GLenum ogl_types::GetGLType(const GLubyte *) { return GL_UNSIGNED_BYTE; }

// Returns the OpenGL type identifier that corresponds to unsigned short.
GLenum ogl_types::GetGLType(const GLushort *) { return GL_UNSIGNED_SHORT; }

// Returns the OpenGL type identifier that corresponds to float.
GLenum ogl_types::GetGLType(const GLfloat *) { return GL_FLOAT; }

// Returns the OpenGL type identifier that corresponds to byte.
GLenum ogl_types::GetGLType(const GLbyte *) { return GL_BYTE; }

// Returns the OpenGL type identifier that corresponds to short.
GLenum ogl_types::GetGLType(const GLshort *) { return GL_SHORT; }

// Returns the OpenGL type identifier that corresponds to int.
GLenum ogl_types::GetGLType(const GLint *) { return GL_INT; }

// Returns the OpenGL type identifier that corresponds to unsigned int.
GLenum ogl_types::GetGLType(const GLuint *) { return GL_UNSIGNED_INT; }

// Returns the Type that corresponds to unsigned byte.
ogl_types::Type ogl_types::GetType(const GLubyte *) { return TYPE_UBYTE; }

// Returns the Type that corresponds to unsigned short.
ogl_types::Type ogl_types::GetType(const GLushort *) { return TYPE_USHORT; }

// Returns the Type that corresponds to float.
ogl_types::Type ogl_types::GetType(const GLfloat *) { return TYPE_FLOAT; }

// Returns the Type that corresponds to byte.
ogl_types::Type ogl_types::GetType(const GLbyte *) { return TYPE_INT8; }

// Returns the Type that corresponds to short.
ogl_types::Type ogl_types::GetType(const GLshort *) { return TYPE_INT16; }

// Returns the Type that corresponds to int.
ogl_types::Type ogl_types::GetType(const GLint *) { return TYPE_INT32; }

// Returns the Type that corresponds to unsigned int.
ogl_types::Type ogl_types::GetType(const GLuint *) { return TYPE_UINT32; }

unsigned int TextureFormat(ogl_types::Type type, int numChannels) {
    assert(numChannels > 0 && numChannels <= 4);
    const GLenum internalFormats[][4] = {
            {GL_R8, GL_RG8, GL_RGB8, GL_RGBA8},
            {GL_R16, GL_RG16, GL_RGB16, GL_RGBA16},
#ifdef GL_VERSION_3_0
            {GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F},
            {GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F},
            {GL_R8I, GL_RG8I, GL_RGB8I, GL_RGBA8I},
            {GL_R8UI, GL_RG8UI, GL_RGB8UI, GL_RGBA8UI},
            {GL_R16I, GL_RG16I, GL_RGB16I, GL_RGBA16I},
            {GL_R16UI, GL_RG16UI, GL_RGB16UI, GL_RGBA16UI},
            {GL_R32I, GL_RG32I, GL_RGB32I, GL_RGBA32I},
            {GL_R32UI, GL_RG32UI, GL_RGB32UI, GL_RGBA32UI},
#else
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
#endif
    };
    return internalFormats[type][numChannels - 1];
}

unsigned int TextureDataFormat(ogl_types::Type type, int numChannels) {
    const GLenum formats[][4] = {
            {GL_RED, GL_RG, GL_RGB, GL_RGBA},
#ifdef GL_VERSION_3_0
            {GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_RGBA_INTEGER},
#else
            { GL_RED,         GL_RG,         GL_RGB,         GL_RGBA         },
#endif
    };
    return formats[type >= ogl_types::TYPE_INT8][numChannels - 1];
}

ogl_handle::ogl_handle : handle(BCG_GL_INVALID_ID), name("") {

}

ogl_handle::ogl_handle(unsigned int handle) : handle(handle), name("") {

}

ogl_handle::ogl_handle(unsigned int handle, std::string name) : handle(handle), name(name) {

}

bool ogl_handle::is_valid() const {
    return handle != BCG_GL_INVALID_ID;
}

ogl_handle::operator bool() const {
    return is_valid();
}


//----------------------------------------------------------------------------------------------------------------------
// glsl shader
//----------------------------------------------------------------------------------------------------------------------


glsl_shader::glsl_shader() : ogl_handle, type(BCG_GL_INVALID_ID), source(), filename() {

}

glsl_shader::glsl_shader(unsigned int handle, unsigned int type) : ogl_handle(handle), type(type), source(),
                                                                   filename() {}

glsl_shader::glsl_shader(unsigned int handle, unsigned int type, std::string name) : ogl_handle(handle, name),
                                                                                     type(type),
                                                                                     source(), filename() {

}

void glsl_shader::create() {
    destroy();
    handle = glCreateShader(type);
    assert_ogl_error();
}

void glsl_shader::destroy() {
    if (is_valid()) {
        glDeleteShader(handle);
        assert_ogl_error();
        *this = glsl_shader();
    }
}

void glsl_shader::replace_source(int count, const char **string, const int *length) {
    glShaderSource(handle, count, string, length);
    assert_ogl_error();
}

bool glsl_shader::compile_file(const char *filename, unsigned int type) {
    return compile_file(filename, type, 0, nullptr);
}

bool glsl_shader::compile_file(const char *filename, unsigned int type, const char *prepend_source) {
    return compile_file(filename, type, 1, &prepend_source);
}

bool
glsl_shader::compile_file(const char *filename, unsigned int type, int prepend_count, const char **prepend_sources) {
    this->filename = std::string(filename);
    if (load_source(filename))
        return compile(source.data(), type, prepend_count, prepend_sources);
    return false;
}

bool glsl_shader::load_source(const char *filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);

    std::streampos begin(in.tellg());
    in.seekg(0, std::ios::end);
    std::streampos end(in.tellg());
    in.close();

    in = std::ifstream(filename, std::ios::in | std::ios::binary);

    long s = end - begin;
    if (s == 0 || !in.good()) {
        std::cerr << "Cannot compile shader file: " + std::string(filename);
        return false;
    }

    source = std::string(s, '\0');

    in.read(&source[0], s);
    in.close();
    return true;
}

bool glsl_shader::compile(const char *source, unsigned int type) {
    return compile(source, type, 0, nullptr);
}

bool glsl_shader::compile(const char *source, unsigned int type, const char *prepend_source) {
    return compile(source, type, 1, &prepend_source);
}

bool glsl_shader::compile(const char *source, unsigned int type, int prepend_count, const char **prepend_sources) {
    if (!is_valid()) {
        type = type;
        create();
    }
    if (prepend_count > 0) {
        std::vector<const char *> sources(prepend_count + 1);
        for (int i = 0; i < prepend_count; i++) sources[i] = prepend_sources[i];
        sources[prepend_count] = source;
        replace_source(prepend_count + 1, sources.data(), nullptr);
    } else {
        replace_source(1, &source, nullptr);
    }
    glCompileShader(handle);
    assert_ogl_error();
    check_compile_status();
    return check_eq_shader_type(type);
}

int glsl_shader::get_compile_status() const {
    GLint comile_error(GL_FALSE);
    glGetShaderiv(handle, GL_COMPILE_STATUS, &comile_error);
    check_compile_status();
    return comile_error;
}

int glsl_shader::get_info_log_length() const {
    GLint info_log_length(0);
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &info_log_length);
    check_compile_status();
    return info_log_length;
}

std::string glsl_shader::get_info_log(int length) const {
    std::vector<char> message(length);
    glGetShaderInfoLog(handle, length, nullptr, message.data());
    check_compile_status();
    return std::string(message.data());
}

void glsl_shader::check_compile_status() const {
    GLint info_log_length = get_info_log_length();

    if (info_log_length > 1) {
        std::cerr << "Cannot compile shader source:\nOpenGL Version: " + ogl_version_string() + "\n" +
                     "OpenGl Message: \n" +
                     get_info_log(info_log_length);
    }
}

bool glsl_shader::check_eq_shader_type(unsigned int type) const {
    GLint result = get_compile_status();
    if (result) {
        GLint stype;
        glGetShaderiv(handle, GL_SHADER_TYPE, &stype);
        check_compile_status();
        if (stype != (GLint) type) {
            std::cerr << "Incorrect shader type.";
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// glsl program
//----------------------------------------------------------------------------------------------------------------------

glsl_program::glsl_program(): ogl_handle(){

}

glsl_program::glsl_program(unsigned int handle) : ogl_handle(handle){

}

glsl_program::glsl_program(unsigned int handle, std::string name): ogl_handle(handle, name){

}

void glsl_program::create() {
    destroy();
    handle = glCreateProgram();
    assert_ogl_error();
}

void glsl_program::destroy() {
    if (is_valid()) {
        glDeleteProgram(handle);
        assert_ogl_error();
        handle = BCG_GL_INVALID_ID;
    }
}

void glsl_program::attach(const glsl_shader &shader) const {
    glAttachShader(handle, shader.handle);
    assert_ogl_error();
}

void glsl_program::detach(const glsl_shader &shader) const {
    glDetachShader(handle, shader.handle);
    assert_ogl_error();
}

bool glsl_program::build(const std::vector<glsl_shader> &shaders) {
    if (!is_valid()) {
        create();
    }
    for (const auto &shader:shaders) {
        attach(shader);
    }
    return link();
}


bool glsl_program::build(const glsl_shader *vertex_shader, const glsl_shader *fragment_shader,
                         const glsl_shader *geometry_shader,
                         const glsl_shader *tess_control_shader, const glsl_shader *tess_eval_shader) {
    std::vector<glsl_shader> shaders{*vertex_shader, *fragment_shader};
    if (geometry_shader) shaders.push_back(*geometry_shader);
#if GL_VERSION_4_0
    if (tess_control_shader) shaders.push_back(*tess_control_shader);
    if (tess_eval_shader) shaders.push_back(*tess_eval_shader);
#endif
    return build(shaders);
}

bool glsl_program::build_files(const char *vertex_shader_file, const char *fragment_shader_file,
                               const char *geometry_shader_file,
                               const char *tess_control_shader_file, const char *tess_eval_shader_file) {
    return build_files(vertex_shader_file, fragment_shader_file, geometry_shader_file, tess_control_shader_file,
                       tess_eval_shader_file, 0, nullptr);
}

bool glsl_program::build_files(const char *vertex_shader_file, const char *fragment_shader_file,
                               const char *geometry_shader_file,
                               const char *tess_control_shader_file, const char *tess_eval_shader_file,
                               const char *prepend_source) {
    return build_files(vertex_shader_file, fragment_shader_file, geometry_shader_file, tess_control_shader_file,
                       tess_eval_shader_file, 1, &prepend_source);
}

bool glsl_program::build_files(const char *vertex_shader_file, const char *fragment_shader_file,
                               const char *geometry_shader_file,
                               const char *tess_control_shader_file, const char *tess_eval_shader_file,
                               int prepend_count, const char **prepend_sources) {
    std::vector<glsl_shader> shaders;
    glsl_shader vs, fs, gs, tcs, tes;
    if (!vs.compile_file(vertex_shader_file, GL_VERTEX_SHADER, prepend_count, prepend_sources)) {
        return false;
    }
    shaders.push_back(vs);
    if (!fs.compile_file(fragment_shader_file, GL_FRAGMENT_SHADER, prepend_count, prepend_sources)) {
        return false;
    }
    shaders.push_back(fs);
    if (geometry_shader_file) {
        if (!gs.compile_file(geometry_shader_file, GL_GEOMETRY_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(gs);
    }
#ifdef GL_VERSION_4_0
    if (tess_control_shader_file) {
        if (!tcs.compile_file(tess_control_shader_file, GL_TESS_CONTROL_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(tcs);
    }
    if (tess_eval_shader_file) {
        if (!tes.compile_file(tess_eval_shader_file, GL_TESS_EVALUATION_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(tes);
    }
#endif
    return build(shaders);
}


bool glsl_program::build_sources(const char *vertex_shader_source, const char *fragment_shader_source,
                                 const char *geometry_shader_source, const char *tess_control_shader_source,
                                 const char *tess_eval_shader_source) {
    return build_sources(vertex_shader_source, fragment_shader_source, geometry_shader_source,
                         tess_control_shader_source, tess_eval_shader_source, 0, nullptr);
}

bool glsl_program::build_sources(const char *vertex_shader_source, const char *fragment_shader_source,
                                 const char *geometry_shader_source, const char *tess_control_shader_source,
                                 const char *tess_eval_shader_source, const char *prepend_source) {
    return build_sources(vertex_shader_source, fragment_shader_source, geometry_shader_source,
                         tess_control_shader_source, tess_eval_shader_source, 1, &prepend_source);
}

bool glsl_program::build_sources(const char *vertex_shader_source, const char *fragment_shader_source,
                                 const char *geometry_shader_source, const char *tess_control_shader_source,
                                 const char *tess_eval_shader_source, int prepend_count,
                                 const char **prepend_sources) {
    std::vector<glsl_shader> shaders;
    glsl_shader vs, fs, gs, tcs, tes;

    if (!vs.compile(vertex_shader_source, GL_VERTEX_SHADER, prepend_count, prepend_sources)) {
        return false;
    }
    shaders.push_back(vs);
    if (!fs.compile(fragment_shader_source, GL_FRAGMENT_SHADER, prepend_count, prepend_sources)) {
        return false;
    }
    shaders.push_back(fs);
    if (geometry_shader_source) {
        if (!gs.compile(geometry_shader_source, GL_GEOMETRY_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(gs);
    }
#ifdef GL_VERSION_4_0
    if (tess_control_shader_source) {
        if (!tcs.compile(tess_control_shader_source, GL_TESS_CONTROL_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(tcs);
    }
    if (tess_eval_shader_source) {
        if (!tes.compile(tess_eval_shader_source, GL_TESS_EVALUATION_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(tes);
    }
#else
    BCG_MAYBE_UNUSED(tess_control_shader_source);
        BCG_MAYBE_UNUSED(tess_eval_shader_source);
#endif
    return build(shaders);
}

#if GL_VERSION_4_3

bool glsl_program::build_compute_file(const char *compute_shader_file) {
    if (!is_valid()) create();
    glsl_shader cs;
    if (!cs.compile_file(compute_shader_file, GL_COMPUTE_SHADER)) {
        return false;
    }
    attach(cs);
    return link();
}

#endif

bool glsl_program::link() const {
    glLinkProgram(handle);
    assert_ogl_error();
    check_link_status();
    bind();
    return get_link_status();
}

bool glsl_program::get_link_status() const {
    GLint result = GL_FALSE;
    glGetProgramiv(handle, GL_LINK_STATUS, &result);
    assert_ogl_error();
    return result == GL_TRUE;
}

GLint glsl_program::get_info_log_length() const {
    GLint info_log_length(0);
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &info_log_length);
    assert_ogl_error();
    return info_log_length;
}

std::string glsl_program::get_info_log(GLint length) const {
    std::vector<char> message(length);
    glGetProgramInfoLog(handle, length, nullptr, message.data());
    assert_ogl_error();
    return std::string(message.data());
}

void glsl_program::check_link_status() const {
    GLint info_log_length = get_info_log_length();
    if (info_log_length > 0)
        Logger::BCG_ASSERT(true,
                           "Could not link program:\nOpenGl Message: \n" + get_info_log(info_log_length));
}

void glsl_program::bind() const {
    glUseProgram(handle);
    assert_ogl_error();
}

void glsl_program::release() const {
    glUseProgram(0);
    assert_ogl_error();
}

void glsl_program::print_active_attrinutes() const {
    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 16; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &count);
    assert_ogl_error();

    Logger::BCG_INFO("Active Attributes: " + std::to_string(count));
    for (int i = 0; i < count; i++) {
        glGetActiveAttrib(handle, (GLuint) i, bufSize, &length, &size, &type, name);
        assert_ogl_error();
        Logger::BCG_INFO("Attribute #" + std::to_string(i) + " Type: " + std::to_string(type) + " Name: " + name);
    }
    std::cout << "\n";
}

void glsl_program::print_active_uniforms() const {
    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 16; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &count);
    assert_ogl_error();

    Logger::BCG_INFO("Active Uniforms: " + std::to_string(count));
    for (int i = 0; i < count; i++) {
        glGetActiveUniform(handle, (GLuint) i, bufSize, &length, &size, &type, name);
        assert_ogl_error();
        Logger::BCG_INFO("Uniform #" + std::to_string(i) + " Type: " + std::to_string(type) + " Name: " + name);
    }
    std::cout << "\n";
}

GLuint glsl_program::get_attribute_location(const char *name) const {
    GLint index = glGetAttribLocation(handle, name);
    assert_ogl_error();
    if (index < 0) return BCG_GL_INVALID_ID;
    return GLuint(index);
}

GLint glsl_program::get_uniforlocation(const char *name) const {
    GLint index = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (index < 0) return BCG_GL_INVALID_ID;
    return index;
}

GLint glsl_program::get_uniforblock_location(const char *name) const {
    GLint index = glGetUniformBlockIndex(handle, name);
    assert_ogl_error();
    if (index < 0) return BCG_GL_INVALID_ID;
    return index;
}

GLint glsl_program::get_uniforblock_size(GLint index) const {
    GLint size = 0;
    glGetActiveUniformBlockiv(handle, index, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
    assert_ogl_error();
    return size;
}

void glsl_program::set_uniforf(const char *name, float x) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1f(id, x);
    assert_ogl_error();
}

void glsl_program::set_uniforf(const char *name, float x, float y) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2f(id, x, y);
    assert_ogl_error();
}

void glsl_program::set_uniforf(const char *name, float x, float y, float z) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3f(id, x, y, z);
    assert_ogl_error();
}

void glsl_program::set_uniforf(const char *name, float x, float y, float z, float w) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4f(id, x, y, z, w);
    assert_ogl_error();
}

void glsl_program::set_unifor1f(const char *name, int count, const float *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1fv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor2f(const char *name, int count, const float *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2fv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor3f(const char *name, int count, const float *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3fv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor4f(const char *name, int count, const float *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0)glUniform4fv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifori(const char *name, int x) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1i(id, x);
    assert_ogl_error();
}

void glsl_program::set_unifori(const char *name, int x, int y) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2i(id, x, y);
    assert_ogl_error();
}

void glsl_program::set_unifori(const char *name, int x, int y, int z) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3i(id, x, y, z);
    assert_ogl_error();
}

void glsl_program::set_unifori(const char *name, int x, int y, int z, int w) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4i(id, x, y, z, w);
    assert_ogl_error();
}

void glsl_program::set_unifor1i(const char *name, int count, const int *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1iv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor2i(const char *name, int count, const int *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2iv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor3i(const char *name, int count, const int *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3iv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor4i(const char *name, int count, const int *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4iv(id, count, data);
    assert_ogl_error();
}

#ifdef GL_VERSION_3_0

void glsl_program::set_uniforu(const char *name, GLuint x) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1ui(id, x);
    assert_ogl_error();
}

void glsl_program::set_uniforu(const char *name, GLuint x, GLuint y) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2ui(id, x, y);
    assert_ogl_error();
}

void glsl_program::set_uniforu(const char *name, GLuint x, GLuint y, GLuint z) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3ui(id, x, y, z);
    assert_ogl_error();
}

void glsl_program::set_uniforu(const char *name, GLuint x, GLuint y, GLuint z, GLuint w) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4ui(id, x, y, z, w);
    assert_ogl_error();
}

void glsl_program::set_unifor1u(const char *name, int count, const GLuint *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1uiv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor2u(const char *name, int count, const GLuint *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2uiv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor3u(const char *name, int count, const GLuint *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3uiv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor4u(const char *name, int count, const GLuint *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4uiv(id, count, data);
    assert_ogl_error();
}

#endif
#ifdef GL_VERSION_4_0

void glsl_program::set_uniford(const char *name, double x) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1d(id, x);
    assert_ogl_error();
}

void glsl_program::set_uniford(const char *name, double x, double y) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2d(id, x, y);
    assert_ogl_error();
}

void glsl_program::set_uniford(const char *name, double x, double y, double z) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3d(id, x, y, z);
    assert_ogl_error();
}

void glsl_program::set_uniford(const char *name, double x, double y, double z, double w) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4d(id, x, y, z, w);
    assert_ogl_error();
}

void glsl_program::set_unifor1d(const char *name, int count, const double *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1dv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor2d(const char *name, int count, const double *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2dv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor3d(const char *name, int count, const double *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3dv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_unifor4d(const char *name, int count, const double *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4dv(id, count, data);
    assert_ogl_error();
}

#endif

void glsl_program::set_uniformatrix_2f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2fv(id, count, transpose, m);
    assert_ogl_error();
}

void glsl_program::set_uniformatrix_3f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3fv(id, count, transpose, m);
    assert_ogl_error();
}

void glsl_program::set_uniformatrix_4f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4fv(id, count, transpose, m);
    assert_ogl_error();
}

#ifdef GL_VERSION_2_1

void
glsl_program::set_uniformatrix_2x3f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2x3fv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_2x4f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2x4fv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_3x2f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3x2fv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_3x4f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3x4fv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_4x2f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4x2fv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_4x3f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4x3fv(id, count, transpose, m);
    assert_ogl_error();
}

#endif
#ifdef GL_VERSION_4_0

void
glsl_program::set_uniformatrix_2d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_3d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_4d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_2x3d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2x3dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_2x4d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2x4dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_3x2d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3x2dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_3x4d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3x4dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_4x2d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4x2dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniformatrix_4x3d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4x3dv(id, count, transpose, m);
    assert_ogl_error();
}

#endif

//----------------------------------------------------------------------------------------------------------------------
// ogl texture
//----------------------------------------------------------------------------------------------------------------------

void ogl_texture::create() {
    destroy();
    glGenTextures(1, &handle);
    assert_ogl_error();
}

void ogl_texture::destroy() {
    if (is_valid()) {
        glDeleteTextures(1, &handle);
        assert_ogl_error();
        handle = BCG_GL_INVALID_ID;
    }
}

void ogl_texture::bind() const {
    glBindTexture(target, handle);
    assert_ogl_error();
}

void ogl_texture::release() const {
    glBindTexture(target, 0);
    assert_ogl_error();
}

void ogl_texture::activate(GLint unit) {
    unit = unit;
    activate();
}

void ogl_texture::activate() const {
    glActiveTexture(GL_TEXTURE0 + unit);
    assert_ogl_error();
    bind();
}

void ogl_texture::set_wrap_repeat() {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert_ogl_error();
}

void ogl_texture::set_wrap_mirror_repeat() {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    assert_ogl_error();
}

void ogl_texture::set_wrap_clamp_to_edge() {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    assert_ogl_error();
}

void ogl_texture::set_wrap_clamp_to_border() {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    assert_ogl_error();
    glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border_color_rgba);
    assert_ogl_error();
}

void ogl_texture::set_filter_linear() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert_ogl_error();
}

void ogl_texture::set_filter_nearest() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    assert_ogl_error();
}

void ogl_texture::generate_mipmaps() {
    glGenerateMipmap(target);
    assert_ogl_error();
}

void ogl_texture::set_mipmaps_filter_nearest_nearest() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert_ogl_error();
}

void ogl_texture::set_mipmaps_filter_linear_nearest() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert_ogl_error();
}

void ogl_texture::set_mipmaps_filter_nearest_linear() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert_ogl_error();
}

void ogl_texture::set_mipmaps_filter_linear_linear() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert_ogl_error();
}

void ogl_texture::set_image_data(GLenum target, GLint mipmap_level, GLenum internal_format, int width, int height,
                               GLenum format, GLenum type, void *data) {
    this->target = target;
    this->mipmap_level = mipmap_level;
    this->internal_format = internal_format;
    this->format = format;
    this->type = type;
    update_data(data, width, height);
}

void ogl_texture::update_data(void *data, int w, int h) {
    bind();
    if(width != w || height != h){
        width = w;
        height = h;
        if (target == GL_TEXTURE_1D) {
            glTexImage1D(target, mipmap_level, internal_format, width, 0, format, type, data);
        } else if (target == GL_TEXTURE_2D) {
            glTexImage2D(target, mipmap_level, internal_format, width, height, 0, format, type, data);
        }
    }else{
        if (target == GL_TEXTURE_1D) {
            glTexSubImage1D(target, mipmap_level, 0, width, format, type, data);
        } else if (target == GL_TEXTURE_2D) {
            glTexSubImage2D(target, mipmap_level, 0, 0, width, height, format, type, data);
        }
    }
    assert_ogl_error();
    release();
    assert_ogl_error();
}

void ogl_texture::download_data(void *data) {
    bind();
    glGetTexImage(target, mipmap_level, format, type, data);
    release();
    assert_ogl_error();
}

void ogl_texture::resize(int width, int height) {
    update_data(nullptr, width, height);
}

//----------------------------------------------------------------------------------------------------------------------
// glfw helper
//----------------------------------------------------------------------------------------------------------------------

std::string GlfwKeyToString(int key) {
    if (key == GLFW_KEY_SPACE) return "SPACE";
    if (key == GLFW_KEY_APOSTROPHE) return "\'";  /* ' */
    if (key == GLFW_KEY_COMMA) return ",";  /* , */
    if (key == GLFW_KEY_MINUS) return "-";  /* - */
    if (key == GLFW_KEY_PERIOD) return "!";  /* . */
    if (key == GLFW_KEY_SLASH) return "/";  /* / */
    if (key == GLFW_KEY_0) return "0";
    if (key == GLFW_KEY_1) return "1";
    if (key == GLFW_KEY_2) return "2";
    if (key == GLFW_KEY_3) return "3";
    if (key == GLFW_KEY_4) return "4";
    if (key == GLFW_KEY_5) return "5";
    if (key == GLFW_KEY_6) return "6";
    if (key == GLFW_KEY_7) return "7";
    if (key == GLFW_KEY_8) return "8";
    if (key == GLFW_KEY_9) return "9";
    if (key == GLFW_KEY_SEMICOLON) return ";";  /* ; */
    if (key == GLFW_KEY_EQUAL) return "=";  /* = */
    if (key == GLFW_KEY_A) return "A";
    if (key == GLFW_KEY_B) return "B";
    if (key == GLFW_KEY_C) return "C";
    if (key == GLFW_KEY_D) return "D";
    if (key == GLFW_KEY_E) return "E";
    if (key == GLFW_KEY_F) return "F";
    if (key == GLFW_KEY_G) return "G";
    if (key == GLFW_KEY_H) return "H";
    if (key == GLFW_KEY_I) return "I";
    if (key == GLFW_KEY_J) return "J";
    if (key == GLFW_KEY_K) return "K";
    if (key == GLFW_KEY_L) return "L";
    if (key == GLFW_KEY_M) return "M";
    if (key == GLFW_KEY_N) return "N";
    if (key == GLFW_KEY_O) return "O";
    if (key == GLFW_KEY_P) return "P";
    if (key == GLFW_KEY_Q) return "Q";
    if (key == GLFW_KEY_R) return "R";
    if (key == GLFW_KEY_S) return "S";
    if (key == GLFW_KEY_T) return "T";
    if (key == GLFW_KEY_U) return "U";
    if (key == GLFW_KEY_V) return "V";
    if (key == GLFW_KEY_W) return "W";
    if (key == GLFW_KEY_X) return "X";
    if (key == GLFW_KEY_Y) return "Y";
    if (key == GLFW_KEY_Z) return "Z";
    if (key == GLFW_KEY_LEFT_BRACKET) return "[";  /* [ */
    if (key == GLFW_KEY_BACKSLASH) return "\\";  /* \ */
    if (key == GLFW_KEY_RIGHT_BRACKET) return "]";  /* ] */
    if (key == GLFW_KEY_GRAVE_ACCENT) return "`"; /* ` */
    if (key == GLFW_KEY_WORLD_1) return ""; /* non-US #1 */
    if (key == GLFW_KEY_WORLD_2) return ""; /* non-US #2 */

    if (key == GLFW_KEY_ESCAPE) return "ESC";
    if (key == GLFW_KEY_ENTER) return "ENTER";
    if (key == GLFW_KEY_TAB) return "TAB";
    if (key == GLFW_KEY_BACKSPACE) return "BACKSPACE";
    if (key == GLFW_KEY_INSERT) return "INSERT";
    if (key == GLFW_KEY_DELETE) return "DELETE";
    if (key == GLFW_KEY_RIGHT) return "RIGHT";
    if (key == GLFW_KEY_LEFT) return "LEFT";
    if (key == GLFW_KEY_DOWN) return "DOWN";
    if (key == GLFW_KEY_UP) return "UP";
    if (key == GLFW_KEY_PAGE_UP) return "PAGE_UP";
    if (key == GLFW_KEY_PAGE_DOWN) return "PAGE_DOWN";
    if (key == GLFW_KEY_HOME) return "HOME";
    if (key == GLFW_KEY_END) return "END";
    if (key == GLFW_KEY_CAPS_LOCK) return "CAPS";
    if (key == GLFW_KEY_SCROLL_LOCK) return "SCROLL";
    if (key == GLFW_KEY_NUM_LOCK) return "NUM";
    if (key == GLFW_KEY_PRINT_SCREEN) return "PRINT";
    if (key == GLFW_KEY_PAUSE) return "PAUSE";
    if (key == GLFW_KEY_F1) return "F1";
    if (key == GLFW_KEY_F2) return "F2";
    if (key == GLFW_KEY_F3) return "F3";
    if (key == GLFW_KEY_F4) return "F4";
    if (key == GLFW_KEY_F5) return "F5";
    if (key == GLFW_KEY_F6) return "F6";
    if (key == GLFW_KEY_F7) return "F7";
    if (key == GLFW_KEY_F8) return "F8";
    if (key == GLFW_KEY_F9) return "F9";
    if (key == GLFW_KEY_F10) return "F10";
    if (key == GLFW_KEY_F11) return "F11";
    if (key == GLFW_KEY_F12) return "F12";
    if (key == GLFW_KEY_F13) return "F13";
    if (key == GLFW_KEY_F14) return "F14";
    if (key == GLFW_KEY_F15) return "F15";
    if (key == GLFW_KEY_F16) return "F16";
    if (key == GLFW_KEY_F17) return "F17";
    if (key == GLFW_KEY_F18) return "F18";
    if (key == GLFW_KEY_F19) return "F19";
    if (key == GLFW_KEY_F20) return "F20";
    if (key == GLFW_KEY_F21) return "F21";
    if (key == GLFW_KEY_F22) return "F22";
    if (key == GLFW_KEY_F23) return "F23";
    if (key == GLFW_KEY_F24) return "F24";
    if (key == GLFW_KEY_F25) return "F25";
    if (key == GLFW_KEY_KP_0) return "KP0";
    if (key == GLFW_KEY_KP_1) return "KP1";
    if (key == GLFW_KEY_KP_2) return "KP2";
    if (key == GLFW_KEY_KP_3) return "KP3";
    if (key == GLFW_KEY_KP_4) return "KP4";
    if (key == GLFW_KEY_KP_5) return "KP5";
    if (key == GLFW_KEY_KP_6) return "KP6";
    if (key == GLFW_KEY_KP_7) return "KP7";
    if (key == GLFW_KEY_KP_8) return "KP8";
    if (key == GLFW_KEY_KP_9) return "KP9";
    if (key == GLFW_KEY_KP_DECIMAL) return "";
    if (key == GLFW_KEY_KP_DIVIDE) return "KP_DIV";
    if (key == GLFW_KEY_KP_MULTIPLY) return "KP_MUL";
    if (key == GLFW_KEY_KP_SUBTRACT) return "KP_SUB";
    if (key == GLFW_KEY_KP_ADD) return "KP_ADD";
    if (key == GLFW_KEY_KP_ENTER) return "KP_ENTER";
    if (key == GLFW_KEY_KP_EQUAL) return "KP_EQ";
    if (key == GLFW_KEY_LEFT_SHIFT) return "LEFT_SHIFT";
    if (key == GLFW_KEY_LEFT_CONTROL) return "LEFT_CONTROL";
    if (key == GLFW_KEY_LEFT_ALT) return "LEFT_ALT";
    if (key == GLFW_KEY_LEFT_SUPER) return "LEFT_SUPER";
    if (key == GLFW_KEY_RIGHT_SHIFT) return "RIGHT_SHIFT";
    if (key == GLFW_KEY_RIGHT_CONTROL) return "RIGHT_CONTROL";
    if (key == GLFW_KEY_RIGHT_ALT) return "RIGHT_ALT";
    if (key == GLFW_KEY_RIGHT_SUPER) return "RIGHT_SUPER";
    if (key == GLFW_KEY_MENU) return "";
    return "";
}


}