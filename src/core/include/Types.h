#ifndef TYPES_H
#define TYPES_H

#include <cstddef>
#include <chrono>
#include <vector>
#include <cinttypes>
#include <string>
#include "SerialConfig.h"

enum class SerialCommandKind {
    Open,
    Close,
    WriteBytes,
    SetDtr,
    SetRts,
    SendBreak,
    Reconfigure,
    Shutdown
};

struct SerialCommand {
    SerialCommandKind kind;
    SerialConfig config;
    std::vector<std::byte> payload;
};

enum class SerialEventKind {
    PortOpened,
    PortClosed,
    RxBytes,
    TxBytes,
    Error,
    Warning,
    LineStateChanged,
    ConfigChanged
};

struct SerialEvent {
    uint64_t seqNum = 0;
    uint32_t portId = 0;
    SerialEventKind kind;

    // for calculating time elapsed
    std::chrono::steady_clock::time_point monotonicTime;
    // for timestamping when things happened
    std::chrono::system_clock::time_point wallTime;

    std::vector<std::byte> payload;
    std::string message;
};

struct PortInfo {
    std::string portName;
    std::string description;
    std::string transport; // usb, bluetooth, etc
};

#endif // TYPES_H
