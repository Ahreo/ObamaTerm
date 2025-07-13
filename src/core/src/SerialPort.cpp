#include "SerialPort.h"
#include <iostream>
#include <sstream>

namespace OT {
SerialPort::SerialPort(sp_port* raw_port) : m_port(raw_port, sp_free_port) {
    bool success;
    while(true) {
        char* result;
        if(raw_port == nullptr) {
            fprintf(stderr, "SerialPort Constructor: Port cannot be null\n");
            success = false;
            break;
        }

        result = sp_get_port_name(m_port.get()); 
        if(result == nullptr) {
            fprintf(stderr, "Failed to get port name\n");
            success = false;
            break;
        }
        else {
            m_port_name = result;
        }

        result = sp_get_port_description(m_port.get());
        if(result == nullptr) {
            fprintf(stderr, "Failed to get port description\n");
            success = false;
            break;
        }
        else {
            m_port_desc = result;
        }

        if(!resolve_port_transport()) {
            fprintf(stderr, "Failed to resolve port transport protocol\n");
            success = false;
            break;
        }

        success = true;
        break;
    }

    m_initialized = success;
    printf("Ctor for %s finished\n\n", m_port_name.c_str());
}

bool SerialPort::resolve_port_transport() {
    // Used to append to formatted string
    std::ostringstream oss;

    // Result to check for null values
    char* result;

    /* Identify the transport which this port is connected through,
    * e.g. native port, USB or Bluetooth. */
    enum sp_transport transport = sp_get_port_transport(m_port.get());

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
            result = sp_get_port_usb_manufacturer(m_port.get());
            if(result == nullptr) {
                fprintf(stderr, "Failed to usb manufacturer\n");
                return false;
            }
            else {
                oss << "Manufacturer: " << result << "\n";
            }

            // try getting usb product
            result = sp_get_port_usb_product(m_port.get());
            if (result == nullptr) {
                fprintf(stderr, "Failed to get usb product\n");
                return false;
            }
            else {
                oss << "Product: " << result << "\n";
            }

            // try getting usb serial
            result = sp_get_port_usb_serial(m_port.get());
            if(result == nullptr) {
                fprintf(stderr, "Failed to get usb serial\n");
                return false;
            }
            else {
                oss << "Serial: " << result << "\n";
            }

            /* Display USB vendor and product IDs. */
            int usb_vid, usb_pid;
            sp_get_port_usb_vid_pid(m_port.get(), &usb_vid, &usb_pid);
            oss << "VID: " << usb_vid << " PID: " << usb_pid << "\n";

            /* Display bus and address. */
            int usb_bus, usb_address;
            sp_get_port_usb_bus_address(m_port.get(), &usb_bus, &usb_address);
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
            result = sp_get_port_bluetooth_address(m_port.get());
            if(result == nullptr) {
                fprintf(stderr, "Failed to get Bluetooth MAC address\n");
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
    int result = sp_blocking_write(m_port.get(), buffer.data(), buffer.size(), m_timeout);
    return result;
}

std::vector<uint8_t> SerialPort::receive_data() {
    if(m_initialized) {
        std::vector<uint8_t> buffer;
        buffer.reserve(1024);
        sp_return result = sp_nonblocking_read(m_port.get(), buffer.data(), buffer.size());
        if(result > 0) {
            return buffer;
        }
    }
    return {};
}
}
