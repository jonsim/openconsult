#ifndef OPENCONSULT_LIB_SERIAL
#define OPENCONSULT_LIB_SERIAL

#include "byte_interface.h"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace openconsult {


/**
 * @brief Exception type thrown to indicate an unhandled error coming from an
 * Operating System call.
 */
class os_error : public std::runtime_error {
public:
    os_error(const char* message) :
        std::runtime_error(message) {
    }

    os_error(const std::string& message) :
        std::runtime_error(message) {
    }
};

/**
 * @brief Basic RAII interface for communicating with a serial port in a
 *      platform-agnostic manner.
 */
class SerialPort : public ByteInterface {
public:
    /**
     * @brief Construct a new \c SerialPort . The serial port is configured for
     *      standard 8N1 communications. This is a RAII interface: the
     *      connection is established at construction and released at
     *      destruction.
     *
     * @param device Identifier of the serial device to communicate with. The
     *      exact representation or meaning of this string is platform-specific.
     * @param baud_rate Baud rate to use for the connection. The permitted
     *      values are platform-specific.
     *
     * @throws os_error if the device cannot be opened or configured as
     *      requested.
     */
    SerialPort(const std::string& device, uint32_t baud_rate);

    /**
     * @brief Destroy the \c SerialPort object, closing the connection.
     */
    virtual ~SerialPort();

    /**
     * @copydoc ByteInterface::read(std::size_t)
     *
     * @throws os_error is the read fails unexpectedly.
     */
    virtual std::vector<uint8_t> read(std::size_t size = 0) override;

    /**
     * @copydoc ByteInterface::write(std::vector<uint8_t>)
     *
     * @throws os_error is the write fails unexpectedly.
     */
    virtual void write(const std::vector<uint8_t>& bytes) override;

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};


}

#endif