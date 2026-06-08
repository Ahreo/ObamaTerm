#include "PortManager.h"

extern "C" {
    #include "libserialport.h"
}

void PortManager::refreshPortList() {
    availablePorts_.clear();

    sp_port** portList = nullptr;
    if (sp_list_ports(&portList) != SP_OK)
        return;

    for (int i = 0; portList[i] != nullptr; i++) {
        PortInfo info;

        const char* name = sp_get_port_name(portList[i]);
        info.portName = name ? name : "";

        const char* desc = sp_get_port_description(portList[i]);
        info.description = desc ? desc : "";

        switch (sp_get_port_transport(portList[i])) {
            case SP_TRANSPORT_USB:       info.transport = "usb";       break;
            case SP_TRANSPORT_BLUETOOTH: info.transport = "bluetooth"; break;
            default:                     info.transport = "native";    break;
        }

        availablePorts_.push_back(std::move(info));
    }

    sp_free_port_list(portList);
}

