#ifndef BCG_GRAPHICS_BCG_COMMONIO_H
#define BCG_GRAPHICS_BCG_COMMONIO_H

// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------

#include <array>
#include <cstdio>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <chrono>

// -----------------------------------------------------------------------------
// PRINT/FORMATTING UTILITIES
// -----------------------------------------------------------------------------
namespace bcg {

using namespace std::string_literals;

// Print a message to the console
void print_info(const std::string &msg);

// Prints a message to the console and exit with an error.
void print_fatal(const std::string &msg);

// Timer that prints as scope end. Create with `print_timed` and print with
// `print_elapsed`.
struct print_timer {
    int64_t start_time = -1;

    ~print_timer();  // print time if scope ends
};

// Print traces for timing and program debugging
print_timer print_timed(const std::string &msg);

void print_elapsed(print_timer &timer);

// Print progress
void print_progress(const std::string &message, int current, int total);

// Format duration std::std::string from nanoseconds
std::string format_duration(int64_t duration);

// Format a large integer number in human readable form
std::string format_num(uint64_t num);

}  // namespace bcg

// -----------------------------------------------------------------------------
// COMMAND LINE PARSING
// -----------------------------------------------------------------------------
namespace bcg {

// Initialize a command line parser.
struct cli_state;

cli_state make_cli(const std::string &cmd, const std::string &usage);

// parse arguments, checks for errors, and exits on error or help
void parse_cli(cli_state &cli, int argc, const char **argv);

// parse arguments and checks for errors
bool parse_cli(cli_state &cli, int argc, const char **argv, std::string &error);

// gets usage message
std::string get_usage(const cli_state &cli);

// gets whether help was invoked
bool get_help(const cli_state &cli);

// Parses an optional or positional argument. Optional arguments' names start
// with "--" or "-", otherwise they are arguments. Supports std::strings, numbers,
// boolean flags and enums.
// Many names, separated by commas, can be used for each argument.
// Boolean flags are indicated with a pair of names "--name/--no-name", so that
// both options are explicitly specified.
template<typename T>
inline void add_option(cli_state &cli, const std::string &name, T &value,
                       const std::string &usage, bool req = false);

// Parses an optional or positional argument where values can only be within a
// set of choices. Supports std::strings, integers and enums.
template<typename T>
inline void add_option(cli_state &cli, const std::string &name, T &value,
                       const std::string &usage, const std::vector<std::string> &choices, bool req = false);

// Parse all arguments left on the command line. Can only be used as argument.
template<typename T>
inline void add_option(cli_state &cli, const std::string &name, std::vector<T> &value,
                       const std::string &usage, bool req = false);

}  // namespace bcg

// -----------------------------------------------------------------------------
// PATH UTILITIES
// -----------------------------------------------------------------------------
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

}  // namespace bcg

// -----------------------------------------------------------------------------
// FILE IO
// -----------------------------------------------------------------------------
namespace bcg {

// Load/save a text file
bool load_text(const std::string &filename, std::string &str, std::string &error);

bool save_text(const std::string &filename, const std::string &str, std::string &error);

// Using directive
using byte = unsigned char;

// Load/save a binary file
bool load_binary(const std::string &filename, std::vector<byte> &data, std::string &error);

bool save_binary(
        const std::string &filename, const std::vector<byte> &data, std::string &error);

}  // namespace bcg

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// FORMATTING
// -----------------------------------------------------------------------------
namespace bcg {

// This is a very crude replacement for `std::format()` that will be used when
// available on all platforms.
template<typename... Args>
inline std::string format(const std::string &fmt, Args &&... args);

}  // namespace bcg

// -----------------------------------------------------------------------------
// FILE IO
// -----------------------------------------------------------------------------
namespace bcg {

// Safe wrapper for FILE stream
struct file_stream {
    // file parameters
    std::string filename = "";
    FILE *fs = nullptr;
    bool owned = false;

    // move-only type
    file_stream(const file_stream &) = delete;

    file_stream &operator=(const file_stream &) = delete;

    ~file_stream();

