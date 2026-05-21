#include "SerialPort.h"
#include <iostream>
#include <sstream>

namespace OT {
SerialPort::SerialPort(struct sp_port* raw_port) {
}


int SerialPort::send_data(std::vector<uint8_t> buffer) {
    return 0;
}

std::vector<uint8_t> SerialPort::receive_data() {
    return {};
}
}
