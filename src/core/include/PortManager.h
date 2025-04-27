#ifndef PORT_MANAGER
#define PORT_MANAGER

#include <memory>
#include <vector>
#include <forward_list>
#include <utility>

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
            for(const auto& pair : m_port_list) {
                if(pair.first == port_name) {
                    return pair.second.get();
                }
            }

            return nullptr;
        }
        const std::vector<std::string>& get_portname_vec() { return m_portname_vec; }

    private:
        // Private ctor
        PortManager();
        ~PortManager();
        static PortManager* m_instance;

        // Repopulates the m_port_vec with any new ports
        // and removes old ones
        void update_ports();

        // Called internally to add ports to list
        void add_ports();

        std::forward_list<std::pair<std::string, std::unique_ptr<SerialPort>>> m_port_list;
        std::vector<std::string> m_portname_vec;
};
}

#endif // PORT_MANAGER
