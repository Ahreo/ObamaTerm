#ifndef SERIAL_CONFIG_H
#define SERIAL_CONFIG_H

#include <cinttypes>

enum class Baud {
    _9600 = 9600, 
    _115200 = 115200, 
    _230400 = 230400, 
};

enum class DataBits {
    _8 = 8
};

enum class Parity {
    None,
    Odd,
    Even,
    Mark,
    Space
};

enum class StopBits { 
    One,
    OnePointFive,
    Two
};

enum class FlowControl {
    None,
    SoftwareXonXoff,
    HardwareRtsCts,
    HardwareDtrDsr,
};

struct SerialConfig { 
    std::string portName;
    Baud baudRate = Baud::_115200;
    DataBits dataBits = DataBits::_8;
    Parity parity = Parity::None;
    StopBits stopBits = StopBits::One;
    FlowControl flowControl = FlowControl::None;

    int readTimeoutMs = 20;
    size_t readBufSize = 4096;
};

#endif // SERIAL_CONFIG_H
