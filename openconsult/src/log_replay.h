#ifndef OPENCONSULT_LIB_LOG_REPLAY
#define OPENCONSULT_LIB_LOG_REPLAY

#include <istream>
#include <memory>
#include "byte_interface.h"

/**
 * @brief \c ByteInterface that reads from a log file and replays communications
 * described within it.
 */
class LogReplay : public ByteInterface
{
public:
    /**
     * @brief Construct a new \c LogReplay .
     *
     * @param input_stream Stream to read the log from.
     */
    LogReplay(std::istream& input_stream);

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
    virtual void write(std::vector<uint8_t> bytes) override;

private:
    class impl;
    std::unique_ptr<impl> pimpl;
};

#endif