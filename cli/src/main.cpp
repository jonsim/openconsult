#include "openconsult/src/consult_interface.h"
#include "openconsult/src/serial.h"

using namespace openconsult;


int main(int argc, char** argv) {
    std::unique_ptr<SerialPort> serial_port(new SerialPort("/dev/ttyUSB0", 9600));

    ConsultInterface consult(std::move(serial_port));

    return 0;
}
