#include <Windows.h>
#include <string>
#include <system_error>
#include "common.h"
#include "serial.h"

struct SerialPort::impl {
    HANDLE port_handle;
};

std::string last_error() {
   DWORD error = GetLastError();
   return std::system_category().message(error);
}

SerialPort::SerialPort(const std::string& device, uint32_t baud_rate)
        : pimpl(new impl) {
    // Open the port.
    HANDLE handle = CreateFileA(static_cast<LPCSTR>(device.c_str()),
            GENERIC_READ | GENERIC_WRITE,
            0,                          // No sharing of the underlying file.
            NULL,                       // No security descriptor (the handle will not be shared).
            OPEN_EXISTING,              // The device must exist.
            FILE_ATTRIBUTE_NORMAL,      // No special file type.
            NULL);                      // No template file.
	if (handle == INVALID_HANDLE_VALUE) {
        std::string error = cmn::pformat("Failed to open %s: %s", device, last_error());
        throw os_error(error);
	}

    // Configure the port.
    DCB params = {0};
    if (!GetCommState(handle, &params)) {
        std::string error = cmn::pformat("Failed to query device: %s", last_error());
        throw os_error(error);
    }

    // Standard 8N1 configuration parameters.
    params.BaudRate = baud_rate;
    params.ByteSize = 8;
    params.StopBits = ONESTOPBIT;
    params.Parity = NOPARITY;
    params.fDtrControl = DTR_CONTROL_ENABLE;    // Enable DTR flow control.

    if (!SetCommState(handle, &params))
        std::string error = cmn::pformat("Failed to configure device: %s", last_error());
        throw os_error(error);
    }
    PurgeComm(handle, PURGE_RXCLEAR | PURGE_TXCLEAR);

    // Assign to the impl.
    pimpl->port_handle = handle;
}

SerialPort::~SerialPort() {
    CloseHandle(pimpl->port_handle);
}

std::vector<uint8_t> SerialPort::read(std::size_t size) {
    std::vector<uint8_t> buff(size);
    std::size_t total_bytes_read = 0;
    while (total_bytes_read < size) {
        std::size_t bytes_read = 0;
        bool success = ReadFile(pimpl->port_handle,
                buff.data() + total_bytes_read,
                size - total_bytes_read,
                &bytes_read, NULL);
        if (!success) {
            std::string error = cmn::pformat("Failed to read from serial port: %s", last_error());
            throw os_error(error);
        }
        total_bytes_read += bytes_read;
    }
    return buff;
}

void SerialPort::write(const std::vector<uint8_t>& bytes) {
    std::size_t total_bytes_written = 0;
    while (total_bytes_written < bytes.size()) {
        std::size_t bytes_written = 0;
        bool success = WriteFile(pimpl->port_handle,
                bytes.data() + total_bytes_written,
                bytes.size() - total_bytes_written,
                &bytes_written, NULL);
        if (!success) {
            std::string error = cmn::pformat("Failed to write to serial port: %s", last_error());
            throw os_error(error);
        }
        total_bytes_written += bytes_written;
    }
}
