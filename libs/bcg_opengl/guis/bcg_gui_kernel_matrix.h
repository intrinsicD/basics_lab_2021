//
// Created by alex on 11.02.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_KERNEL_MATRIX_H
#define BCG_GRAPHICS_BCG_GUI_KERNEL_MATRIX_H

namespace bcg{

struct viewer_state;

struct kernel_parameters{
    int kernel_type;
    int sampling_type;
    int kernel_method_type;
};

kernel_parameters gui_kernel_matrix(viewer_state *state);

int gui_kernel_type(viewer_state *state);

int gui_kernel_method_type(viewer_state *state);

int gui_kernel_sampling_type(viewer_state *state);

}

#endif //BCG_GRAPHICS_BCG_GUI_KERNEL_MATRIX_H
