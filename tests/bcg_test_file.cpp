//
// Created by alex on 12.10.20.
//

#include <gtest/gtest.h>

#include "bcg_library/utils/bcg_file.h"

using namespace bcg;

#ifdef _WIN32
static std::string test_data_path = "..\\..\\tests\\data\\";
#else
static std::string test_data_path = "../../tests/data/";
#endif

TEST(TestSuiteFiles, empty_file) {
    file file("");
    EXPECT_EQ("", file.file_path);
    EXPECT_FALSE(file.exists());
}

TEST(TestSuiteFiles, exists) {
    file test_file(test_data_path + "test_file.txt");
    EXPECT_TRUE(test_file.exists());
    file does_not_exists_file(test_data_path + "doesnotexist.txt");
    EXPECT_FALSE(does_not_exists_file.exists());
}

TEST(TestSuiteFiles, is_file) {
    file test_file(test_data_path + "test_file.txt");
    EXPECT_TRUE(test_file.is_file());

    file test_path(test_data_path);
    EXPECT_FALSE(test_path.is_file());
}

TEST(TestSuiteFiles, is_directory) {
    file test_file(test_data_path + "test_file.txt");
    EXPECT_FALSE(test_file.is_directory());

    file test_path(test_data_path);
    EXPECT_TRUE(test_path.is_directory());
}

TEST(TestSuiteFiles, is_empty) {
    file test_file(test_data_path + "test_file.txt");
    EXPECT_FALSE(test_file.is_empty());
    file empty_file(test_data_path + "empty_file.txt");
    EXPECT_TRUE(empty_file.is_empty());
}

TEST(TestSuiteFiles, copy_to) {
    file test_file(test_data_path + "test_file.txt");
    file test_file_copy(test_data_path + "test_file_copy.txt");
    EXPECT_FALSE(test_file_copy.exists());
    test_file.copy_to(test_file_copy);
    EXPECT_TRUE(test_file_copy.exists());
    test_file_copy.erase();
}

TEST(TestSuiteFiles, move_to) {
    file test_file(test_data_path + "test_file_new.txt");
    file test_file_destination(test_data_path + "test_file_moved.txt");
    EXPECT_FALSE(test_file.exists());
    EXPECT_FALSE(test_file_destination.exists());
    test_file.create();
    EXPECT_TRUE(test_file.exists());
    test_file.move_to(test_file_destination);
    EXPECT_TRUE(test_file_destination.exists());
    test_file_destination.erase();
    EXPECT_FALSE(test_file_destination.exists());
}

TEST(TestSuiteFiles, create_erase) {
    file test_file(test_data_path + "test_file_new.txt");
    EXPECT_FALSE(test_file.exists());
    test_file.create();
    EXPECT_TRUE(test_file.exists());
    test_file.erase();
    EXPECT_FALSE(test_file.exists());
}

TEST(TestSuiteFiles, load_text) {
    file test_file(test_data_path + "test_file.txt");
    std::string text;
    test_file.load_text(text);
    EXPECT_EQ(text, "Lorem ipsum dolor sit amet,\nconsetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.\n");
}

TEST(TestSuiteFiles, load_lines) {
    file test_file(test_data_path + "test_file.txt");
    std::vector<std::string> result;
    test_file.load_lines([&result](std::string line) {
        result.push_back(line);
    });
    EXPECT_EQ(result[0], "Lorem ipsum dolor sit amet,");
    EXPECT_EQ(result[1],
              "consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.");
}

TEST(TestSuiteFiles, save_text) {
    file test_file(test_data_path + "test_file_new.txt");
    test_file.save_text("Lorem ipsum dolor sit amet,");
    std::string text;
    test_file.load_text(text);
    EXPECT_EQ(text, "Lorem ipsum dolor sit amet,");
    test_file.erase();
}

TEST(TestSuiteFiles, open_close) {
    file test_file(test_data_path + "test_file_new.txt");
    EXPECT_FALSE(test_file.exists());
    test_file.create();
    EXPECT_TRUE(test_file.open_in());
    EXPECT_TRUE(test_file.close());
    test_file.erase();

    EXPECT_FALSE(test_file.exists());
    EXPECT_TRUE(test_file.open_out());
    EXPECT_TRUE(test_file.close());
    test_file.erase();
}

TEST(TestSuiteFiles, write_read) {

    struct Data {
        Data() : Data(0, 0.0f) {}

        Data(int a, float b) : a(a), b(b) {}

        int a;
        float b;

        bool operator==(const Data &other) const {
            return a == other.a && b == other.b;
        }
    };
    Data data(5, 6.05f);

    file test_file(test_data_path + "test_file_new.bin");
    test_file.create();
    test_file.open_out();
    EXPECT_TRUE(test_file.save_binary(&data, sizeof(data)));
    test_file.close();
    test_file.open_in();
    Data data1;
    EXPECT_TRUE(test_file.load_binary(&data1, sizeof(data1)));
    test_file.close();
    EXPECT_EQ(data, data1);
    test_file.erase();
}

TEST(TestSuiteFiles, append_bin) {

    struct Data {
        Data() : Data(0, 0.0f) {}

        Data(int a, float b) : a(a), b(b){}

        int a;
        float b;

        bool operator==(const Data &other) const {
            return a == other.a && b == other.b;
        }
    };
    file test_file(test_data_path + "test_file_new.bin");
    test_file.create();

    Data data(5, 6.05f);
    Data result;
    EXPECT_TRUE(test_file.save_append_binary(&data.a));
    EXPECT_TRUE(test_file.save_append_binary(&data.b));
    EXPECT_TRUE(test_file.load_binary(&result));
    EXPECT_EQ(data, result);
    test_file.erase();
}

TEST(TestSuiteFiles, dir_create_erase) {
    file test_dir(test_data_path + "new_dir");
    EXPECT_FALSE(test_dir.exists());
    EXPECT_TRUE(test_dir.is_directory());
    EXPECT_TRUE(test_dir.create());
    EXPECT_TRUE(test_dir.exists());
    EXPECT_TRUE(test_dir.is_directory());
    EXPECT_TRUE(test_dir.is_empty());
    EXPECT_TRUE(test_dir.erase());
}

TEST(TestSuiteFiles, list) {
    file test_dir(test_data_path);
    EXPECT_TRUE(test_dir.exists());
    auto result = test_dir.list_directory();
    EXPECT_TRUE(result.size() >= 2);
    EXPECT_EQ(result[0], test_data_path + "empty_file.txt");
    EXPECT_EQ(result[1], test_data_path + "test_file.txt");
}