    // operator bool to check for error
    explicit operator bool() const { return fs != nullptr; }
};

// Open a file
file_stream open_file(const std::string &filename, const std::string &mode);

// Close a file
void close_file(file_stream &fs);

// Read a line of text
bool read_line(file_stream &fs, char *buffer, size_t size);

// Read a line of text
template<size_t N>
inline bool read_line(file_stream &fs, std::array<char, N> &buffer) {
    return read_line(fs, buffer.data(), buffer.size());
}

// Write text to a file
bool write_text(file_stream &fs, const std::string &str);

// Read data from a file
bool read_data(file_stream &fs, void *buffer, size_t count);

// Write data from a file
bool write_data(file_stream &fs, const void *buffer, size_t count);

// Read data from a file
template<typename T>
inline bool read_value(file_stream &fs, T &buffer) {
    return read_data(fs, &buffer, sizeof(T));
}

// Write data from a file
template<typename T>
inline bool write_value(file_stream &fs, const T &buffer) {
    return write_data(fs, &buffer, sizeof(T));
}

// Read data from a file
template<typename T>
inline bool read_values(file_stream &fs, T *buffer, size_t count) {
    return read_data(fs, buffer, sizeof(T) * count);
}

// Write data from a file
template<typename T>
inline bool write_values(file_stream &fs, const T *buffer, size_t count) {
    return write_data(fs, buffer, sizeof(T) * count);
}

template<typename T>
inline T swap_endian(T value) {
    // https://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
    static_assert(sizeof(char) == 1, "sizeof(char) == 1");
    union {
        T value;
        unsigned char bytes[sizeof(T)];
    } source, dest;
    source.value = value;
    for (auto k = (size_t) 0; k < sizeof(T); k++)
        dest.bytes[k] = source.bytes[sizeof(T) - k - 1];
    return dest.value;
}

template<typename T>
inline bool read_value(file_stream &fs, T &value, bool big_endian) {
    if (!read_value(fs, value)) return false;
    if (big_endian) value = swap_endian(value);
    return true;
}

template<typename T>
inline bool write_value(file_stream &fs, const T &value_, bool big_endian) {
    auto value = big_endian ? swap_endian(value_) : value_;
    return write_value(fs, value);
}

// Opens a file with a utf8 file name
FILE *fopen_utf8(const char *filename, const char *mode);

}  // namespace bcg

// -----------------------------------------------------------------------------
// IMPLEMENTATION OF FORMATTING
// -----------------------------------------------------------------------------
namespace bcg {

// Formats values to std::string
inline void format_value(std::string &str, const std::string &value) { str += value; }

inline void format_value(std::string &str, int8_t value) {
    str += std::to_string((int32_t) value);
}

inline void format_value(std::string &str, int16_t value) {
    str += std::to_string(value);
}

inline void format_value(std::string &str, int32_t value) {
    str += std::to_string(value);
}

inline void format_value(std::string &str, int64_t value) {
    str += std::to_string(value);
}

inline void format_value(std::string &str, uint8_t value) {
    str += std::to_string((uint32_t) value);
}

inline void format_value(std::string &str, uint16_t value) {
    str += std::to_string(value);
}

inline void format_value(std::string &str, uint32_t value) {
    str += std::to_string(value);
}

inline void format_value(std::string &str, uint64_t value) {
    str += std::to_string(value);
}

inline void format_value(std::string &str, float value) {
    auto buf = std::array<char, 256>{};
    snprintf(buf.data(), buf.size(), "%g", value);
    str += buf.data();
}

inline void format_value(std::string &str, double value) {
    auto buf = std::array<char, 256>{};
    snprintf(buf.data(), buf.size(), "%g", value);
    str += buf.data();
}

// Foramt to file
inline void format_values(std::string &str, const std::string &fmt) {
    auto pos = fmt.find("{}");
    if (pos != std::string::npos) throw std::invalid_argument("bad format std::string");
    str += fmt;
}

template<typename Arg, typename... Args>
inline void format_values(
        std::string &str, const std::string &fmt, const Arg &arg, const Args &... args) {
    auto pos = fmt.find("{}");
    if (pos == std::string::npos) throw std::invalid_argument("bad format std::string");
    str += fmt.substr(0, pos);
    format_value(str, arg);
    format_values(str, fmt.substr(pos + 2), args...);
}

template<typename... Args>
inline bool format_values(
        file_stream &fs, const std::string &fmt, const Args &... args) {
    auto str = ""s;
    format_values(str, fmt, args...);
    return write_text(fs, str);
}

template<typename T>
inline bool format_value(file_stream &fs, const T &value) {
    auto str = ""s;
    format_value(str, value);
    return write_text(fs, str);
}

}  // namespace bcg

// -----------------------------------------------------------------------------
// IMPLEMENTATION OF COMMAND-LINE PARSING
// -----------------------------------------------------------------------------
namespace bcg {

// Command line value type
enum struct cli_type {
    integer, uinteger, number, boolean, string
};
// Command line value
struct cli_value {
    cli_type type = cli_type::integer;
    int64_t integer = 0;
    uint64_t uinteger = 0;
    double number = 0;
    std::string text = "";
};
// Command line option. All data should be considered private.
struct cli_option {
    std::string name = "";
    cli_type type = cli_type::string;
    bool req = false;
    int nargs = 0;
    std::string usage = "";
    std::vector<cli_value> value = {};
    std::vector<cli_value> def = {};
    std::vector<std::string> choices = {};
    bool set = false;
    std::function<void(const std::vector<cli_value> &values)> set_reference = {};
};
// Command line parser. All data should be considered private.
struct cli_state {
    std::string name = "";
    std::string usage = "";
    std::vector<cli_option> options = {};
    std::string usage_options = "";
    std::string usage_arguments = "";
    bool help = false;
};

template<typename T>
inline cli_type get_cli_type() {
    static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, bool> ||
                  std::is_integral_v<T> || std::is_floating_point_v<T> ||
                  std::is_enum_v<T>,
                  "unsupported type");
    if constexpr (std::is_same_v<T, std::string>) {
        return cli_type::string;
    }
    if constexpr (std::is_same_v<T, bool>) {
        return cli_type::boolean;
    }
    if constexpr (std::is_integral_v<T> && !std::is_unsigned_v<T>) {
        return cli_type::integer;
    }
    if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
        return cli_type::uinteger;
    }
    if constexpr (std::is_floating_point_v<T>) {
        return cli_type::number;
    }
    return cli_type::string;
}

