#ifndef SERIAL_CONNECTION_H
#define SERIAL_CONNECTION_H

#include "Types.h"
#include "BlockingQueue.h"
#include "SerialWorker.h"

struct SerialQueues {
    CommandQueue commandQueue_;
    EventQueue eventQueue_;
};

class SerialConnection : private SerialQueues {
public:
    SerialConnection(const SerialConnection&) = delete;
    SerialConnection& operator=(const SerialConnection&) = delete;
    SerialConnection(SerialConnection&&) = delete;
    SerialConnection& operator=(SerialConnection&&) = delete;

    explicit SerialConnection(uint32_t portId) :
        portId_(portId),
        worker_(portId, commandQueue_, eventQueue_) {}

private:
    uint32_t portId_;

    SerialWorker worker_;
};

#endif // SERIAL_CONNECTION_H
