#ifndef SERIAL_WORKER_H
#define SERIAL_WORKER_H

#include "BlockingQueue.h"
#include "Types.h"
#include "SerialTransport.h"

class SerialWorker {
public:
    SerialWorker(uint32_t portId,
        BlockingQueue<SerialCommand>& commands,
        BlockingQueue<SerialEvent>& events);

    void start();
    void requestStop();
    void rejoin();

private:
    void threadMain();
    void processCommand(const SerialCommand& cmd);
    void emitEvent(SerialEvent ev);

    uint32_t portId_;
    BlockingQueue<SerialCommand>& commands_;
    BlockingQueue<SerialEvent>& events_;

    std::unique_ptr<ISerialTransport> transport_;
    std::thread thread_;
    std::atomic<bool> stopRequested_ = false;

    SerialConfig config_;
};

#endif // SERIAL_WORKER_H
