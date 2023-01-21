#include "log_recorder.h"
#include "common.h"

namespace openconsult {


/**
 * @brief The type of record currently being logged.
 */
enum class LogRecordType {
    READ,
    WRITE,
    NONE,
};


struct LogRecorder::impl {
    impl(std::ostream& stream) : log_stream(stream), current_type(LogRecordType::NONE) {}

    std::ostream& log_stream;
    LogRecordType current_type;
};


LogRecorder::LogRecorder(std::ostream& log_stream)
        : pimpl(new impl(log_stream)) {
}

LogRecorder::~LogRecorder() {
    // Emit a final newline when closing the log. This is not required by the
    // replayer, but will allow concatenating logs together.
    pimpl->log_stream << '\n';
}

void LogRecorder::read(const std::vector<uint8_t>& bytes) {
    // If we're currrently logging a write, finish the entry.
    // Then, if we're logging a write or nothing, start a read entry.
    // Then log the transaction.
    switch (pimpl->current_type) {
        case LogRecordType::WRITE:
            pimpl->log_stream << '\n';
        case LogRecordType::NONE:
            pimpl->log_stream << 'R' << ' ';
            pimpl->current_type = LogRecordType::READ;
        case LogRecordType::READ:
            pimpl->log_stream << cmn::format_bytes(bytes);
            break;
    }
}

void LogRecorder::write(const std::vector<uint8_t>& bytes) {
    // If we're currrently logging a read, finish the entry.
    // Then, if we're logging a read or nothing, start a write entry.
    // Then log the transaction.
    switch (pimpl->current_type) {
        case LogRecordType::READ:
            pimpl->log_stream << '\n';
        case LogRecordType::NONE:
            pimpl->log_stream << 'W' << ' ';
            pimpl->current_type = LogRecordType::WRITE;
        case LogRecordType::WRITE:
            pimpl->log_stream << cmn::format_bytes(bytes);
            break;
    }
}


}
