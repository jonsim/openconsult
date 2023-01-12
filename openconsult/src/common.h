#ifndef OPENCONSULT_LIB_COMMON
#define OPENCONSULT_LIB_COMMON

#include <iterator>
#include <memory>
#include <string>
#include <stdexcept>

/**
 * @brief Formats \c args according to the printf-style format string \c format
 * and returns the result as a string.
 *
 * Implementation based on https://stackoverflow.com/a/26221725.
 *
 * @tparam Args type of arguments to be formatted
 * @param format Format string to apply formatting to.
 * @param args Arguments to be formatted.
 * @return std::string holding the formatted result.
 */
template<class... Args>
std::string string_format(const std::string& format, Args&&... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Add space for '\0'.
    if (size_s <= 0) {
        throw std::runtime_error("Error during formatting.");
    }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside.
}

/**
 * @brief C++11 compatible backport of C++20's std::advance. Increments a given
 * iterator \c n times, or until \c iter \c == \c bound , whichever comes first.
 *
 * @tparam It Iterator type to advance.
 * @param iter Iterator to advance. Mutated by this function.
 * @param n Number of steps to advance at most.
 * @param bound Bound iterator to not exceed.
 * @return dist_t Difference between \c n and the actual distance incremented.
 */
template<class It, typename dist_t = typename std::iterator_traits<It>::difference_type>
dist_t advance(It& iter, dist_t n, It bound) {
    if (n < 0) {
        // Unlike the C++20 version, this implementation doesn't support
        // decrementing advances.
        throw std::invalid_argument("Decrementing advance is unsupported.");
    }
    // TODO: specialise for std::random_access_iterator
    while (n > 0 && iter != bound) {
        n--;
        iter++;
    }
    return n;
}

#endif