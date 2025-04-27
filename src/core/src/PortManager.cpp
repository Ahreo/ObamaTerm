#include "PortManager.h"
#include "SerialPort.h"
#include <iostream>

namespace OT {
    // Private ctor to populate with initial ports
    PortManager::PortManager() {
        // Search for ports
        struct sp_port **port_list_raw;

        /* Call sp_list_ports() to get the ports. The port_list_raw
        * pointer will be updated to refer to the array created. */
        enum sp_return result = sp_list_ports(&port_list_raw);

        if (result != SP_OK) {
            std::cerr << "PortManager Constructor: failed to get port list\n";
        }

        /* Iterate through the ports. When port_list_raw[i] is NULL
        * this indicates the end of the list. */
        for (int i = 0; port_list_raw[i] != NULL; i++) {
            struct sp_port *port = port_list_raw[i];
            std::string port_name = sp_get_port_name(port);
            sp_get_port_by_name(port_name.c_str(), &port);

            // Opening port
            sp_open(port, SP_MODE_READ_WRITE);

            // Setting port to 115200 8N1, no flow control
            sp_set_baudrate(port, 115200);
            sp_set_bits(port, 8);
            sp_set_parity(port, SP_PARITY_NONE);
            sp_set_stopbits(port, 1);
            sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);

            // Class template argument deduction from C++17
            m_port_list.emplace_front(port_name, std::make_unique<SerialPort>(port));
            m_portname_vec.push_back(port_name);
        }
    }

    PortManager::~PortManager() {
        // because the map uses unique_ptr, clear() 
        // will automatically deallocate the memory!
        m_port_list.clear();
        m_portname_vec.clear();
    }

    void PortManager::update_ports() {
    }

    void PortManager::add_ports() {
        
    }

    PortManager* PortManager::m_instance = nullptr;

    PortManager* PortManager::get_instance()  {
        if(!m_instance) {
            m_instance = new PortManager();
        }
        return m_instance;
    }

}
