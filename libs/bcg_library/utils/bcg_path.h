//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_PATH_H
#define BCG_GRAPHICS_BCG_PATH_H

#include <string>
#include <vector>

namespace bcg {

// Utility to normalize a path
std::string normalize_path(const std::string &filename);

// Get directory name (not including '/').
std::string path_dirname(const std::string &filename);

// Get extension (including '.').
std::string path_extension(const std::string &filename);

// Get filename without directory.
std::string path_filename(const std::string &filename);

// Get filename without directory and extension.
std::string path_basename(const std::string &filename);

// Joins paths
std::string path_join(const std::string &patha, const std::string &pathb);

std::string path_join(const std::string &patha, const std::string &pathb, const std::string &pathc);

// Replaces extensions
std::string replace_extension(const std::string &filename, const std::string &ext);

// Check if a file can be opened for reading.
bool path_exists(const std::string &filename);

// Check if a file is a directory
bool path_isdir(const std::string &filename);

// Check if a file is a file
bool path_isfile(const std::string &filename);

// List the contents of a directory
std::vector<std::string> list_directory(const std::string &filename);

// Create a directory and all missing parent directories if needed
bool make_directory(const std::string &dirname, std::string &error);

// Get the current directory
std::string path_current();

}

#endif //BCG_GRAPHICS_BCG_PATH_H
