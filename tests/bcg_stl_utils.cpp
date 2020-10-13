//
// Created by alex on 12.10.20.
//

#include <gtest/gtest.h>
#include <numeric>

#include "bcg_library/utils/bcg_stl_utils.h"

using namespace bcg;

TEST(TestSuiteStlUtils, zip) {
    std::vector<float> values1(10, 0);
    values1[5] = 1.0;
    std::vector<std::string> values2(10, "hello");
    values2[6] = "world";
    values2[8] = "bunny";
    auto pairs = zip(values1, values2);
    for(size_t i = 0; i < pairs.size(); ++i){
        EXPECT_EQ(pairs[i].first, values1[i]);
        EXPECT_EQ(pairs[i].second, values2[i]);
    }
}

TEST(TestSuiteStlUtils, unzip) {
    std::vector<float> values1(10, 0);
    values1[5] = 1.0;
    std::vector<std::string> values2(10, "hello");
    values2[6] = "world";
    values2[8] = "bunny";
    auto pairs = zip(values1, values2);
    unzip(pairs, &values1, &values2);
    for(size_t i = 0; i < pairs.size(); ++i){
        EXPECT_EQ(pairs[i].first, values1[i]);
        EXPECT_EQ(pairs[i].second, values2[i]);
    }
}


TEST(TestSuiteStlUtils, sort_by_dirst) {
    std::vector<float> values1 = { 2, 1, 5, 4,9,8,3,6,7, 0};
    std::vector<int> values2 = { 7, 8, 4, 5,0,1,6,3,2, 9};

    sort_by_first(values1, values2);
    auto max = 9;
    for(size_t i = 0; i < values2.size(); ++i){
        EXPECT_EQ(values1[i], i);
        EXPECT_EQ(values2[i], max - i);
    }
}

TEST(TestSuiteStlUtils, range){
    size_t counter = 0;
    for(const auto &i : range<int>(0, 100)){
        EXPECT_EQ(i, counter++);
    }
}

TEST(TestSuiteStlUtils, enumerate){
    std::vector<int> vec(100);
    std::iota(vec.begin(), vec.end(), 0);
    size_t counter = 0;
    for(const auto &item : enumerate(vec)){
        EXPECT_EQ(item.first, counter);
        EXPECT_EQ(item.second, counter++);
    }
}