#ifndef OPENCONSULT_LIB_BYTE_INTERFACE
#define OPENCONSULT_LIB_BYTE_INTERFACE

#include <cstdint>
#include <string>
#include <vector>

namespace openconsult {


/**
 * @brief Basic interface for communicating on a generic bytewise interface.
 */
class ByteInterface
{
public:
    virtual ~ByteInterface() = default;

    /**
     * @brief Performs a blocking read of data from the interface.
     *
     * @param size Number of bytes to read. The read will block if fewer bytes
     *      than this are available to read. May be zero to read all currently
     *      available bytes (which may be zero).
     * @return Vector containing the read bytes.
     */
    virtual std::vector<uint8_t> read(std::size_t size = 0) = 0;

    /**
     * @brief Writes data to the interface.
     *
     * @param bytes Vector containing zero or more bytes to write.
     */
    virtual void write(const std::vector<uint8_t>& bytes) = 0;
};


}

#endif