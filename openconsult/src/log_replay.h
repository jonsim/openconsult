#ifndef OPENCONSULT_LIB_LOG_REPLAY
#define OPENCONSULT_LIB_LOG_REPLAY

#include "byte_interface.h"

#include <istream>
#include <memory>

namespace openconsult {


/**
 * @brief \c ByteInterface that reads from a log file and replays communications
 * described within it.
 */
class LogReplay : public ByteInterface {
public:
    /**
     * @brief Construct a new \c LogReplay .
     *
     * @param input_stream Stream to read the log from.
     * @param wrap \c true to cause the replayed data to wrap when reaching the
     *      end, \c false to raise \c std::runtime_error from the \c read(...)
     *      and \c write(...) methods when attempting to interact with it after
     *      their respective logged data has been depleted.
     */
    LogReplay(std::istream& input_stream, bool wrap = false);

    /**
     * @brief Destroy the \c LogReplay , closing the input stream.
     */
    virtual ~LogReplay();

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