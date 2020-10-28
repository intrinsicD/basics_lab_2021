//
// Created by alex on 26.10.20.
//

#include <memory>
#include <chrono>

#include "bcg_viewer_state.h"
#include "bcg_viewer.h"
#include "exts/glad/glad.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

#include "bcg_imgui.h"
#include "exts/imgui/examples/imgui_impl_glfw.h"
#include "exts/imgui/examples/imgui_impl_opengl3.h"

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
    if (state->callbacks.draw_cb) {
        state->callbacks.draw_cb(state);
    }
    if (!state->gui.hide_all) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (state->gui.show_menu && state->gui.menu.show) {
            state->gui.menu.render(state);
        }
        if(state->gui.left.show && state->gui.left.active){
            state->gui.left.render(state);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    glfwSwapBuffers(state->window.win);
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
                                    pathv.emplace_back(paths[i]);
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
    glfwSetWindowSizeCallback(state->window.win,
                              [](GLFWwindow *glfw, int width, int height) {
                                  auto state = (viewer_state *) glfwGetWindowUserPointer(glfw);
                                  glfwGetWindowSize(glfw, &state->window.width, &state->window.height);
                                  if (state->window.widgets_width) state->window.width -= state->window.widgets_width;
                                  glfwGetFramebufferSize(glfw, &state->window.framebuffer_viewport[2],
                                                         &state->window.framebuffer_viewport[3]);
                                  state->window.framebuffer_viewport[0] = 0;
                                  state->window.framebuffer_viewport[1] = 0;
                                  if (state->window.widgets_width) {
                                      int win_size[2];
                                      glfwGetWindowSize(glfw, &win_size[0], &win_size[1]);
                                      auto offset = (int) ((bcg_scalar_t) state->window.widgets_width *
                                                           (bcg_scalar_t) state->window.framebuffer_viewport[2] /
                                                           (bcg_scalar_t) win_size[0]);
                                      state->window.framebuffer_viewport[2] -= offset;
                                      if (state->gui.left.show) state->window.framebuffer_viewport[0] += offset;
                                  }
                              });

    // init gl extensions
    if (!gladLoadGL()) {
        throw std::runtime_error{"cannot initialize OpenGL extensions"};
    }

    if (widgets || state->gui.left.show || state->gui.menu.show) {
        init_widgets(state, widgets_width);
    }
}

void set_close(viewer_state *state, bool close) {
    glfwSetWindowShouldClose(state->window.win, close ? GLFW_TRUE : GLFW_FALSE);
}

void viewer::run(const VectorI<2> &size, const std::string &title, int widgets_width) {

    init_window(&state, size, title, (bool) state.callbacks.widgets_cb, widgets_width);

    if (state.callbacks.init_cb) {
        state.callbacks.init_cb(&state);
    }

    // loop
    auto win = state.window.win;
    while (!glfwWindowShouldClose(win)) {
        // update input
        state.mouse.last_cursor_position = state.mouse.cursor_position;
        auto mouse_posx = 0.0, mouse_posy = 0.0;
        glfwGetCursorPos(win, &mouse_posx, &mouse_posy);
        state.mouse.cursor_position[0] = mouse_posx;
        state.mouse.cursor_position[1] = mouse_posy;
        if (state.window.widgets_width && state.gui.left.show) {
            state.mouse.cursor_position[0] -= state.window.widgets_width;
        }
        state.mouse.left = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        if(state.mouse.left){
            state.mouse.last_left_click = state.mouse.cursor_position;
        }
        state.mouse.middle = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
        if(state.mouse.middle){
            state.mouse.last_middle_click = state.mouse.cursor_position;
        }
        state.mouse.right = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        if(state.mouse.right){
            state.mouse.last_right_click = state.mouse.cursor_position;
        }
        state.keyboard.alt_pressed = glfwGetKey(win, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
                                     glfwGetKey(win, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
        state.keyboard.shift_pressed = glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                                       glfwGetKey(win, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        state.keyboard.ctrl_pressed = glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                                      glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
        state.keyboard.command_pressed = glfwGetKey(win, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS ||
                                         glfwGetKey(win, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS;

        set_close(&state, state.keyboard.ctrl_pressed && glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS);
        state.keyboard.no_modifier = !(state.keyboard.alt_pressed ||
                                       state.keyboard.shift_pressed ||
                                       state.keyboard.ctrl_pressed ||
                                       state.keyboard.command_pressed);

        glfwGetWindowSize(win, &state.window.width, &state.window.height);
        if (state.window.widgets_width) {
            state.window.width -= state.window.widgets_width;
        }
        glfwGetFramebufferSize(win, &state.window.framebuffer_viewport[2],
                               &state.window.framebuffer_viewport[3]);
        state.window.framebuffer_viewport[0] = 0;
        state.window.framebuffer_viewport[1] = 0;
        if (state.window.widgets_width) {
            int win_size[2];
            glfwGetWindowSize(win, &win_size[0], &win_size[1]);
            auto offset = (int) ((float) state.window.widgets_width *
                                 (float) state.window.framebuffer_viewport[2] /
                                 (float) win_size[0]);
            state.window.framebuffer_viewport[2] -= offset;
            if (state.gui.left.show) {
                state.window.framebuffer_viewport[0] += offset;
            }
        }
        if (state.window.widgets_width) {
            auto io = &ImGui::GetIO();
            state.mouse.is_captured_by_gui = io->WantCaptureMouse;
            state.keyboard.is_captured_by_gui = io->WantCaptureKeyboard || io->WantTextInput;
            state.gui.widgets_active = state.mouse.is_captured_by_gui || state.keyboard.is_captured_by_gui;
        }

        // time
        state.time.clock_last = state.time.clock_now;
        state.time.clock_now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        state.time.time_now = (double) state.time.clock_now / 1000000000.0;
        state.time.time_delta = (double) (state.time.clock_now - state.time.clock_last) / 1000000000.0;

        // update ui
        if (state.callbacks.uiupdate_cb && !state.gui.widgets_active) {
            state.callbacks.uiupdate_cb(&state);
        }

        // update
        if (state.callbacks.update_cb) {
            state.callbacks.update_cb(&state);
        }

        // draw
        draw_window(&state);

        // event hadling
        glfwPollEvents();
    }

    // clear
    if (state.callbacks.clear_cb) {
        state.callbacks.clear_cb(&state);
    }
}

}