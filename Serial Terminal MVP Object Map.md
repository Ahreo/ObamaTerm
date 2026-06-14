5/14/2026

# Serial Terminal MVP Object Map

## Scope

This MVP targets a C++20 serial terminal/debugging tool with:
- Dear ImGui docking UI
- `libserialport` backend
- One `SerialWorker` thread per open serial port
- Command queue into the worker
- Event queue out of the worker
- Append-only RAM `SessionStore`
- Terminal-style log view
- Hex view
- Send editor
- Manual compatibility testing with `com0com`

Out of scope for the MVP:
- File-backed live scrollback
- Full VT100/xterm terminal emulation
- Macro/trigger engine
- Protocol decoders such as Modbus
- Script engine
- Native virtual COM/null-modem driver
- Advanced com0com management UI

---
## Big Picture

```text
Application
├─ AppConfig
├─ PortManager
│  └─ SerialConnection[]
│     ├─ SerialWorker
│     ├─ CommandQueue
│     ├─ EventQueue
│     └─ SerialConfig
│
├─ SessionStore
│  ├─ SerialEvent[]
│  └─ byte budget / retention policy
│
├─ UI
│  ├─ MainDockspace
│  ├─ PortPanel
│  ├─ TerminalLogView
│  ├─ HexView
│  ├─ SendPanel
│  └─ StatusBar
│
└─ Optional later
   ├─ TextLogWriter
   └─ SettingsSerializer
```

Core rule:

```text
SerialWorker captures facts.
SessionStore records facts.
Views interpret facts.
UI edits future commands.
```

---

## Ownership Model

|Object|Owns|Does not own|
|---|---|---|
|`Application`|Top-level app state, UI panels, `PortManager`, `SessionStore`|Serial port handles directly|
|`PortManager`|Available port list, active `SerialConnection` objects|Session history|
|`SerialConnection`|One connection facade, command/event queues, worker lifecycle|UI rendering, session history|
|`SerialWorker`|Worker thread behavior and serial transport access|UI state, session history|
|`LibSerialPortTransport`|`libserialport` handle/wrapper|Threading policy, UI, app logic|
|`SessionStore`|Recent event history|Serial port handle, UI widgets|
|`TerminalLogView`|Terminal/log rendering state|Raw serial ownership|
|`HexView`|Hex rendering state|Raw serial ownership|
|`SendPanel`|Mutable pre-send editor state|Session history, serial handle|

Important thread ownership rule:

```text
Only the SerialWorker touches the serial port.
Only the UI/session thread mutates SessionStore and view state.
Queues transfer immutable commands/events between them.
```

---

## Core Data Types

### `SerialConfig`

Configuration used to open or reconfigure a port.

```cpp
struct SerialConfig {
    std::string port_name;   // "COM7", "/dev/ttyUSB0", etc.
    int baud_rate = 115200;
    int data_bits = 8;
    Parity parity = Parity::None;
    StopBits stop_bits = StopBits::One;
    FlowControl flow_control = FlowControl::None;

    int read_timeout_ms = 20;
    size_t read_buffer_size = 4096;
};
```

Related enums:

```cpp
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
    HardwareDtrDsr
};
```

---

### `SerialCommand`

Commands sent from the UI/main thread into the serial worker.

```cpp
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
    bool bool_value = false;
};
```

Examples:

```text
Open COM7
Write bytes 41 54 0D 0A
Set DTR true
Close port
Shutdown worker
```

---

### `SerialEvent`

Facts emitted by the worker and appended to the session.

```cpp
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

using PortId = uint32_t;

struct SerialEvent {
    uint64_t sequence = 0;
    SerialEventKind kind;
    PortId port_id = 0;

    std::chrono::steady_clock::time_point monotonic_time;
    std::chrono::system_clock::time_point wall_time;

    std::vector<std::byte> payload;
    std::string message;
};
```

For the MVP, `std::vector<std::byte>` payloads are acceptable. Later, this can be replaced with a byte arena or file-backed payload store.

---

### `PortInfo`

Metadata for discovered serial ports.

```cpp
struct PortInfo {
    std::string port_name;      // e.g. "COM7"
    std::string description;    // e.g. "USB-SERIAL CH340"
    std::string transport;      // e.g. "usb", "bluetooth", "native"
};
```

---

## Transport Layer

### `ISerialTransport`

Your app-owned serial interface. This prevents `libserialport` from leaking through the rest of the codebase.

