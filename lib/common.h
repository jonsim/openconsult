#ifndef OPENCONSULT_LIB_COMMON
#define OPENCONSULT_LIB_COMMON

#include <memory>
#include <string>
#include <stdexcept>

/**
 * @brief C++11 compatible backport of C++20's std::format.
 *
 * Implementation based on https://stackoverflow.com/a/26221725.
 *
 * @tparam Args type of arguments to be formatted
 * @param format Format string to apply formatting to.
 * @param args Arguments to be formatted.
 * @return std::string holding the formatted result.
 */
template<class... Args>
std::string string_format(const std::string& format, Args&&... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Add space for '\0'.
    if (size_s <= 0) {
        throw std::runtime_error("Error during formatting.");
    }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside.
}

#endif