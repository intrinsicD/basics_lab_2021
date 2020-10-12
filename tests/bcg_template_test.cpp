//
// Created by Alex on 06.05.20.
//

#include <gtest/gtest.h>

TEST(TestSuite, Example1) { // 12/2/2020 -> 737761
    EXPECT_EQ(737761, 737761);
}


TEST(TestSuite, Example2) { // 12/0/2020 -> 0
    ASSERT_EQ(0, 0);
}