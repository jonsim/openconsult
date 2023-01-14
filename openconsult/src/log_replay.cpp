#include <algorithm>
#include <istream>
#include <stdexcept>
#include <stdlib.h>
#include <vector>
#include "common.h"
#include "log_replay.h"


/**
 * @brief The type of record held in a \c LogRecord .
 */
enum class LogRecordType {
    READ,
    WRITE,
};


/**
 * @brief A single entry in a replay log.
 */
struct LogRecord {
    /// The storage type used to hold the data.
    using data_t = std::vector<uint8_t>;

    /**
     * @brief Parses a line and construct a new \c LogRecord from the result.
     *
     * @param line The line to parse.
     *
     * @throws std::invalid_argument if \c line is poorly formatted.
     */
    LogRecord(const std::string& line) {
        // The line format is as follows:
        // (R|W) [0-9a-fA-F]{2}+
        // Where,
        // - the first character denotes the record type (read or write).
        // - the second character is always a space
        // - the rest of the line is made up of one or more bytes of hex data.
        // This means the line must have an even length.
        // Sanity check the provided line before we go any further.
        if (line.length() < 4 || line.length() % 2) {
            std::string error = cmn::pformat("Failed to parse line: %s", line.c_str());
            throw std::invalid_argument(error);
        }

        // Parse type.
        switch (line[0]) {
            case 'R': type = LogRecordType::READ; break;
            case 'W': type = LogRecordType::WRITE; break;
            default:
                std::string error = cmn::pformat("Failed to parse line: %s", line.c_str());
                throw std::invalid_argument(error);
        }

        // Ensure separator is present.
        if (line[1] != ' ') {
            std::string error = cmn::pformat("Failed to parse line: %s", line.c_str());
            throw std::invalid_argument(error);
        }

        // Read data.
        data.reserve((line.length() - 2) / 2);
        for (std::size_t i = 2; i < line.length(); i += 2) {
            std::string byte_str = line.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(strtoul(byte_str.c_str(), nullptr, 16));
            data.push_back(byte);
        }
    }

    /**
     * @brief Searches for a contiguous range of \c bytes data occuring in this
     * log record.
     *
     * @tparam Iter The type of iterator used to describe the range.
     * @param bytes The range of bytes to search for.
     * @return Iterator to the first contiguous match of \c bytes data in this
     *      record, or an iterator to the end of the record's data if no such
     *      match exists.
     */
    template<class Iter>
    data_t::iterator find(cmn::range<Iter> bytes) {
        return std::search(data.begin(), data.end(), bytes.begin(), bytes.end());
    }

    /// @brief The \c LogRecordType represented by this record.
    LogRecordType type;
    /// @brief The data that is in this record.
    data_t data;
};


/// @brief Type alias for a collection of \c LogRecord s.
using LogRecords = std::vector<LogRecord>;


/**
 * @brief Iterator over the underlying data in \c LogRecords .
 */
class LogRecordsIterator : public std::iterator<std::forward_iterator_tag, uint8_t> {
    // Type aliases.
private:
    /// @brief Iterator over the individual records in a \c LogRecords .
    using records_iter_t = LogRecords::iterator;
    /// @brief Iterator over the data in an individual \c LogRecord .
    using record_iter_t = LogRecord::data_t::iterator;

    // Constructors
public:
    LogRecordsIterator() = default;
    LogRecordsIterator(const LogRecordsIterator&) = default;
    LogRecordsIterator(LogRecordsIterator&&) = default;
    LogRecordsIterator& operator=(const LogRecordsIterator&) = default;
    LogRecordsIterator& operator=(LogRecordsIterator&&) = default;

    /**
     * @brief Helper method to create a \c LogRecordsIterator to the start of
     *      the complete collection of records held by a \c LogRecords .
     *
     * @param records The \c LogRecords over which to iterate.
     * @param type The type of \c LogRecord to consider. Any \c LogRecord in \c
     *      records that does not match this type will be skipped.
     * @param wrap \c true to wrap the iterator whenever it reaches the end of
     *      \c records , \c false to only iterate through \c records once.
     * @return \c LogRecordsIterator to the start of the range.
     */
    static LogRecordsIterator begin(LogRecords& records, LogRecordType type, bool wrap = false) {
        return LogRecordsIterator(type, records.begin(), records.end(), wrap);
    }

