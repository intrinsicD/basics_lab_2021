//
// Created by alex on 14.10.20.
//


#include <gtest/gtest.h>

#include "bcg_library/utils/bcg_string_utils.h"

using namespace bcg;

TEST(TestSuiteStrings, ltrim) {
    std::string test_string = "     hello";
    std::string ref = "hello";
    ltrim(test_string);
    EXPECT_EQ(ref, test_string);

    test_string = "hello      ";
    ltrim(test_string);
    EXPECT_NE(ref, test_string);
}

TEST(TestSuiteStrings, rtrim) {
    std::string test_string = "     hello";
    std::string ref = "hello";
    rtrim(test_string);
    EXPECT_NE(ref, test_string);

    test_string = "hello      ";
    rtrim(test_string);
    EXPECT_EQ(ref, test_string);
}

TEST(TestSuiteStrings, trim) {
    std::string test_string = "     hello";
    std::string ref = "hello";
    trim(test_string);
    EXPECT_EQ(ref, test_string);

    test_string = "hello      ";
    trim(test_string);
    EXPECT_EQ(ref, test_string);

    test_string = "    hello      ";
    trim(test_string);
    EXPECT_EQ(ref, test_string);
}

TEST(TestSuiteStrings, join) {
    std::string ref = "hello world!";
    EXPECT_EQ(ref, join("hello", " world!"));
}

TEST(TestSuiteStrings, split) {
    std::string hello = "hello";
    std::string world = "world!";
    auto result = split("hello world!", ' ');
    EXPECT_EQ(hello, result[0]);
    EXPECT_EQ(world, result[1]);
}

TEST(TestSuiteStrings, split2) {
    std::string first = "test";
    std::string second = " 1";
    std::string third = "2!";
    auto result = split("test, 1;2!", std::vector<char>{',',';'});
    EXPECT_EQ(first, result[0]);
    EXPECT_EQ(second, result[1]);
    EXPECT_EQ(third, result[2]);
}

TEST(TestSuiteStrings, remove_chars) {
    std::string test = "test12;";
    remove_chars(test, "12;");
    EXPECT_EQ(test, "test");
}

TEST(TestSuiteStrings, remove_non_digits) {
    std::string test = "test12;";
    remove_non_digits(test);
    EXPECT_EQ(test, "12");
}

TEST(TestSuiteStrings, parse_numbers) {
    std::string test = "test12;3.5\n4";
    auto result = parse_numbers<float>(test);
    EXPECT_EQ(result[0], 12.0f);
    EXPECT_EQ(result[1], 3.5f);
    EXPECT_EQ(result[2], 4.0f);
}