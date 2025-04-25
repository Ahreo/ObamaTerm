#ifndef PORT_MANAGER
#define PORT_MANAGER

#include <unordered_map>
#include <memory>
#include <vector>

#include "SerialPort.h"

namespace OT {
class PortManager {
    public:
        // Accessor for PortManager
        static PortManager* get_instance();

        // Delete copy/move ctors to enfore singleton
        PortManager(const PortManager&) = delete;
        PortManager& operator=(const PortManager&) = delete;

        // Methods
        SerialPort* get_port(const std::string& port_name) {
            auto it = m_port_map.find(port_name);
            if(it != m_port_map.end()) return it->second.get();
            else return nullptr;
        }
        const std::vector<std::string>& get_portname_vec() { return m_portname_vec; }

    private:
        // Private ctor
        PortManager();
        ~PortManager();
        static PortManager* m_instance;

        // Repopulates the m_port_vec with any new ports
        // and removes old ones
        bool update_ports();

        std::unordered_map<std::string, std::unique_ptr<SerialPort>> m_port_map;
        std::vector<std::string> m_portname_vec;
};
}

#endif // PORT_MANAGER
