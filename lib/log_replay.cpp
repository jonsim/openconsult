#include <fstream>
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
    LogRecord(const std::string& line) {
        // Sanity check the provided line before we go any further. The line
        // must contain: the record type (1 character), a space separator (1
        // character), and at least 1 byte of data (2 characters). Additionally,
        // since the data is encoded to take 2 characters per byte, the line
        // must have an even length.
        if (line.length() < 4 || line.length() % 2) {
            std::string error = string_format("Failed to parse line: %s", line);
            throw std::invalid_argument(error);
        }

        // Parse type.
        switch (line[0]) {
            case 'R': type = LogRecordType::READ; break;
            case 'W': type = LogRecordType::WRITE; break;
            default:
                std::string error = string_format("Failed to parse line: %s", line);
                throw std::invalid_argument(error);
        }

        // Ensure separator is present.
        if (line[1] != ' ') {
            std::string error = string_format("Failed to parse line: %s", line);
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
    std::vector<uint8_t> data;
};

struct LogReplay::impl {
    void advanceReadCursor();
    void advanceWriteCursor();

    std::vector<uint8_t> read(std::size_t size);

    using LogRecords = std::vector<LogRecord>;
    using LogRecordsCursor = LogRecords::const_iterator;
    using LogRecordDataCursor = std::vector<uint8_t>::const_iterator;
    LogRecords records;
    LogRecordsCursor read_cursor {records.end()};
    LogRecordsCursor write_cursor {records.end()};
    LogRecordDataCursor read_data_cursor;
    LogRecordDataCursor write_data_cursor;
};


void LogReplay::impl::advanceReadCursor() {
    if (read_cursor == records.end()) {
        read_cursor = records.begin();
    } else {
        read_cursor++;
    }

    for (; read_cursor != records.end() && read_cursor->type != LogRecordType::READ; read_cursor++) {}
    read_data_cursor = read_cursor->data.begin();
}

void LogReplay::impl::advanceWriteCursor() {
    if (write_cursor == records.end()) {
        write_cursor = records.begin();
    } else {
        write_cursor++;
    }

    for (; write_cursor != records.end() && write_cursor->type != LogRecordType::WRITE; write_cursor++) {}
    write_data_cursor = write_cursor->data.begin();
}

std::vector<uint8_t> LogReplay::impl::read(std::size_t size) {
    LogRecordDataCursor start_cursor = read_data_cursor;
    std::size_t remaining = advance(read_data_cursor, size, read_cursor->data.end());
    std::vector<uint8_t> bytes {start_cursor, read_data_cursor};

    if (remaining) {
        advanceReadCursor();
        std::vector<uint8_t> additional = read(remaining);
        bytes.insert(bytes.end(), additional.begin(), additional.end());
    }
    return bytes;
}


LogReplay::LogReplay(const std::string& log_path) {
    std::ifstream log_file(log_path, std::ifstream::in);
    if (!log_file.is_open()) {
        std::string error = string_format("Failed to open %s", log_path);
        throw std::runtime_error(error);
    }
    for (std::string line; std::getline(log_file, line);) {
        impl->records.emplace_back(line);
    }
}

std::vector<uint8_t> LogReplay::read(std::size_t size) {
    return impl->read(size);
}

void LogReplay::write(std::vector<uint8_t> bytes) {

}