template<typename T>
inline void set_value(cli_value &cvalue, const T &value) {
    static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, bool> ||
                  std::is_integral_v<T> || std::is_floating_point_v<T> ||
                  std::is_enum_v<T>,
                  "unsupported type");
    cvalue.type = get_cli_type<T>();
    if constexpr (std::is_same_v<T, std::string>) {
        cvalue.text = value;
    } else if constexpr (std::is_same_v<T, bool>) {
        cvalue.integer = value ? 1 : 0;
    } else if constexpr (std::is_integral_v<T> && !std::is_unsigned_v<T>) {
        cvalue.integer = value;
    } else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
        cvalue.uinteger = value;
    } else if constexpr (std::is_floating_point_v<T>) {
        cvalue.number = value;
    } else {
        // pass
    }
}

template<typename T>
inline bool get_value(const cli_value &cvalue, T &value) {
    static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, bool> ||
                  std::is_integral_v<T> || std::is_floating_point_v<T> ||
                  std::is_enum_v<T>,
                  "unsupported type");
    if constexpr (std::is_same_v<T, std::string>) {
        if (cvalue.type != cli_type::string) return false;
        value = cvalue.text;
        return true;
    } else if constexpr (std::is_same_v<T, bool>) {
        if (cvalue.type != cli_type::boolean) return false;
        value = cvalue.integer != 0;
        return true;
    } else if constexpr (std::is_integral_v<T> && !std::is_unsigned_v<T>) {
        if (cvalue.type != cli_type::integer) return false;
        value = (T) cvalue.integer;
        return true;
    } else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
        if (cvalue.type != cli_type::uinteger) return false;
        value = (T) cvalue.uinteger;
        return true;
    } else if constexpr (std::is_floating_point_v<T>) {
        if (cvalue.type != cli_type::number) return false;
        value = (T) cvalue.number;
        return true;
    } else {
        return false;
    }
}

template<typename T>
inline void add_option(cli_state &cli, const std::string &name, T &value,
                       const std::string &usage, bool req) {
    static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, bool> ||
                  std::is_integral_v<T> || std::is_floating_point_v<T> ||
                  std::is_enum_v<T>,
                  "unsupported type");
    auto def = std::vector<cli_value>{};
    set_value(def.emplace_back(), value);
    auto &option = cli.options.emplace_back();
    option.name = name;
    option.type = get_cli_type<T>();
    option.req = req;
    option.nargs = 1;
    option.usage = usage;
    option.value = def;
    option.def = def;
    option.choices = {};
    option.set_reference = [&value](const std::vector<cli_value> &cvalues) -> bool {
        if (cvalues.size() != 1) throw std::out_of_range{"invalid number of args"};
        return get_value(cvalues.front(), value);
    };
}

template<typename T>
inline void add_option(cli_state &cli, const std::string &name, T &value,
                       const std::string &usage, const std::vector<std::string> &choices, bool req) {
    static_assert(
            std::is_same_v<T, std::string> || std::is_integral_v<T> || std::is_enum_v<T>,
            "unsupported type");
    auto def = std::vector<cli_value>{};
    set_value(def.emplace_back(), value);
    auto &option = cli.options.emplace_back();
    option.name = name;
    option.type = get_cli_type<T>();
    option.req = req;
    option.nargs = 1;
    option.usage = usage;
    option.value = def;
    option.def = def;
    option.choices = choices;
    option.set_reference = [&value](const std::vector<cli_value> &cvalues) -> bool {
        if (cvalues.size() != 1) throw std::out_of_range{"invalid number of args"};
        return get_value(cvalues.front(), value);
    };
}

template<typename T>
inline void add_option(cli_state &cli, const std::string &name, std::vector<T> &values,
                       const std::string &usage, bool req) {
    static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, bool> ||
                  std::is_integral_v<T> || std::is_floating_point_v<T> ||
                  std::is_enum_v<T>,
                  "unsupported type");
    auto def = std::vector<cli_value>{};
    for (auto &value : values) set_value(def.emplace_back(), value);
    auto &option = cli.options.emplace_back();
    option.name = name;
    option.type = get_cli_type<T>();
    option.req = req;
    option.nargs = -1;
    option.usage = usage;
    option.value = def;
    option.def = def;
    option.choices = {};
    option.set_reference = [&values](const std::vector<cli_value> &cvalues) -> bool {
        values.clear();
        for (auto &cvalue : cvalues) {
            if (!get_value(cvalue, values.emplace_back())) return false;
        }
        return true;
    };
}

}  // namespace bcg

#endif
