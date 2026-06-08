#ifndef PORT_MANAGER_H
#define PORT_MANAGER_H

#include "Types.h"
#include <vector>

class PortManager {
public:
    void refreshPortList();

    const std::vector<PortInfo>& availablePorts() const { return availablePorts_; }

private:
    std::vector<PortInfo> availablePorts_;
};

#endif // PORT_MANAGER_H
