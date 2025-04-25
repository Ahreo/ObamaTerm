#include "SerialPort.h"
#include <iostream>
#include <sstream>

namespace OT {
    SerialPort::SerialPort(struct sp_port *port) : m_port(port) {
        if(port == nullptr) {
            throw std::invalid_argument("SerialPort Constructor: Port cannot be null");
        }

        m_port_name = sp_get_port_name(m_port);
        m_port_desc = sp_get_port_description(m_port);
        if(!resolve_port_transport()) {
            std::cerr << "Failed to resolve port transport protocol\n";
        }
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
            oss << "Manufacturer: " << sp_get_port_usb_manufacturer(m_port) << "\n";
            oss << "Product: " << sp_get_port_usb_product(m_port) << "\n";
            oss << "Serial: " << sp_get_port_usb_serial(m_port) << "\n";

            /* Display USB vendor and product IDs. */
            int usb_vid, usb_pid;
            sp_get_port_usb_vid_pid(m_port, &usb_vid, &usb_pid);
            oss << "VID: " << usb_vid << " PID: " << usb_pid << "\n";

            /* Display bus and address. */
            int usb_bus, usb_address;
            sp_get_port_usb_bus_address(m_port, &usb_bus, &usb_address);
            oss << "Bus: " << usb_bus << " Address: " << usb_address << "\n";
            m_port_transport += oss.str();
        }
        else if (transport == SP_TRANSPORT_BLUETOOTH) {
            /* This is a Bluetooth serial port. */
            oss << "Type: Bluetooth\n";

            /* Display Bluetooth MAC address. */
            oss << "MAC: " << sp_get_port_bluetooth_address(m_port) << "\n";
            m_port_transport += oss.str();
        }
        else {
            return false;
        }

        // successfully identified transport
        return true;
    }

    int SerialPort::send_data() {
        return 0;
    }

    int SerialPort::receive_data() {
        // /* Now send some data on each port and receive it back. */
        // /* Get the ports to send and receive on. */
        // // struct sp_port *rx_port = port;

        // /* The data we will send. */
        // constexpr int size = 64;
        // while(1) {
        //     /* We'll allow a 1 second timeout for send and receive. */
        //     unsigned int timeout = 10;

        //     /* On success, sp_blocking_write() and sp_blocking_read()
        //         * return the number of bytes sent/received before the
        //         * timeout expired. We'll store that result here. */
        //     int result;

        //     /* Allocate a buffer to receive data. */
        //     char buf[size];

        //     /* Try to receive the data on the other port. */
        //     result = sp_blocking_read(rx_port, buf, size, timeout);

        //     /* Check whether we received the number of bytes we wanted. */
        //     for(int i=0;i<result;++i) {
        //         printf("%c", buf[i]);
        //     }
        // }
        return 0;
    }
}
