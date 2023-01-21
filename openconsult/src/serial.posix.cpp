#include "serial.h"
#include "common.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <string>

namespace openconsult {


struct SerialPort::impl {
    int port_fd;
};

speed_t baudRateToSpeed(uint32_t baud_rate) {
    switch (baud_rate) {
        case       0:       return B0;
        case      50:      return B50;
        case      75:      return B75;
        case     110:     return B110;
        case     134:     return B134;
        case     150:     return B150;
        case     200:     return B200;
        case     300:     return B300;
        case     600:     return B600;
        case    1200:    return B1200;
        case    1800:    return B1800;
        case    2400:    return B2400;
        case    4800:    return B4800;
        case    9600:    return B9600;
        case   19200:   return B19200;
        case   38400:   return B38400;
        case   57600:   return B57600;
        case  115200:  return B115200;
        case  230400:  return B230400;
        case  460800:  return B460800;
        case  500000:  return B500000;
        case  576000:  return B576000;
        case  921600:  return B921600;
        case 1000000: return B1000000;
        case 1152000: return B1152000;
        case 1500000: return B1500000;
        case 2000000: return B2000000;
        default:

            std::string error = cmn::pformat("Unsupported baud rate %u", baud_rate);
            throw os_error(error);
    };
}

SerialPort::SerialPort(const std::string& device, uint32_t baud_rate)
        : pimpl(new impl) {
    // Open the port.
    int fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        std::string error = cmn::pformat("Failed to open %s: %s", device, strerror(errno));
        throw os_error(error);
    }

    // Configure the port.
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        std::string error = cmn::pformat("Failed to query device: %s", strerror(errno));
        throw os_error(error);
    }

    speed_t speed = baudRateToSpeed(baud_rate);
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    // Standard 8N1 configuration parameters.
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // Set character size to 8-bits.
    tty.c_cflag |= (CLOCAL | CREAD);            // Enable reading and local mode (no modem).
    tty.c_cflag &= ~(PARENB | PARODD);          // Disable parity generation and checking.
    tty.c_cflag &= ~CSTOPB;                     // Disable double-stop-bit behaviour.
    tty.c_cflag &= ~CRTSCTS;                    // Disable RTS/CTS flow control. Non posix.
    tty.c_lflag = 0;                            // Disable all echoing and signal generation. Also
                                                // ensure non-canonical mode (so not line-based).
    tty.c_iflag &= ~IGNBRK;                     // Ignore BREAK condition on input.
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // Disable XON/XOFF flow control.
    tty.c_oflag = 0;                            // Disable all remapping and delays.
    tty.c_cc[VMIN] = 1;                         // Set VMIN to a non-zero value to enable blocking.
    tty.c_cc[VTIME] = 0;                        // Disable read timeouts.

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::string error = cmn::pformat("Failed to configure device: %s", strerror(errno));
        throw os_error(error);
    }

    // Assign to the pimpl.
    pimpl->port_fd = fd;
}

SerialPort::~SerialPort() {
    close(pimpl->port_fd);
}

std::vector<uint8_t> SerialPort::read(std::size_t size) {
    std::vector<uint8_t> buff(size);
    std::size_t total_bytes_read = 0;
    while (total_bytes_read < size) {
        int bytes_read = ::read(pimpl->port_fd,
                buff.data() + total_bytes_read,
                size - total_bytes_read);
        if (bytes_read < 0) {
            std::string error = cmn::pformat("Failed to read from serial port: %s", strerror(errno));
            throw os_error(error);
        }
        total_bytes_read += static_cast<std::size_t>(bytes_read);
    }
    return buff;
}

void SerialPort::write(const std::vector<uint8_t>& bytes) {
    std::size_t total_bytes_written = 0;
    while (total_bytes_written < bytes.size()) {
        ssize_t bytes_written = ::write(pimpl->port_fd,
                bytes.data() + total_bytes_written,
                bytes.size() - total_bytes_written);
        if (bytes_written < 0) {
            std::string error = cmn::pformat("Failed to write to serial port: %s", strerror(errno));
            throw os_error(error);
        }
        total_bytes_written += static_cast<std::size_t>(bytes_written);
    }
}


}
