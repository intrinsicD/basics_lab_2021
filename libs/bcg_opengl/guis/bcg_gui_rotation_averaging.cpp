//
// Created by alex on 22.04.21.
//

#include "bcg_gui_rotation_averaging.h"

namespace bcg{

bool gui_rotation_averaging(viewer_state *state, MatrixS<3, 3> &result){
    static MatrixS<3, 3> Id = MatrixS<3, 3>::Identity();
    bool computed = false;

    static bool robust = false;
    static bool outlier_reject = false;

    if(!computed){
        result = Id;
    }
    return computed;
}

}