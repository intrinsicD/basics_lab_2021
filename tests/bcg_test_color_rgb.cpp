//
// Created by alex on 25.03.21.
//

#include <gtest/gtest.h>

#include "bcg_library/color/bcg_color_rgb.h"

using namespace bcg;

TEST(TestSuiteColorRgb, conversion){
    for(int i = 0; i < 16777216; ++i){
        EXPECT_EQ(i, rgb2hex(hex2rgb(i)));
        Vector<int, 3> rgb = (VectorS<3>::Random().cwiseAbs() * 255).cast<int>();
        Vector<int, 3> rgb_rec = hex2rgb(rgb2hex(rgb));
        EXPECT_EQ(rgb[0], rgb_rec[0]);
        EXPECT_EQ(rgb[1], rgb_rec[1]);
        EXPECT_EQ(rgb[2], rgb_rec[2]);
    }
}