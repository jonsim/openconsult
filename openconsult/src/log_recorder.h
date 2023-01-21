#ifndef OPENCONSULT_LIB_LOG_RECORDER
#define OPENCONSULT_LIB_LOG_RECORDER

#include "byte_interface.h"

#include <ostream>
#include <memory>
#include <vector>

namespace openconsult {


/**
 * @brief \c ByteInterface that shims another \c ByteInterface , logging all
 *      transactions invoked on it before forwarding the response.
 *
 * The generated log may be subsequently passed to a \c LogReplay to replay the
 * transactions. Logs may be concatenated.
 */
class LogRecorder : public ByteInterface {
public:
    /**
     * @brief Construct a new \c LogRecorder .
     *
     * @param snooped Interface whose transactions are to be logged.
     * @param output_stream Stream to write the log to.
     */
    LogRecorder(std::unique_ptr<ByteInterface> snooped, std::ostream& output_stream);

    // LogRecorder is not copyable.
    LogRecorder(const LogRecorder&) = delete;
    LogRecorder& operator=(const LogRecorder&) = delete;
    // LogRecorder is movable.
    LogRecorder(LogRecorder&&);
    LogRecorder& operator=(LogRecorder&&);

    /**
     * @brief Destroy the \c LogRecorder , closing the output stream and
     *      releasing the underlying snooped interface.
     */
    ~LogRecorder();

    /**
     * @copydoc ByteInterface::read(std::size_t)
     */
    virtual std::vector<uint8_t> read(std::size_t size = 0) override;

    /**
     * @copydoc ByteInterface::write(std::vector<uint8_t>)
     */
    virtual void write(const std::vector<uint8_t>& bytes) override;

private:
    class impl;
    std::unique_ptr<impl> pimpl;
};


}

#endif