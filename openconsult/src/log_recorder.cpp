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
    impl(std::unique_ptr<ByteInterface> snooped, std::ostream& stream)
        : shim(std::move(snooped))
        , log_stream(&stream)
        , current_type(LogRecordType::NONE) {
    }

    // Non-copyable.
    impl(const impl&) = delete;
    impl& operator=(const impl&) = delete;

    // Movable.
    impl(impl&& other)
            : shim(std::move(other.shim))
            , log_stream(other.log_stream)
            , current_type(other.current_type) {
        other.shim = nullptr;
        other.log_stream = nullptr;
    }
    impl& operator=(impl&& other) {
        shim = std::move(other.shim);
        log_stream = other.log_stream;
        current_type = other.current_type;
        other.shim = nullptr;
        other.log_stream = nullptr;
        return *this;
    }

    ~impl() {
        if (log_stream) {
            // Emit a final newline when closing the log. This is not required by
            // the replayer, but will allow concatenating logs together.
            *log_stream << '\n';
        }
    }

    void log(LogRecordType type, const std::vector<uint8_t>& bytes) {
        // If we're currrently logging a different type, finish the entry.
        if (type != current_type) {
            if (current_type != LogRecordType::NONE) {
                *log_stream << '\n';
            }
            switch (type) {
                case LogRecordType::READ:
                    *log_stream << 'R' << ' ';
                    break;
                case LogRecordType::WRITE:
                    *log_stream << 'W' << ' ';
                    break;
                default:
                    break;
            }
            current_type = type;
        }
        *log_stream << cmn::format_bytes(bytes);
    }

    std::unique_ptr<ByteInterface> shim;
    std::ostream* log_stream;
    LogRecordType current_type;
};


LogRecorder::LogRecorder(std::unique_ptr<ByteInterface> snooped, std::ostream& log_stream)
        : pimpl(new impl(std::move(snooped), log_stream)) {
}

LogRecorder::LogRecorder(LogRecorder&& other)
        : pimpl(std::move(other.pimpl)) {
    other.pimpl = nullptr;
}

LogRecorder& LogRecorder::operator=(LogRecorder&& other) {
    pimpl = std::move(other.pimpl);
    other.pimpl = nullptr;
    return *this;
}

LogRecorder::~LogRecorder() {
}

std::vector<uint8_t> LogRecorder::read(std::size_t size) {
    auto bytes = pimpl->shim->read(size);
    pimpl->log(LogRecordType::READ, bytes);
    return bytes;
}

void LogRecorder::write(const std::vector<uint8_t>& bytes) {
    pimpl->log(LogRecordType::WRITE, bytes);
    pimpl->shim->write(bytes);
}


}
