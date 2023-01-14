#include <assert.h>
#include <istream>
#include <stdexcept>
#include <stdlib.h>
#include <vector>
#include "common.h"
#include "log_replay.h"

enum class LogRecordType {
    READ,
    WRITE,
};

struct LogRecord {
    using data_type = std::vector<uint8_t>;

    LogRecord(const std::string& line) {
        // Sanity check the provided line before we go any further. The line
        // must contain: the record type (1 character), a space separator (1
        // character), and at least 1 byte of data (2 characters). Additionally,
        // since the data is encoded to take 2 characters per byte, the line
        // must have an even length.
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
        for (std::size_t i = 2; i < line.length(); i += 2) {
            std::string byte_str = line.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(strtoul(byte_str.c_str(), nullptr, 16));
            data.push_back(byte);
        }
    }

    bool contains(const std::vector<uint8_t>& bytes) {
        return false;
    }

    LogRecordType type;
    data_type data;
};

class LogRecordsIterator : public std::iterator<std::forward_iterator_tag, uint8_t> {
public:
    using LogRecords = std::vector<LogRecord>;
private:
    using records_iter_t = LogRecords::iterator;
    using record_iter_t = LogRecord::data_type::iterator;

public:
    static LogRecordsIterator begin(LogRecords& records, LogRecordType type) {
        return LogRecordsIterator(type, records.begin(), records.end());
    }

    static LogRecordsIterator end(LogRecords& records, LogRecordType type) {
        return LogRecordsIterator(type, records.end(), records.end());
    }

private:
    LogRecordsIterator(LogRecordType type, records_iter_t begin, records_iter_t end)
            : record_type(type), records_cursor(begin), records_bound(end) {
        assignRecordCursor();
    }

    void assignRecordCursor() {
        while (records_cursor != records_bound && records_cursor->type != record_type) {
            records_cursor++;
        }
        if (records_cursor != records_bound) {
            record_cursor = records_cursor->data.begin();
            record_bound = records_cursor->data.end();
        }
    }

    LogRecordType record_type;
    records_iter_t records_cursor;
    records_iter_t records_bound;
    record_iter_t record_cursor;
    record_iter_t record_bound;

public:
    LogRecordsIterator() = default;
    LogRecordsIterator(const LogRecordsIterator&) = default;
    LogRecordsIterator(LogRecordsIterator&&) = default;
    LogRecordsIterator& operator=(const LogRecordsIterator&) = default;
    LogRecordsIterator& operator=(LogRecordsIterator&&) = default;

    // ++prefix operator
    LogRecordsIterator& operator++() {
        if (record_cursor != record_bound) {
            record_cursor++;
        } else {
            records_cursor++;
            assignRecordCursor();
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
        return record_type    == other.record_type &&
               records_cursor == other.records_cursor &&
               records_bound  == other.records_bound &&
             ((records_cursor == records_bound) ||
              (record_cursor  == other.record_cursor &&
               record_bound   == other.record_bound));
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
};

struct LogReplay::impl {
    impl(std::istream& log_stream);

    std::vector<uint8_t> read(std::size_t size);

    using LogRecords = std::vector<LogRecord>;
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
    std::vector<uint8_t> bytes(start, read_cursor);
    assert(remaining == 0);
    return bytes;
}


LogReplay::LogReplay(std::istream& log_stream)
        : pimpl(new impl(log_stream)) {
}

LogReplay::~LogReplay() = default;

std::vector<uint8_t> LogReplay::read(std::size_t size) {
    return pimpl->read(size);
}

void LogReplay::write(const std::vector<uint8_t>& bytes) {
}
