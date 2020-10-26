//
// Created by alex on 26.10.20.
//

#include <memory>

#include "bcg_viewer_state.h"
#include "bcg_viewer.h"
#include "bcg_imgui.h"
#include "exts/glad/glad.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

#include "exts/imgui/imgui.h"
#include "exts/imgui/examples/imgui_impl_glfw.h"
#include "exts/imgui/examples/imgui_impl_opengl3.h"
#include "exts/imgui/imgui_internal.h"

#ifdef _WIN32
#undef near
#undef far
#endif

namespace bcg {

void init_window(viewer_state *state, const VectorI<2> &size, std::string title) {
    if (!glfwInit()) {
        throw std::runtime_error("cannot initialize windowing system");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    state->window.title = title;
    state->window.win = glfwCreateWindow(size[0], size[1], title.c_str(), nullptr, nullptr);
    if (state->window.win == nullptr) {
        throw std::runtime_error{"cannot create glfw window"};
    }
    glfwMakeContextCurrent(state->window.win);
    glfwSwapInterval(1);  // Enable vsync
    glfwSetWindowUserPointer(state->window.win, state);

    glfwSetWindowRefreshCallback(state->window.win,
                                 [](GLFWwindow *glfw) {
                                     auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                                     draw_window(state);
                                 });
    glfwSetDropCallback(state->window.win,
                        [](GLFWwindow *glfw, int num, const char **paths) {
                            auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                            if (state->callbacks.drop_cb) {
                                auto pathv = std::vector<std::string>();
                                for (auto i = 0; i < num; i++) {
                                    pathv.push_back(paths[i]);
                                }
                                state->callbacks.drop_cb(state, pathv);
                            }
                        });
    glfwSetKeyCallback(state->window.win,
                       [](GLFWwindow *glfw, int key, int scancode, int action, int mods) {
                           auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                           if (state->callbacks.key_cb) {
                               state->callbacks.key_cb(state, key, (bool) action);
                           }
                       });
    glfwSetCharCallback(state->window.win,
                        [](GLFWwindow *glfw, unsigned int key) {
                            auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                            if (state->callbacks.char_cb) {
                                state->callbacks.char_cb(state, key);
                            }
                        });
    glfwSetMouseButtonCallback(state->window.win,
                               [](GLFWwindow *glfw, int button, int action, int mods) {
                                   auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                                   if (state->callbacks.click_cb)
                                       state->callbacks.click_cb(state, button == GLFW_MOUSE_BUTTON_LEFT,
                                                                 (bool) action);
                               });
    glfwSetScrollCallback(state->window.win,
                          [](GLFWwindow *glfw, double xoffset, double yoffset) {
                              auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                              if (state->callbacks.scroll_cb) {
                                  state->callbacks.scroll_cb(state, (float) yoffset);
                              }
                          });
    //TODO glfwSetWindowSizeCallback
}

void viewer::run(VectorI<2> size, std::string title) {
    auto state_guard = std::make_unique<viewer_state>();
    auto state = state_guard.get();
    init_window(state, size, title);
    run_ui(size, title, callbacks);
}

}