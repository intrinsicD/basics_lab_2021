//
// Created by alex on 13.10.20.
//

#include <filesystem>
#include <iostream>
#include "bcg_file.h"
#include "bcg_path.h"

namespace bcg {

// Make a path from a utf8 string
static std::filesystem::path make_path(const std::string &filename) {
    return std::filesystem::u8path(filename);
}

file_stream::file_stream(std::string filename) : filename(std::move(filename)), fs(nullptr) {}

file_stream::~file_stream() { close(); }

size_t file_stream::size() const {
    return std::filesystem::file_size(make_path(filename));
}

file_stream::operator bool() const { return check_valid(); }

file_stream::operator std::string() const {
    return filename;
}

bool file_stream::exists() const {
    return path_exists(filename);
}

bool file_stream::is_file() const {
    if (exists()) {
        return path_isfile(filename);
    }
    return make_path(filename).has_extension();
}

bool file_stream::is_directory() const {
    if (exists()) {
        return path_isdir(filename);
    }

    return !make_path(filename).has_extension();
}

bool file_stream::is_empty() const {
    return std::filesystem::is_empty(make_path(filename));
}

bool file_stream::create() {
    std::string error;
    if (!exists()) {
        if(is_file()){
            if (!make_directory(path_dirname(filename), error)) {
                std::cerr << error << "\n";
                return false;
            }
            if (open_out()) {
                close();
            } else {
                return false;
            }
        }else{
            if (!make_directory(filename, error)) {
                std::cerr << error << "\n";
                return false;
            }
        }
    }
    return exists();
}

bool file_stream::erase() {
    return std::filesystem::remove(make_path(filename));
}

bool file_stream::open_in() {
    close();
    fs.open(filename.c_str(), std::ios::in | std::ios::binary);
    return fs.good();
}

bool file_stream::open_append() {
    close();
    fs.open(filename.c_str(), std::ios::in | std::ios::binary | std::ios::app);
    return fs.good();
}

bool file_stream::open_out() {
    close();
    fs.open(filename.c_str(), std::ios::out | std::ios::binary);
    return fs.good();
}

bool file_stream::close() {
    if (fs.is_open()) {
        fs.close();
    }
    return !fs.is_open();
}

bool file_stream::check_valid() const {
    return fs.is_open();
}

time_t file_stream::last_write_time() const{
    auto tp = std::filesystem::last_write_time(make_path(filename));
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp - std::filesystem::file_time_type::clock::now()
                                                        + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sctp);
}

std::string file_stream::current_path() const {
    return path_current();
}

bool file_stream::copy_to(const std::string &filename, bool override) {
    file_stream copy(filename);

    if (!copy.exists() || override) {
        open_in();
        if (copy.open_out()) {
            copy.fs << fs.rdbuf();
            copy.close();
            return true;
        }
        close();
    }
    return false;
}

bool file_stream::move_to(const std::string &destinationname, bool override) {
    file_stream destination(destinationname);
    auto exists = destination.exists();
    if (!exists || override) {
        if (!exists) {
            destination.create();
        }
        auto result = std::rename(filename.c_str(), destinationname.c_str()) == 0;
        filename = destinationname;
        return result;
    }
    return false;
}

bool file_stream::load_text(std::string &text) {
    if (exists() && open_in()) {
        auto file_size = size();
        if (file_size == 0) {
            return close();
        }
        text = std::string(file_size, '\0');
        fs.read(&text[0], file_size);
        return close();
    }
    return false;
}

bool file_stream::save_text(const std::string &text) {
    if (open_out()) {
        fs << text;
        return close();
    }
    return false;
}

bool file_stream::load_binary(void *data, size_t size_in_bytes) {
    if (exists() && open_in() && fs.read((char *) data, size_in_bytes)) {
        return close();
    }
    close();
    return false;
}

bool file_stream::save_binary(void *data, size_t size_in_bytes) {
    if (open_out() && fs.write((const char *) data, size_in_bytes)) {
        return close();
    }
    close();
    return false;
}

}