    /**
     * @brief Helper method to create a \c LogRecordsIterator to the end of the
     *      complete collection of records held by a \c LogRecords .
     *
     * @param records The \c LogRecords over which to iterate.
     * @param type The type of \c LogRecord to consider. Any \c LogRecord in \c
     *      records that does not match this type will be skipped.
     * @param wrap \c true to wrap the iterator whenever it reaches the end of
     *      \c records , \c false to only iterate through \c records once.
     * @return \c LogRecordsIterator to the end of the range.
     */
    static LogRecordsIterator end(LogRecords& records, LogRecordType type, bool wrap = false) {
        return LogRecordsIterator(type, records.end(), records.end(), wrap);
    }

private:
    /**
     * @brief Construct a new \c LogRecordsIterator over the specified range.
     *
     * @param type The type of \c LogRecord to consider. Any \c LogRecord in \c
     *      records that does not match this type will be skipped.
     * @param begin Iterator to the first \c LogRecord to iterate from.
     * @param end Iterator to the 'beyond the last' \c LogRecord to iterate to.
     */
    LogRecordsIterator(LogRecordType type, records_iter_t begin, records_iter_t end, bool wrap)
            : record_type(type), records_cursor(begin)
            , records_begin(begin), records_end(end)
            , should_wrap(wrap) {
        resetRecordCursor(false);
    }

    // Operators
public:
    // ++prefix operator
    LogRecordsIterator& operator++() {
        // Advance the record cursor.
        record_cursor++;
        if (record_cursor == record_end) {
            // If that has caused it to now be at the end, advance the records
            // cursor and reset the record cursor.
            records_cursor++;
            resetRecordCursor(should_wrap);
        }
        return *this;
    }

    // postfix++ operator
    LogRecordsIterator operator++(int) {
        LogRecordsIterator prev = *this;
        ++(*this);
        return prev;
    }

    bool operator==(const LogRecordsIterator& other) const {
        // If records_cursor is at the end, the value of record_cursor is
        // undefined and must not be accessed. This only considers remaining
        // data in the iterator - the begin points are not compared.
        return record_type    == other.record_type &&
               records_cursor == other.records_cursor &&
               records_end    == other.records_end &&
             ((records_cursor == records_end) ||
              (record_cursor  == other.record_cursor &&
               record_end     == other.record_end));
    }

    bool operator!=(const LogRecordsIterator &other) const {
        return !(*this == other);
    }

    const uint8_t& operator*() const {
        return *record_cursor;
    }

    uint8_t& operator*() {
        return *record_cursor;
    }

    const uint8_t& operator->() const {
        return *record_cursor;
    }

    uint8_t& operator->() {
        return *record_cursor;
    }

