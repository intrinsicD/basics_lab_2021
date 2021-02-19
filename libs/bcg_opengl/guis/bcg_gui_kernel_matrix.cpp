//
// Created by alex on 11.02.21.
//


#include "bcg_gui_kernel_matrix.h"
#include "bcg_viewer_state.h"
#include "math/matrix/bcg_matrix_kernel.h"

namespace bcg{

int gui_kernel_type(viewer_state *state){
    static auto kernel_names = kernel_type_names();
    static int kernel_type = 0;
    draw_combobox(&state->window, "kernel_type", kernel_type, kernel_names);
    return kernel_type;
}

int gui_kernel_method_type(viewer_state *state){
    static auto kernel_method_names = kernel_method_type_names();
    static int kernel_method_type = 0;
    draw_combobox(&state->window, "kernel_method_type", kernel_method_type, kernel_method_names);
    return kernel_method_type;
}

int gui_kernel_sampling_type(viewer_state *state){
    static auto sampling_names = sampling_type_names();
    static int kernel_sampling_type = 0;
    draw_combobox(&state->window, "sampling_type", kernel_sampling_type, sampling_names);
    return kernel_sampling_type;
}

kernel_parameters gui_kernel_matrix(viewer_state *state){
    kernel_parameters params;
    params.kernel_type = gui_kernel_type(state);
    params.kernel_method_type = gui_kernel_method_type(state);
    params.sampling_type = gui_kernel_sampling_type(state);
    return params;
}

}