//
// Created by alex on 26.10.20.
//

#include <memory>
#include <chrono>
#include <iostream>

#include "glad/glad.h"
#include "bcg_viewer_state.h"
#include "bcg_viewer.h"
#include "events/bcg_events.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

#include "bcg_imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#ifdef _WIN32
#undef near
#undef far
#endif

namespace bcg {

static void draw_window(viewer_state *state) {
    glClearColor(state->colors.background[0],
                 state->colors.background[1],
                 state->colors.background[2],
                 state->colors.background[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    state->dispatcher.trigger<event::internal::begin_frame>();
    if (state->callbacks.draw_cb) {
        state->callbacks.draw_cb(state);
    } else {
        state->dispatcher.trigger<event::internal::render>();
    }
    state->dispatcher.trigger<event::internal::end_frame>();
}

void draw_gui(viewer_state *state){
    if (!state->gui.hide_all) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (state->gui.show_menu && state->gui.menu.show) {
            state->gui.menu.render(state);
        }
        if (state->gui.left.show && state->gui.left.active) {
            state->gui.left.render(state);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void init_widgets(viewer_state *state, int width) {
    // init widgets
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::GetStyle().WindowRounding = 0;
    ImGui_ImplGlfw_InitForOpenGL(state->window.win, true);
#ifndef __APPLE__
    ImGui_ImplOpenGL3_Init();
#else
    ImGui_ImplOpenGL3_Init("#version 330");
#endif
    ImGui::StyleColorsDark();
    state->window.widgets_width = width;
}

void
init_window(viewer_state *state, const VectorI<2> &size, const std::string &title, bool widgets, int widgets_width) {
    if (!glfwInit()) {
        throw std::runtime_error("cannot initialize windowing system");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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
                            auto pathv = std::vector<std::string>();
                            for (auto i = 0; i < num; ++i) {
                                pathv.emplace_back(paths[i]);
                            }
                            if (state->callbacks.drop_cb) {
                                state->callbacks.drop_cb(state, pathv);
                            } else {
                                state->dispatcher.trigger<event::internal::file_drop>(pathv);
                            }
                        });
    glfwSetKeyCallback(state->window.win,
                       [](GLFWwindow *glfw, int key, int, int action, int) {
                           auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                           if (state->callbacks.key_cb) {
                               state->callbacks.key_cb(state, key, (bool) action);
                           } else {
                               state->dispatcher.trigger<event::internal::keyboard>(key, action);
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
                               [](GLFWwindow *glfw, int button, int action, int) {
                                   auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                                   if (state->callbacks.click_cb) {
                                       state->callbacks.click_cb(state, button == GLFW_MOUSE_BUTTON_LEFT,
                                                                 (bool) action);
                                   } else {
                                       state->dispatcher.trigger<event::internal::mouse::button>(button, action);
                                   }
                               });
    glfwSetCursorPosCallback(state->window.win,
                             [](GLFWwindow *glfw, double x, double y) {
                                 auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                                 state->dispatcher.trigger<event::internal::mouse::motion>(x, y);
                             });
    glfwSetScrollCallback(state->window.win,
                          [](GLFWwindow *glfw, double, double yoffset) {
                              auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                              if (state->callbacks.scroll_cb) {
                                  state->callbacks.scroll_cb(state, (float) yoffset);
                              } else {
                                  if (state->keyboard.ctrl_pressed) {
                                      state->dispatcher.trigger<event::internal::uniform_point_size>(yoffset);
                                  } else {
                                      state->dispatcher.trigger<event::internal::mouse::scroll>(yoffset);
                                  }
                              }
                          });
    glfwSetWindowSizeCallback(state->window.win,
                              [](GLFWwindow *glfw, int width, int height) {
                                  auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                                  state->window.width = width;
                                  state->window.height = height;
                                  int vw, vh;
                                  glfwGetFramebufferSize(glfw, &vw, &vh);
                                  state->window.framebuffer_viewport[0] = 0;
                                  state->window.framebuffer_viewport[1] = 0;
                                  state->window.framebuffer_viewport[2] = vw;
                                  state->window.framebuffer_viewport[3] = vh;
                                  state->dispatcher.trigger<event::internal::resize>(width, height);
                              });

    glfwSetWindowCloseCallback(state->window.win,
                               [](GLFWwindow *glfw) {
                                   auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                                   glfwSetWindowShouldClose(state->window.win, true);
                               });
    glfwSwapInterval(1);
    // init gl extensions
    //if (!gladLoadGL()) {
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error{"cannot initialize OpenGL extensions"};
    }

    if (widgets || state->gui.left.show || state->gui.menu.show) {
        init_widgets(state, widgets_width);
    }

    state->window.update();
    state->window.high_dpi_scaling =
            (bcg_scalar_t) state->window.framebuffer_viewport[2] / (bcg_scalar_t) state->window.width;
    if (state->window.high_dpi_scaling != 1) {
        std::cout << "highDPI scaling: " << state->window.high_dpi_scaling << "\n";
    }
    state->dispatcher.trigger<event::internal::resize>(state->window.width, state->window.height);
    std::cout << "GL       " << ogl_version_string() << "\n";
    std::cout << "GLSL     " << glsl_version_string() << "\n";
    std::cout << "Renderer " << ogl_renderer_string() << "\n";
}


void viewer::run(const VectorI<2> &size, const std::string &title, int widgets_width) {
    init_window(&state, size, title, (bool) state.callbacks.widgets_cb, widgets_width);

    if (state.callbacks.startup_cb) {
        state.callbacks.startup_cb(&state);
    } else {
        state.dispatcher.trigger<event::internal::startup>();
    }

    //------------------------------------------------------------------------------------------------------------------

    //------------------------------------------------------------------------------------------------------------------

    // loop
    auto win = state.window.win;
    while (!glfwWindowShouldClose(win)) {
        // event hadling
        glfwPollEvents();
        // update input
        state.window.update();
        state.time.update();
        state.gui.update();

        // update ui
        if (state.callbacks.uiupdate_cb && !state.gui.widgets_active) {
            state.callbacks.uiupdate_cb(&state);
        }

        // update
        if (state.callbacks.update_cb) {
            state.callbacks.update_cb(&state);
        } else {
            state.dispatcher.trigger<event::internal::update>();
        }

        // draw
        draw_window(&state);
        draw_gui(&state);

        state.mouse.is_moving = false;
        state.mouse.is_scrolling = false;
        state.mouse.scroll_value = 0;

        glfwSwapBuffers(win);
    }

    // clear
    if (state.callbacks.shutdown_cb) {
        state.callbacks.shutdown_cb(&state);
    } else {
        state.dispatcher.trigger<event::internal::shutdown>();
    }
}

}