    // Other modifiers.
public:
    /**
     * @brief Advances this iterator to a point described by another iterator.
     *
     * If \c pos is iterating over a different \c LogRecordType than this
     * iterator, this iterator will be advanced to the next legal position after
     * that described by \c pos. If \c pos is at a position beyond the end of
     * this iterator, behaviour is undefined.
     *
     * @param pos The iterator describing the position to advance this iterator
     *      to.
     */
    void advanceTo(const LogRecordsIterator& pos) {
        // Advance the records cursor and reset the record cursor.
        records_cursor = pos.records_cursor;
        resetRecordCursor(should_wrap);
        // If resetting the records cursor did not advance it, set the record
        // cursor to the correct position. Otherwise leave the record cursor
        // pointing at the beginning of the (next valid) record.
        if (records_cursor == pos.records_cursor) {
            record_cursor = pos.record_cursor;
        }
    }

private:
    /**
     * @brief Resets the cursor that describes the position in the current
     *      record to the start of the current record. If the current record
     *      does not match this iterator's type, the current record is advanced
     *      until a record matching this iterator's type is found.
     *
     * After directly modifying \c records_cursor the iterator is in an
     * undefined state until this method is called.
     *
     * @param wrap \c true if the records cursor should be wrapped to the start
     *      of the records when no more records matching the iterator's type are
     *      available. \c false to leave the iterator at the end when records
     *      matching the iterator's type are depleted. The cursor will wrap no
     *      more than once, so even when passsing \c true , it is necessary to
     *      check the \c records_cursor is not at the end before attempting to
     *      de-reference it.
     */
    void resetRecordCursor(bool wrap) {
        // Advance the records cursor until we find a valid record.
        while (records_cursor != records_end && records_cursor->type != record_type) {
            records_cursor++;
        }
        if (records_cursor != records_end) {
            // If we found a record, set the record cursor to point to it.
            record_cursor = records_cursor->data.begin();
            record_end = records_cursor->data.end();
        } else if (wrap) {
            // Otherwise, if we are wrapping, reset the records cursor back to
            // the start and then re-invoke this method (forcing wrap off to
            // prevent endlessly recursing if there are no valid records).
            records_cursor = records_begin;
            resetRecordCursor(false);
        }
    }

    // Members
private:
    LogRecordType record_type;
    records_iter_t records_cursor;
    records_iter_t records_begin;
    records_iter_t records_end;
    record_iter_t record_cursor;
    record_iter_t record_end;
    bool should_wrap;
};


struct LogReplay::impl {
    impl(std::istream& log_stream);

    std::vector<uint8_t> read(std::size_t size);
    void write(const std::vector<uint8_t>& bytes);

    LogRecords records;
    LogRecordsIterator read_cursor;
    LogRecordsIterator read_bound;
    LogRecordsIterator write_cursor;
    LogRecordsIterator write_bound;
};


LogReplay::impl::impl(std::istream& log_stream) {
    for (std::string line; std::getline(log_stream, line);) {
        records.emplace_back(line);
    }
    read_cursor  = LogRecordsIterator::begin(records, LogRecordType::READ);
    read_bound   = LogRecordsIterator::end(  records, LogRecordType::READ);
    write_cursor = LogRecordsIterator::begin(records, LogRecordType::WRITE);
    write_bound  = LogRecordsIterator::end(  records, LogRecordType::WRITE);
}

std::vector<uint8_t> LogReplay::impl::read(std::size_t size) {
    LogRecordsIterator start = read_cursor;
    std::size_t remaining = cmn::advance(read_cursor, size, read_bound);
    if (remaining) {
        throw std::runtime_error("No more read log records to replay");
    }
    return std::vector<uint8_t>(start, read_cursor);
}

void LogReplay::impl::write(const std::vector<uint8_t>& bytes) {
    // Advance the write cursor to the next position that contains a write of
    // the given byte sequence.
    write_cursor = std::search(write_cursor, write_bound, bytes.begin(), bytes.end());

    // Advance the write cursor, then advance the read cursor to it.
    // The read cursor needs to be set to the position the final byte was
    // written to. The final byte written may have caused the write cursor to
    // advance a record, skipping over any read records that immediately follow
    // the write record (and which we want to replay).
    std::size_t remaining = 0;
    if (bytes.size() > 0) {
        remaining += cmn::advance(write_cursor, bytes.size() - 1, write_bound);
    }
    read_cursor.advanceTo(write_cursor);
    remaining += cmn::advance(write_cursor, 1, write_bound);

    if (remaining) {
        throw std::runtime_error("No more write log records to replay");
    }
}


LogReplay::LogReplay(std::istream& log_stream)
        : pimpl(new impl(log_stream)) {
}

LogReplay::~LogReplay() = default;

std::vector<uint8_t> LogReplay::read(std::size_t size) {
    return pimpl->read(size);
}

void LogReplay::write(const std::vector<uint8_t>& bytes) {
    pimpl->write(bytes);
}
