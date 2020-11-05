//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_FILE_WATCHER_H
#define BCG_GRAPHICS_BCG_FILE_WATCHER_H

#include <unordered_map>
#include <filesystem>
#include <functional>
#include <algorithm>
#include <string>
#include "bcg_file.h"

namespace bcg {

struct file_watcher {
    std::unordered_map<std::string, std::function<bool(bool)>> watched;

    bool trigger(bool force_trigger = false) {
        bool some_file_changed = false;
        std::for_each(watched.begin(), watched.end(), [&some_file_changed, &force_trigger](auto &item){
            some_file_changed |= item.second(force_trigger);
        });
        return some_file_changed;
    }

    template<typename Lambda>
    void watch(const std::string&, Lambda cb);
};
}

namespace bcg {

template<typename Lambda>
void file_watcher::watch(const std::string& filename, Lambda cb) {
    //auto time = std::filesystem::last_write_time(filename);
    auto time = file_stream(filename).last_write_time();
    watched[filename] = [filename, cb, time](bool force = false) mutable {
        //auto last_mod_time = std::filesystem::last_write_time((filename));
        auto last_mod_time = file_stream(filename).last_write_time();
        if (force || time != last_mod_time) {
            time = last_mod_time;
            cb();
            return true;
        }
        return false;
    };
}

}

#endif //BCG_GRAPHICS_BCG_FILE_WATCHER_H