```cpp
class ISerialTransport {
public:
    virtual ~ISerialTransport() = default;

    virtual bool open(const SerialConfig& config) = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;

    virtual int read(std::span<std::byte> dst, int timeout_ms) = 0;
    virtual int write(std::span<const std::byte> src, int timeout_ms) = 0;

    virtual void setDtr(bool enabled) = 0;
    virtual void setRts(bool enabled) = 0;
};
```

Design notes:

- `read()` should return the actual number of bytes read.
- `write()` should return the actual number of bytes accepted/written.
- Callers must handle partial writes.
- Transport should not know about ImGui, sessions, logs, or views.

---

### `LibSerialPortTransport`

Concrete `libserialport` implementation.

```cpp
class LibSerialPortTransport final : public ISerialTransport {
public:
    bool open(const SerialConfig& config) override;
    void close() override;
    bool isOpen() const override;

    int read(std::span<std::byte> dst, int timeout_ms) override;
    int write(std::span<const std::byte> src, int timeout_ms) override;

    void setDtr(bool enabled) override;
    void setRts(bool enabled) override;

private:
    sp_port* port_ = nullptr;
    bool open_ = false;
};
```

This class should use RAII to safely close/free the `sp_port*` handle.

---

## Queue Layer

### `BlockingQueue<T>`

General thread-safe FIFO queue used for commands and events.

```cpp
template <typename T>
class BlockingQueue {
public:
    void push(T item);
    bool try_pop(T& out);
    bool wait_pop_for(T& out, std::chrono::milliseconds timeout);

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<T> queue_;
};
```

Aliases:

```cpp
using CommandQueue = BlockingQueue<SerialCommand>;
using EventQueue = BlockingQueue<SerialEvent>;
```

Important: this is a FIFO queue. If events are pushed as A, B, C, they are popped as A, B, C.

---

## Connection Layer

### `SerialWorker`

Background executor that owns the serial transport and runs the I/O loop.

```cpp
class SerialWorker {
public:
    SerialWorker(PortId port_id,
                 CommandQueue& commands,
                 EventQueue& events);

    void start();
    void requestStop();
    void join();

private:
    void threadMain();
    void processCommand(const SerialCommand& cmd);
    void emitEvent(SerialEvent ev);

    PortId port_id_;
    CommandQueue& commands_;
    EventQueue& events_;

    std::unique_ptr<ISerialTransport> transport_;
    std::thread thread_;
    std::atomic<bool> stop_requested_ = false;

    SerialConfig config_;
};
```

The worker should:

- own the serial port handle indirectly through `ISerialTransport`
- process commands from `CommandQueue`
- read bytes from the port
- emit `RxBytes`, `TxBytes`, and error events into `EventQueue`
- avoid UI work, formatting, heavy parsing, or synchronous logging in the read path

Recommended MVP loop:

```cpp
void SerialWorker::threadMain() {
    std::vector<std::byte> read_buf(config_.read_buffer_size);

    while (!stop_requested_) {
        SerialCommand cmd;
        while (commands_.try_pop(cmd)) {
            processCommand(cmd);
        }

        if (!transport_ || !transport_->isOpen()) {
            if (commands_.wait_pop_for(cmd, std::chrono::milliseconds(20))) {
                processCommand(cmd);
            }
            continue;
        }

        int n = transport_->read(
            std::span<std::byte>(read_buf.data(), read_buf.size()),
            config_.read_timeout_ms
        );

        if (n > 0) {
            SerialEvent ev;
            ev.kind = SerialEventKind::RxBytes;
            ev.port_id = port_id_;
            ev.monotonic_time = std::chrono::steady_clock::now();
            ev.wall_time = std::chrono::system_clock::now();
            ev.payload.assign(read_buf.begin(), read_buf.begin() + n);
            events_.push(std::move(ev));
        } else if (n < 0) {
            emitEvent(/* Error event */);
        }
    }
}
```

---

### `SerialConnection`

Public facade for one serial connection. Owns the worker and its queues.

```cpp
class SerialConnection {
public:
    explicit SerialConnection(PortId id)
        : id_(id),
          worker_(id, command_queue_, event_queue_) {}

    void open(const SerialConfig& config);
    void close();
    void send(std::span<const std::byte> bytes);
    void setDtr(bool enabled);
    void setRts(bool enabled);

    bool tryPopEvent(SerialEvent& out);

    PortId id() const { return id_; }

private:
    PortId id_;

    CommandQueue command_queue_;
    EventQueue event_queue_;
    SerialWorker worker_;
};
```

The rest of the app talks to `SerialConnection`, not directly to `SerialWorker`.

Ownership relationship:

