//
// Created by alex on 08.12.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_STATISTICS_H
#define BCG_GRAPHICS_BCG_GUI_STATISTICS_H

#include <vector>
#include "math/statistics/bcg_statistics_running.h"

namespace bcg{

struct viewer_state;

void gui_statistics(viewer_state *state, const running_stats *stats, const std::vector<float> *values = nullptr);

}

#endif //BCG_GRAPHICS_BCG_GUI_STATISTICS_H
