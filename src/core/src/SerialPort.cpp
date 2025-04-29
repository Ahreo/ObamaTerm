#include "SerialPort.h"
#include <iostream>
#include <sstream>

namespace OT {
    SerialPort::SerialPort(struct sp_port *port) : m_port(port) {
        bool success;
        while(true) {
            char* result;
            if(port == nullptr) {
                std::cerr << ("SerialPort Constructor: Port cannot be null");
                success = false;
                break;
            }

            result = sp_get_port_name(m_port); 
            if(result == nullptr) {
                std::cerr << ("Failed to get port name");
                success = false;
                break;
            }
            else {
                m_port_name = result;
            }

            result = sp_get_port_description(m_port);
            if(result == nullptr) {
                std::cerr << ("Failed to get port description");
                success = false;
                break;
            }
            else {
                m_port_desc = result;
            }

            if(!resolve_port_transport()) {
                std::cerr << "Failed to resolve port transport protocol\n";
                success = false;
                break;
            }

            success = true;
            break;
        }

        m_initialized = success;
    }

    SerialPort::~SerialPort() {
        if(sp_close(m_port) != SP_OK) {
            std::cerr << "SerialPort Destructor: Port cannot be null\n";
        }

        sp_free_port(m_port);
    }

    bool SerialPort::resolve_port_transport() {
        // Used to append to formatted string
        std::ostringstream oss;

        // Result to check for null values
        char* result;

        /* Identify the transport which this port is connected through,
        * e.g. native port, USB or Bluetooth. */
        enum sp_transport transport = sp_get_port_transport(m_port);

        if (transport == SP_TRANSPORT_NATIVE) {
            /* This is a "native" port, usually directly connected
            * to the system rather than some external interface. */
            m_port_transport += "Type: Native\n";
        }
        else if (transport == SP_TRANSPORT_USB) {
            /* This is a USB to serial converter of some kind. */
            /* Display string information from the USB descriptors. */
            oss << "Type: USB\n";
            while(true) {
                // try getting usb manufacturer
                result = sp_get_port_usb_manufacturer(m_port);
                if(result == nullptr) {
                    std::cerr << "Failed to usb manufacturer\n";
                    return false;
                }
                else {
                    oss << "Manufacturer: " << result << "\n";
                }

                // try getting usb product
                result = sp_get_port_usb_product(m_port);
                if (result == nullptr) {
                    std::cerr << "Failed to get usb product\n";
                    return false;
                }
                else {
                    oss << "Product: " << result << "\n";
                }

                // try getting usb serial
                result = sp_get_port_usb_serial(m_port);
                if(result == nullptr) {
                    std::cerr << "Failed to get usb serial\n";
                    return false;
                }
                else {
                    oss << "Serial: " << result << "\n";
                }

                /* Display USB vendor and product IDs. */
                int usb_vid, usb_pid;
                sp_get_port_usb_vid_pid(m_port, &usb_vid, &usb_pid);
                oss << "VID: " << usb_vid << " PID: " << usb_pid << "\n";

                /* Display bus and address. */
                int usb_bus, usb_address;
                sp_get_port_usb_bus_address(m_port, &usb_bus, &usb_address);
                oss << "Bus: " << usb_bus << " Address: " << usb_address << "\n";
                m_port_transport += oss.str();

                // exit flow control
                break;
            }
        }
        else if (transport == SP_TRANSPORT_BLUETOOTH) {
            /* This is a Bluetooth serial port. */
            oss << "Type: Bluetooth\n";

            while(true) {
                /* Display Bluetooth MAC address. */
                result = sp_get_port_bluetooth_address(m_port);
                if(result == nullptr) {
                    std::cerr << "Failed to get Bluetooth MAC address\n";
                    return false;
                }
                else {
                    oss << "MAC: " << result << "\n";
                }
                
                m_port_transport += oss.str();

                // exit flow control
                break;
            }
        }
        else {
            return false;
        }

        // successfully identified transport
        return true;
    }

    int SerialPort::send_data(std::vector<uint8_t> buffer) {
		int result = sp_blocking_write(m_port, buffer.data(), buffer.size(), m_timeout);
        return result;
    }

    std::vector<uint8_t> SerialPort::receive_data() {
        std::vector<uint8_t> buffer(1024);
        int result = sp_blocking_read(m_port, buffer.data(), buffer.size(), m_timeout);

        if(result > 0) {
            return buffer;
        }
        else {
            return {};
        }
    }
}
