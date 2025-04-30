#ifndef SERIAL
#define SERIAL

#include <cstdio>
#include <cinttypes>
#include <stdexcept>
#include <vector> 
#include <memory>

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
        SerialPort(sp_port* raw_port);
        ~SerialPort();

        // default move constructor - ok because we use the custom deleter
        SerialPort(SerialPort&&) noexcept = default;

        bool resolve_port_transport();
        int send_data(std::vector<uint8_t> buffer);
        std::vector<uint8_t> receive_data();

        std::string get_name() { return m_port_name; }
        std::string get_desc() { return m_port_desc; }
        std::string get_trans_protocol() { return m_port_transport; }

    private:
        std::unique_ptr<sp_port, decltype(&sp_free_port)> m_port;
        std::string m_port_name;
        std::string m_port_desc;
        std::string m_port_transport;

        // timeout is in milliseconds
        unsigned int m_timeout = 10;
        bool m_initialized;
};
}

#endif // SERIAL
