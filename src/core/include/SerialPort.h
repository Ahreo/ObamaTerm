#ifndef SERIAL
#define SERIAL

#include <cstdio>
#include <cinttypes>
#include <stdexcept>

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
        SerialPort(struct sp_port *port);
        ~SerialPort();

        SerialPort(SerialPort&&) noexcept = default;

        bool resolve_port_transport();
        int send_data();
        int receive_data();
        std::string get_name() { return m_port_name; }
        std::string get_desc() { return m_port_desc; }
        std::string get_trans_protocol() { return m_port_transport; }

    private:
        struct sp_port *m_port;
        std::string m_port_name;
        std::string m_port_desc;
        std::string m_port_transport;
};
}

#endif // SERIAL
