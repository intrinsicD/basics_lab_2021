//
// Created by alex on 12.10.20.
//

#ifndef BCG_GRAPHICS_BCG_FILE_H
#define BCG_GRAPHICS_BCG_FILE_H

#include <string>
#include <fstream>
#include <filesystem>
#include <utility>

namespace bcg {

struct file {
    std::filesystem::path file_path;
    std::fstream fs;

    explicit file(std::string filename);

    ~file();

    inline std::string absolute_path() const;

    inline std::string relative_path() const;

    inline std::string filename() const;

    inline std::string extension() const;

    inline size_t size() const;

#ifdef _WIN32
    inline     wchar_t separator() const;
#else

    inline char separator() const;

#endif

    inline operator std::string() const;

    inline operator bool() const;

    inline bool exists() const;

    inline bool is_file() const;

    inline bool is_directory() const;

    inline bool is_empty() const;

    inline bool create();

    inline bool erase();

    inline bool open_in();

    inline bool open_out();

    inline bool close();

    inline bool check_valid() const;

    inline bool copy_to(const std::string &filename, bool override = false);

    inline bool move_to(const std::string &filename, bool override = false);

    inline bool load_text(std::string &text);

    inline bool save_text(const std::string &text);

    template<typename Lambda>
    inline bool load_lines(Lambda function);

    inline bool load_binary(void *data, size_t size_in_bytes);

    inline bool save_binary(void *data, size_t size_in_bytes);

    template<typename T>
    inline bool load_binary(T *data);

    template<typename T>
    inline bool save_binary(T *data);

    template<typename T>
    inline bool save_append_binary(T *data);

    inline std::vector<std::string> list_directory() const;
};

inline file::file(std::string filename) : file_path(std::move(filename)), fs(nullptr) {}

inline file::~file() { close(); }

inline std::string file::absolute_path() const {
    return file_path.string();
}

inline std::string file::relative_path() const {
    return file_path.relative_path();
}

inline std::string file::filename() const {
    return file_path.filename();
}

inline std::string file::extension() const {
    return file_path.extension();
}

inline size_t file::size() const {
    return std::filesystem::file_size(file_path);
}

#ifdef _WIN32
inline wchar_t file::separator() const{
    return std::filesystem::path::preferred_separator;
}
#else

inline char file::separator() const {
    return std::filesystem::path::preferred_separator;
}

#endif

inline file::operator std::string() const {
    return absolute_path();
}

inline file::operator bool() const { return check_valid(); }

inline bool file::exists() const {
    return std::filesystem::exists(file_path);
}

inline bool file::is_file() const {
    if (exists()) {
        return std::filesystem::is_regular_file(file_path);
    }
    if (file_path.has_extension()) {
        return true;
    }
    return false;
}

inline bool file::is_directory() const {
    if (exists()) {
        return std::filesystem::is_directory(file_path);
    }
    if (file_path.has_extension()) {
        return false;
    }
    return true;
}

inline bool file::is_empty() const {
    return std::filesystem::is_empty(file_path);
}

inline bool file::create() {
    if (!exists() && is_directory()) {
        std::filesystem::create_directories(absolute_path());
    }
    if (is_file()) {
        if (open_out()) {
            close();
        } else {
            return false;
        }
    }

    return exists();
}

inline bool file::erase() {
    return std::filesystem::remove(file_path);
}

inline bool file::open_in() {
    close();
    fs.open(absolute_path().c_str(), std::ios::in | std::ios::binary);
    return fs.good();
}

inline bool file::open_out() {
    close();
    fs.open(absolute_path().c_str(), std::ios::out | std::ios::binary);
    return fs.good();
}

inline bool file::close() {
    if (fs.is_open()) {
        fs.close();
    }
    return !fs.is_open();
}

inline bool file::check_valid() const {
    return fs.is_open();
}

bool file::copy_to(const std::string &filename, bool override) {
    file copy(filename);

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

inline bool file::move_to(const std::string &filename, bool override) {
    file destination(filename);
    auto exists = destination.exists();
    if (!exists || override) {
        if (!exists) {
            destination.create();
        }
        auto result = std::rename(absolute_path().c_str(), filename.c_str()) == 0;
        file_path = destination.file_path;
        return result;
    }
    return false;
}

inline bool file::load_text(std::string &text) {
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

inline bool file::save_text(const std::string &text) {
    if (open_out()) {
        fs << text;
        return close();
    }
    return false;
}

template<typename Lambda>
inline bool file::load_lines(Lambda function) {
    if (exists() && open_in()) {
        std::string line;
        while (std::getline(fs, line)) {
            function(line);
        }
        return close();
    }
    return false;
}

inline bool file::load_binary(void *data, size_t size_in_bytes) {
    if (exists() && open_in() && fs.read((char *) data, size_in_bytes)) {
        return close();
    }
    close();
    return false;
}

inline bool file::save_binary(void *data, size_t size_in_bytes) {
    if (open_out() && fs.write((const char *) data, size_in_bytes)) {
        return close();
    }
    close();
    return false;
}

template<typename T>
inline bool file::load_binary(T *data) {
    return load_binary(data, sizeof(T));
}

template<typename T>
inline bool file::save_binary(T *data) {
    return save_binary(data, sizeof(T));
}

template<typename T>
inline bool file::save_append_binary(T *data) {
    fs.open(absolute_path().c_str(), std::ios::out | std::ios::binary | std::ios::app);
    if (fs.is_open()) {
        fs.write((char *) data, sizeof(T));
        return close();
    }
    close();
    return false;
}

inline std::vector<std::string> file::list_directory() const {
    auto entries = std::vector<std::string>{};
    if (is_directory()) {
        for (const auto &entry : std::filesystem::directory_iterator(file_path)) {
            entries.push_back(entry.path().generic_u8string());
        }
    } else if (is_file()) {
        for (const auto &entry : std::filesystem::directory_iterator(file_path.parent_path())) {
            entries.push_back(entry.path().generic_u8string());
        }
    }
    return entries;
}

}

#endif //BCG_GRAPHICS_BCG_FILE_H
