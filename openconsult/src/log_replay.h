#ifndef OPENCONSULT_LIB_LOG_REPLAY
#define OPENCONSULT_LIB_LOG_REPLAY

#include <memory>
#include "byte_interface.h"

class LogReplay : public ByteInterface
{
public:
    LogReplay(const std::string& log_path);

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