```text
SerialConnection owns queues.
SerialWorker holds references to those queues.
```

---

### `PortManager`

Owns available port metadata and active connections.

```cpp
class PortManager {
public:
    void refreshPortList();

    const std::vector<PortInfo>& availablePorts() const;

    SerialConnection& createConnection();
    std::vector<std::unique_ptr<SerialConnection>>& connections();

private:
    std::vector<PortInfo> available_ports_;
    std::vector<std::unique_ptr<SerialConnection>> connections_;
    PortId next_port_id_ = 1;
};
```

---

## Session Storage

### `SessionStore`

Canonical recent event history.

```cpp
class SessionStore {
public:
    void append(SerialEvent ev);

    const std::deque<SerialEvent>& events() const {
        return events_;
    }

    size_t retainedBytes() const {
        return retained_payload_bytes_;
    }

private:
    std::deque<SerialEvent> events_;

    size_t retained_payload_bytes_ = 0;
    size_t max_retained_payload_bytes_ = 64 * 1024 * 1024;
};
```

Append behavior:

```cpp
void SessionStore::append(SerialEvent ev) {
    retained_payload_bytes_ += ev.payload.size();
    events_.push_back(std::move(ev));

    while (retained_payload_bytes_ > max_retained_payload_bytes_) {
        retained_payload_bytes_ -= events_.front().payload.size();
        events_.pop_front();
    }
}
```

For the MVP, this is RAM-only recent scrollback. Old events may be evicted from memory once the byte budget is exceeded.

Later upgrade path:

```text
std::deque<SerialEvent>
→ event metadata ring + byte arena
→ optional binary session file
→ file-backed live scrollback
```

---

## UI Layer

### `PortPanel`

Port selection and configuration UI.

```cpp
class PortPanel {
public:
    void draw(PortManager& ports);

private:
    SerialConfig draft_config_;
};
```

Responsibilities:

- show available ports
- refresh port list
- edit baud/parity/data/stop/flow settings
- open/close ports
- show basic connection status

---

### `SendPanel`

Mutable editor state before transmission.

```cpp
enum class SendMode {
    Text,
    Hex
};

enum class LineEnding {
    None,
    CR,
    LF,
    CRLF
};

class SendPanel {
public:
    void draw(SerialConnection& connection);

private:
    std::string input_;
    SendMode mode_ = SendMode::Text;
    LineEnding line_ending_ = LineEnding::CRLF;

    std::vector<std::byte> buildPayload() const;
};
```

Important distinction:

```text
The SendPanel is mutable pre-send state.
The SessionStore is append-only post-send history.
```

If the user makes a typo before pressing Send, they edit `SendPanel::input_`. No event exists yet.

---

### `TerminalLogView`

Log-style terminal display derived from `SessionStore`.

```cpp
class TerminalLogView {
public:
    void draw(const SessionStore& session);

private:
    bool autoscroll_ = true;
    bool show_timestamps_ = true;
    bool show_tx_ = true;
    bool show_rx_ = true;
};
```

This is not a full terminal emulator. It is a serial monitor/log display.

It should support:

- RX/TX direction markers
- timestamps
- printable text rendering
- escaping or replacement for non-printable bytes
- autoscroll
- clear view, if implemented as a view operation

The raw event history remains in `SessionStore`.

---

### `HexView`

Hexadecimal byte-oriented view over the same session data.

```cpp
class HexView {
public:
    void draw(const SessionStore& session);

private:
    size_t bytes_per_row_ = 16;
    bool show_ascii_ = true;
};
```

Useful MVP features:

- offset column
- hex bytes
- ASCII side
- RX/TX labels
- copy selected bytes as hex

---

### `StatusBar`

Small summary of runtime state.

```cpp
class StatusBar {
public:
    void draw(const PortManager& ports, const SessionStore& session);
};
```

Useful fields:

- open port count
- RX bytes/sec
- TX bytes/sec
- retained session bytes
- queue depth warning
- last error

---

## Application Loop

### `Application`

Coordinates the top-level frame.

```cpp
class Application {
public:
    void runFrame();

private:
    void drainSerialEvents();

    AppConfig config_;
    PortManager port_manager_;
    SessionStore session_;

    PortPanel port_panel_;
    TerminalLogView terminal_view_;
    HexView hex_view_;
    SendPanel send_panel_;
    StatusBar status_bar_;

    uint64_t next_sequence_ = 0;
};
```

Frame flow:

```text
1. Drain events from every SerialConnection.
2. Append events to SessionStore.
3. Draw PortPanel.
4. Draw SendPanel.
5. Draw TerminalLogView.
6. Draw HexView.
7. Draw StatusBar.
```

