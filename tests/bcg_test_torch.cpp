//
// Created by alex on 18.03.21.
//

#include <gtest/gtest.h>

#include <torch/torch.h>

TEST(TestSuiteTorch, torch) {
    torch::Tensor tensor = torch::rand({2, 3});
    std::cout << tensor << std::endl;
}