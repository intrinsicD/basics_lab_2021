//
// Created by alex on 12.10.20.
//

#include <gtest/gtest.h>

#include "bcg_library/utils/bcg_file.h"
#include "bcg_library/utils/bcg_path.h"

using namespace bcg;

class TestFileFixture : public ::testing::Test {
public:
    TestFileFixture() : file(""), test_file(test_data_path + "test_file.txt"),
                        empty_file(test_data_path + "empty_file.txt"),
                        does_not_exists_file(test_data_path + "doesnotexist.txt") {

    }

#ifdef _WIN32
    std::string test_data_path = "..\\..\\tests\\data\\";
#else
    std::string test_data_path = "../../tests/data/";
#endif
    file_stream file;
    file_stream test_file;
    file_stream empty_file;
    file_stream does_not_exists_file;
};

TEST_F(TestFileFixture, empty_file) {
    EXPECT_EQ("", file.filename);
    EXPECT_FALSE(file.exists());
}

TEST_F(TestFileFixture, exists) {
    EXPECT_TRUE(test_file.exists());
    EXPECT_FALSE(does_not_exists_file.exists());
}

TEST_F(TestFileFixture, is_file) {
    EXPECT_TRUE(test_file.is_file());

    file_stream test_path(test_data_path);
    EXPECT_FALSE(test_path.is_file());
}

TEST_F(TestFileFixture, is_directory) {
    EXPECT_FALSE(test_file.is_directory());

    file_stream test_path(test_data_path);
    EXPECT_TRUE(test_path.is_directory());
}

TEST_F(TestFileFixture, is_empty) {
    EXPECT_FALSE(test_file.is_empty());
    EXPECT_TRUE(empty_file.is_empty());
}

TEST_F(TestFileFixture, copy_to) {
    file_stream test_file_copy(test_data_path + "test_file_copy.txt");
    EXPECT_FALSE(test_file_copy.exists());
    test_file.copy_to(test_file_copy);
    EXPECT_TRUE(test_file_copy.exists());
    test_file_copy.erase();
}

TEST_F(TestFileFixture, move_to) {
    file_stream test_file_new(test_data_path + "test_file_new.txt");
    file_stream test_file_destination(test_data_path + "test_file_moved.txt");
    EXPECT_FALSE(test_file_new.exists());
    EXPECT_FALSE(test_file_destination.exists());
    test_file_new.create();
    EXPECT_TRUE(test_file_new.exists());
    test_file_new.move_to(test_file_destination);
    EXPECT_TRUE(test_file_destination.exists());
    test_file_destination.erase();
    EXPECT_FALSE(test_file_destination.exists());
}

TEST_F(TestFileFixture, create_erase) {
    file_stream test_file_new(test_data_path + "test_file_new.txt");
    EXPECT_FALSE(test_file_new.exists());
    test_file_new.create();
    EXPECT_TRUE(test_file_new.exists());
    test_file_new.erase();
    EXPECT_FALSE(test_file_new.exists());
}

TEST_F(TestFileFixture, load_text) {
    std::string text;
    test_file.load_text(text);
    EXPECT_EQ(text,
              "Lorem ipsum dolor sit amet,\nconsetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.\n");
}

TEST_F(TestFileFixture, load_lines) {
    std::vector<std::string> result;
    test_file.load_lines([&result](const std::string& line) {
        result.push_back(line);
    });
    EXPECT_EQ(result[0], "Lorem ipsum dolor sit amet,");
    EXPECT_EQ(result[1],
              "consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.");
}

TEST_F(TestFileFixture, save_text) {
    file_stream test_file_new(test_data_path + "test_file_new.txt");
    test_file_new.save_text("Lorem ipsum dolor sit amet,");
    std::string text;
    test_file_new.load_text(text);
    EXPECT_EQ(text, "Lorem ipsum dolor sit amet,");
    test_file_new.erase();
}

TEST_F(TestFileFixture, open_close) {
    file_stream test_file_new(test_data_path + "test_file_new.txt");
    EXPECT_FALSE(test_file_new.exists());
    test_file_new.create();
    EXPECT_TRUE(test_file_new.open_in());
    EXPECT_TRUE(test_file_new.close());
    test_file_new.erase();

    EXPECT_FALSE(test_file_new.exists());
    EXPECT_TRUE(test_file_new.open_out());
    EXPECT_TRUE(test_file_new.close());
    test_file_new.erase();
}

TEST_F(TestFileFixture, write_read) {

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

    file_stream test_file_new(test_data_path + "test_file_new.bin");
    EXPECT_FALSE(test_file_new.exists());
    test_file_new.create();
    EXPECT_TRUE(test_file_new.exists());
    test_file_new.open_out();
    EXPECT_TRUE(test_file_new.save_binary(&data, sizeof(data)));
    test_file_new.close();
    test_file_new.open_in();
    Data data1;
    EXPECT_TRUE(test_file_new.load_binary(&data1, sizeof(data1)));
    test_file_new.close();
    EXPECT_EQ(data, data1);
    test_file_new.erase();
}

TEST_F(TestFileFixture, append_bin) {

    struct Data {
        Data() : Data(0, 0.0f) {}

        Data(int a, float b) : a(a), b(b) {}

        int a;
        float b;

        bool operator==(const Data &other) const {
            return a == other.a && b == other.b;
        }
    };
    file_stream test_file_new(test_data_path + "test_file_new.bin");
    test_file_new.create();

    Data data(5, 6.05f);
    Data result;
    EXPECT_TRUE(test_file_new.save_append_binary(&data.a));
    EXPECT_TRUE(test_file_new.save_append_binary(&data.b));
    EXPECT_TRUE(test_file_new.load_binary(&result));
    EXPECT_EQ(data, result);
    test_file_new.erase();
}

TEST_F(TestFileFixture, dir_create_erase) {
    file_stream test_dir(test_data_path + "new_dir");
    EXPECT_FALSE(test_dir.exists());
    EXPECT_TRUE(test_dir.is_directory());
    EXPECT_TRUE(test_dir.create());
    EXPECT_TRUE(test_dir.exists());
    EXPECT_TRUE(test_dir.is_directory());
    EXPECT_TRUE(test_dir.is_empty());
    EXPECT_TRUE(test_dir.erase());
}

TEST_F(TestFileFixture, list) {
    file_stream test_dir(test_data_path);
    EXPECT_TRUE(test_dir.exists());
    auto result = list_directory(test_dir);
    EXPECT_TRUE(result.size() >= 2);
    EXPECT_EQ(result[0], test_data_path + "empty_file.txt");
    EXPECT_EQ(result[1], test_data_path + "test.3d");
}