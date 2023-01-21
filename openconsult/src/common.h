#ifndef OPENCONSULT_LIB_COMMON
#define OPENCONSULT_LIB_COMMON

#include <iomanip>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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
 * @brief Formats \c bytes into a string with the numeric values represented in
 * zero-padded hex, with no separator between bytes.
 *
 * @param bytes Vector of bytes to format.
 * @return Formatted string representing the bytes.
 */
std::string format_bytes(const std::vector<uint8_t>& bytes) {
    std::ostringstream sstream;
    sstream << std::hex << std::setfill('0');
    for (const uint8_t& byte : bytes) {
        sstream << std::setw(2) << static_cast<uint32_t>(byte);
    }
    return sstream.str();
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
 * @brief Class holding a range between two iterators. This class is itself an
 * iterator - incrementing which will increment the beginning of the range.
 *
 * @tparam Iter Type of the iterator that the range is defined by.
 * @tparam T    Type of the values that can be obtained by dereferencing Iter.
 *      This type should be void for output iterators.
 */
template<class Iter, class T = typename Iter::value_type>
class range {
public:
    /// @brief The iterator type that defines this range.
    using iterator = Iter;
    /// @brief Type of the values obtained by deferencing this iterator.
    using value_type = T;
    /// @brief The category of the iterator defining this range.
    using iterator_category = typename iterator::iterator_category;
    /// @brief Integer type representing the difference between this range's
    /// iterators.
    using difference_type = typename iterator::difference_type;
    using pointer = typename iterator::pointer;
    using reference = typename iterator::reference;

    /**
     * @brief Construct a new \c range between two iterators. \c end must be
     * reachable from \c start .
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

    // ++prefix operator
    range<iterator, value_type>& operator++() {
        _begin++;
        return *this;
    }

    // postfix++ operator
    range<iterator, value_type> operator++(int) {
        range<iterator, value_type> prev = *this;
        ++(*this);
        return prev;
    }

    bool operator==(const range<iterator, value_type>& other) const {
        return _begin == other._begin && _end == other._end;
    }

    bool operator!=(const range<iterator, value_type>& other) const {
        return !(*this == other);
    }

    reference operator*() {
        return *_begin;
    }

    pointer operator->() {
        return &(*_begin);
    }

    /**
     * @brief The beginning of the range.
     *
     * @return Iterator to the element at the start of the range.
     */
    const iterator& begin() const {
        return _begin;
    }

    /**
     * @brief The end of the range.
     *
     * @return Iterator to the element 'beyond the end' of the range.
     */
    const iterator& end() const {
        return _end;
    }

    /**
     * @brief Determines if this range is empty.
     *
     * @return \c true if this range has no elements, \c false otherwise.
     */
    bool empty() const {
        return _begin == _end;
    }

    /**
     * @brief Calculates the size of this range.
     *
     * @return The number of elements remaining in the range.
     */
    difference_type size() const {
        return std::distance(_begin, _end);
    }

private:
    iterator _begin;
    iterator _end;
};

/**
 * @brief Helper to construct a range between two iterators.
 *
 * @tparam Iter Iterator type to construct the range between.
 * @param begin The starting iterator.
 * @param end The 'beyond the end' iterator.
 * @return \c range of the type inferrred from the passed iterators.
 */
template<class iterator>
range<iterator> make_range(iterator begin, iterator end) {
    return range<iterator>(begin, end);
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
template<class Container,
         class iterator = typename Container::const_iterator,
         class value_type = typename Container::value_type>
range<iterator, value_type> make_range(const Container& container) {
    return range<iterator, value_type>(container);
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
template<class Container,
         class iterator = typename Container::iterator,
         class value_type = typename Container::value_type>
range<iterator, value_type> make_range(Container& container) {
    return range<iterator, value_type>(container);
}


}

#endif