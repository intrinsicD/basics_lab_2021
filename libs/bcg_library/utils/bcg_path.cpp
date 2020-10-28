//
// Created by alex on 13.10.20.
//

#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include "bcg_path.h"

namespace bcg {

// Make a path from a utf8 std::string
static std::filesystem::path make_path(const std::string &filename) {
    return std::filesystem::u8path(filename);
}

// Normalize path
std::string normalize_path(const std::string &filename) {
    return make_path(filename).generic_u8string();
}

// Get directory name (not including /)
std::string path_dirname(const std::string &filename) {
    return make_path(filename).parent_path().generic_u8string();
}

// Get extension (including .)
std::string path_extension(const std::string &filename) {
    return make_path(filename).extension().u8string();
}

// Get filename without directory.
std::string path_filename(const std::string &filename) {
    return make_path(filename).filename().u8string();
}

// Get filename without directory and extension.
std::string path_basename(const std::string &filename) {
    return make_path(filename).stem().u8string();
}

// Joins paths
std::string path_join(const std::string &patha, const std::string &pathb) {
    return (make_path(patha) / make_path(pathb)).generic_u8string();
}

std::string path_join(
        const std::string &patha, const std::string &pathb, const std::string &pathc) {
    return (make_path(patha) / make_path(pathb) / make_path(pathc))
            .generic_u8string();
}

// Replaces extensions
std::string replace_extension(const std::string &filename, const std::string &ext) {
    return make_path(filename).replace_extension(ext).u8string();
}

// Check if a file can be opened for reading.
bool path_exists(const std::string &filename) { return exists(make_path(filename)); }

// Check if a file is a directory
bool path_isdir(const std::string &filename) {
    return is_directory(make_path(filename));
}

// Check if a file is a file
bool path_isfile(const std::string &filename) {
    return is_regular_file(make_path(filename));
}

// List the contents of a directory
std::vector<std::string> list_directory(const std::string &filename) {
    auto entries = std::vector<std::string>{};
    for (auto entry : std::filesystem::directory_iterator(make_path(filename))) {
        entries.push_back(entry.path().generic_u8string());
    }
    std::sort(entries.begin(), entries.end());
    return entries;
}

// Create a directory and all missing parent directories if needed
bool make_directory(const std::string &dirname, std::string &error) {
    if (path_exists(dirname)) return true;
    try {
        create_directories(make_path(dirname));
        return true;
    } catch (...) {
        error = dirname + ": cannot create directory";
        return false;
    }
}

// Get the current directory
std::string path_current() { return std::filesystem::current_path().u8string(); }


}