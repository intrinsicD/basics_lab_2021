//
// Created by alex on 08.05.20.
//

#include <gtest/gtest.h>

#include "bcg_library/utils/bcg_file_watcher.h"
#include "bcg_library/utils/bcg_file.h"

using namespace bcg;

#ifdef _WIN32
static std::string test_data_path = "..\\..\\tests\\data\\";
#else
static std::string test_data_path = "../../tests/data/";
#endif

TEST(TestSuiteFileWatcher, constructor){
    file_watcher watcher;
    EXPECT_FALSE(watcher.trigger());
    EXPECT_FALSE(watcher.trigger(true));
}

TEST(TestSuiteFileWatcher, add_file){
    file test_file(test_data_path + "test_watched_file.txt");
    test_file.create();
    file_watcher watcher;
    bool changed = false;
    watcher.watch(test_file.absolute_path(), [&changed](){
        changed = true;
    });
    sleep(1);
    test_file.save_text("lol");
    EXPECT_TRUE(watcher.trigger());
    EXPECT_TRUE(changed);
    changed = false;
    EXPECT_FALSE(watcher.trigger());
    EXPECT_TRUE(watcher.trigger(true));
    EXPECT_TRUE(changed);
    test_file.erase();
}