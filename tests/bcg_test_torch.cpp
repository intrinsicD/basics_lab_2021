//
// Created by alex on 15.03.21.
//

#include <gtest/gtest.h>
#include <torch/torch.h>

#include <iostream>

TEST(TestSuiteTorch, torch) {
    torch::Tensor tensor = torch::eye(3);
    std::cout << tensor << std::endl;
}