Event draining:

```cpp
void Application::drainSerialEvents() {
    for (auto& conn : port_manager_.connections()) {
        SerialEvent ev;
        while (conn->tryPopEvent(ev)) {
            ev.sequence = next_sequence_++;
            session_.append(std::move(ev));
        }
    }
}
```

---

## RX Flow

```text
Device emits bytes
  ↓
OS/driver buffer
  ↓
SerialWorker reads bytes
  ↓
SerialWorker pushes RxBytes event
  ↓
Application drains EventQueue
  ↓
SessionStore appends event
  ↓
TerminalLogView renders text interpretation
  ↓
HexView renders byte interpretation
```

Key point:

```text
Rendering speed should not determine capture speed.
```

The serial worker captures data independently from ImGui rendering.

---

## TX Flow

```text
User edits SendPanel
  ↓
User presses Send
  ↓
SendPanel converts input to bytes
  ↓
SerialConnection pushes WriteBytes command
  ↓
SerialWorker writes bytes to serial port
  ↓
SerialWorker emits TxBytes event
  ↓
Application drains EventQueue
  ↓
SessionStore appends event
  ↓
TerminalLogView / HexView display transmitted bytes
```

Safe write behavior:

```cpp
size_t total_written = 0;

while (total_written < payload.size()) {
    auto remaining = std::span(payload).subspan(total_written);

    int n = transport.write(remaining, timeout_ms);

    if (n > 0) {
        total_written += static_cast<size_t>(n);
    } else {
        emitEvent(/* Error event */);
        break;
    }
}

if (total_written > 0) {
    emitEvent(/* TxBytes event for bytes actually written */);
}
```

---

## Throughput/Drop-Prevention Metrics

For the MVP, add basic instrumentation early.

```cpp
struct SerialStats {
    uint64_t bytes_rx_total = 0;
    uint64_t bytes_tx_total = 0;
    uint64_t read_calls = 0;
    uint64_t rx_events = 0;

    size_t current_event_queue_depth = 0;
    size_t max_event_queue_depth = 0;

    std::chrono::microseconds max_gap_between_reads{};
    std::chrono::microseconds max_read_duration{};
};
```

Warning signs:

```text
event queue grows continuously
read gaps are unexpectedly large
input-waiting count keeps climbing
protocol sequence numbers show gaps
UI freezes during high-volume input
logs cause RX stalls
```

Initial read settings:

```text
read timeout: 10–20 ms
read buffer: 4096–8192 bytes
reader thread: no heavy work
session memory: bounded
```

---

## MVP Milestone Sequence

```text
1. App shell
   - ImGui docking
   - panels
   - basic logging

2. Port enumeration
   - libserialport integration
   - show available ports

3. SerialConnection / SerialWorker skeleton
   - queues
   - thread lifecycle
   - open/close commands
   - events

4. RX path
   - timed blocking read
   - RxBytes events
   - SessionStore append
   - terminal log view

5. TX path
   - SendPanel text mode
   - WriteBytes command
   - robust partial-write loop
   - TxBytes events

6. com0com manual test
   - your app on one side
   - another terminal on the other side
   - verify both directions

7. Hex mode
   - hex send parser
   - hex display
   - copy bytes as hex

8. Bounded scrollback
   - max retained bytes
   - visible retained byte count
   - warning when old RAM history is evicted

9. Throughput instrumentation
   - bytes/sec
   - queue depth
   - max read gap
   - events/sec
   - input-waiting probe if available

10. File logging
   - optional text log
   - later raw/binary log

11. Multi-port
   - multiple SerialConnection objects
   - combined session view
   - per-port filtering
```

---

## Design Principles

1. **The serial port is not the terminal.**  
    The terminal/log view is just one interpretation of the byte/event stream.
2. **The worker owns the port.**  
    Avoid multiple threads calling into the serial handle directly.
3. **The session owns history.**  
    Views should not be the canonical data store.
4. **The send editor is pre-history.**  
    User edits are mutable until bytes are actually sent.
5. **The event stream is append-only.**  
    Once RX/TX/error events happen, preserve them as historical facts.
6. **Rendering must not throttle capture.**  
    The read path should be independent from ImGui rendering.
7. **Start RAM-only, but design for later storage upgrades.**  
    A bounded `SessionStore` is enough for MVP. File-backed live history can come later.
8. **Do not implement a virtual COM driver.**  
    Use normal serial access and test against `com0com` pragmatically.



