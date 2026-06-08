#ifndef SERIAL_TRANSPORT
#define SERIAL_TRANSPORT

#include <cstdio>
#include <cinttypes>
#include <span>
#include <memory>
#include <string>

extern "C" {
    #include "libserialport.h"
}

#include "Types.h"

class ISerialTransport {
public:
    virtual ~ISerialTransport() = default;

    virtual bool open(const SerialConfig config) = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;

    virtual int read(std::span<std::byte> dst, int timeoutMs) = 0;
    virtual int write(std::span<const std::byte> src, int timeoutMs) = 0;

    virtual void setDtr(bool enabled) = 0;
    virtual void setRts(bool enabled) = 0;
};

class LibSerialTransport final : public ISerialTransport {
public: 
    bool open(const SerialConfig config) override;
    void close() override;
    bool isOpen() const override;

    int read(std::span<std::byte> dst, int timeoutMs) override;
    int write(std::span<const std::byte> src, int timeoutMs) override;

    void setDtr(bool enabled) override;
    void setRts(bool enabled) override;

private:
    sp_port* rawPort_ = nullptr;
    bool open_ = false;
};

#endif // SERIAL_TRANSPORT
