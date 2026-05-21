#ifndef SERIAL
#define SERIAL

#include <cstdio>
#include <cinttypes>
#include <stdexcept>
#include <vector> 
#include <memory>
#include <string>
#include <unordered_map>

extern "C" {
    #include "libserialport.h"
}

namespace OT {
enum baud : uint32_t {
    B_9600 = 9600, 
    B_115200 = 115200, 
    B_230400 = 230400, 
    B_460800 = 460800, 
    B_921600 = 921600
};

class SerialPort {
    public:
        SerialPort(struct sp_port* raw_port);
        ~SerialPort() = default;

        // default move constructor - ok because we use the custom deleter
        SerialPort(SerialPort&&) noexcept = default;

        int send_data(std::vector<uint8_t> buffer);
        std::vector<uint8_t> receive_data();

    private:
        struct sp_port* raw_port;
};
}

#endif // SERIAL
