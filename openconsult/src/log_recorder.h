#ifndef OPENCONSULT_LIB_LOG_RECORDER
#define OPENCONSULT_LIB_LOG_RECORDER

#include <ostream>
#include <memory>
#include <vector>

namespace openconsult {


/**
 * @brief Class that records read and write transactions to a log. The generated
 *      log may be subsequently passed to a \c LogReplay to replay the
 *      transactions. Logs may be concatenated.
 */
class LogRecorder
{
public:
    /**
     * @brief Construct a new \c LogRecorder .
     *
     * @param output_stream Stream to write the log to.
     */
    LogRecorder(std::ostream& output_stream);

    /**
     * @brief Destroy the \c LogRecorder , closing the output stream.
     */
    ~LogRecorder();

    /**
     * @brief Records a read transaction to the log.
     *
     * @param bytes Transaction to record.
     */
    void read(const std::vector<uint8_t>& bytes);

    /**
     * @brief Records a write transaction to the log.
     *
     * @param bytes Transaction to record.
     */
    void write(const std::vector<uint8_t>& bytes);

private:
    class impl;
    std::unique_ptr<impl> pimpl;
};


}

#endif