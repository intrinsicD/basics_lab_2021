//
// Created by alex on 12.10.20.
//

#ifndef BCG_GRAPHICS_BCG_FILE_H
#define BCG_GRAPHICS_BCG_FILE_H

#include <string>
#include <fstream>
#include <utility>

namespace bcg {

struct file_stream {
    std::string filename;
    std::fstream fs;

    explicit file_stream(std::string filename);

    ~file_stream();

    size_t size() const;

    operator bool() const;

    operator std::string() const;

    bool exists() const;

    bool is_file() const;

    bool is_directory() const;

    bool is_empty() const;

    bool create();

    bool erase();

    bool open_in();

    bool open_append();

    bool open_out();

    bool close();

    bool check_valid() const;

    time_t last_write_time() const;

    std::string current_path() const;

    bool copy_to(const std::string &filename, bool override = false);

    bool move_to(const std::string &filename, bool override = false);

    bool load_text(std::string &text);

    bool save_text(const std::string &text);

    template<typename Lambda>
    bool load_lines(Lambda function);

    bool load_binary(void *data, size_t size_in_bytes);

    bool save_binary(void *data, size_t size_in_bytes);

    template<typename T>
    bool load_binary(T *data);

    template<typename T>
    bool save_binary(T *data);

    template<typename T>
    bool save_append_binary(T *data);
};

template<typename Lambda>
bool file_stream::load_lines(Lambda function) {
    if (exists() && open_in()) {
        std::string line;
        while (std::getline(fs, line)) {
            function(line);
        }
        return close();
    }
    return false;
}

template<typename T>
bool file_stream::load_binary(T *data) {
    return load_binary(data, sizeof(T));
}

template<typename T>
bool file_stream::save_binary(T *data) {
    return save_binary(data, sizeof(T));
}

template<typename T>
bool file_stream::save_append_binary(T *data) {
    if (open_append() && fs.write((const char *) data, sizeof(T))) {
        return close();
    }
    close();
    return false;
}


}

#endif //BCG_GRAPHICS_BCG_FILE_H
