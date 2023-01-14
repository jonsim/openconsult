#ifndef OPENCONSULT_LIB_COMMON
#define OPENCONSULT_LIB_COMMON

#include <iterator>
#include <memory>
#include <string>
#include <stdexcept>

namespace cmn {

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
std::string pformat(const std::string& format, Args&&... args) {
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

/**
 * @brief Class holding a range between two iterators.
 *
 * @tparam Iter Iterator type that the range is defined by.
 */
template<class Iter>
class range {
public:
    /**
     * @brief Construct a new \c range between two iterators.
     *
     * @param begin The starting iterator.
     * @param end The 'beyond the end' iterator.
     */
    range(const Iter& begin, const Iter& end)
        : _begin(begin), _end(end) {
    }

    /**
     * @brief Construct a new \c range over a container.
     *
     * @tparam Container The container type the range is over. This type must
     *      expose methods \c Container::cbegin() and \c Container::cend() .
     * @param container The const container over which to construct the range.
     */
    template<class Container>
    range(const Container& container)
        : _begin(container.cbegin()), _end(container.cend()) {
    }

    /**
     * @brief Construct a new \c range over a container.
     *
     * @tparam Container The container type the range is over. This type must
     *      expose methods \c Container::begin() and \c Container::end() .
     * @param container The container over which to construct the range.
     */
    template<class Container>
    range(Container& container)
        : _begin(container.begin()), _end(container.end()) {
    }

    bool operator==(const range<Iter>& other) const {
        return _begin == other._begin && _end == other._end;
    }

    bool operator!=(const range<Iter>& other) const {
        return !(*this == other);
    }

    /**
     * @brief The beginning of the range.
     *
     * @return Iterator to the element at the start of the range.
     */
    const Iter& begin() const {
        return _begin;
    }

    /**
     * @brief The end of the range.
     *
     * @return Iterator to the element 'beyond the end' of the range.
     */
    const Iter& end() const {
        return _end;
    }

private:
    Iter _begin;
    Iter _end;
};

/**
 * @brief Helper to construct a range between two iterators.
 *
 * @tparam Iter Iterator type to construct the range between.
 * @param begin The starting iterator.
 * @param end The 'beyond the end' iterator.
 * @return \c range of the type inferrred from the passed iterators.
 */
template<class Iter>
range<Iter> make_range(Iter begin, Iter end) {
    return range<Iter>(begin, end);
}

/**
 * @brief Helper to construct a range over a const container.
 *
 * @tparam Container The container type the range is over. This type must
 *      expose methods \c Container::cbegin() and \c Container::cend() .
 * @tparam Iter Iterator type to use, defaulting to \c Container 's
 *      \c const_iterator type alias.
 * @param container The const container over which to construct the range.
 * @return \c range of the type inferrred from the passed container.
 */
template<class Container, class Iter = typename Container::const_iterator>
range<Iter> make_range(const Container& container) {
    return range<Iter>(container);
}

/**
 * @brief Helper to construct a range over a container.
 *
 * @tparam Container The container type the range is over. This type must
 *      expose methods \c Container::begin() and \c Container::end() .
 * @tparam Iter Iterator type to use, defaulting to \c Container 's
 *      \c iterator type alias.
 * @param container The container over which to construct the range.
 * @return \c range of the type inferrred from the passed container.
 */
template<class Container, class Iter = typename Container::iterator>
range<Iter> make_range(Container& container) {
    return range<Iter>(container);
}

}

#endif