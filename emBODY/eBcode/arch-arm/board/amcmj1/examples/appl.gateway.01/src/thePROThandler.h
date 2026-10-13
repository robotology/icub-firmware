// thePROThandler.h
// Generic CAN protocol handler + grammar for the PaXini PX3Q joint torque sensor (CAN, manual sec. 5.3).
// - thePROThandler<P> is protocol-agnostic: it filters/dispatches incoming frames
//   and encodes commands, delegating the "grammar" to P.
// - P (e.g. PX3Qprotocol, CubeMarsProtocol) defines Config, Callbacks, parse(), encode().
// - P::parse() may keep state (PX3Q needs it to merge two CAN frames into one sample),
//   so process() is non-const and frames are always parsed in arrival order.
// - The device phase state machine (init / streaming / stop) belongs to a thin
//   device class built on top of the handler, not to the handler itself.

#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "embot_hw_can.h"
#include "theCANdispatcher.h"

namespace embot::app::can {

using Frame = embot::hw::can::Frame;

// ---------------------------------------------------------------------------
// Typed callback: plain function pointer + owner (no heap, RTOS friendly).
// Captureless lambdas convert to fpCall.
// ---------------------------------------------------------------------------
template<typename T>
struct Callback
{
    using fpCall = void (*)(const T &data, void *owner);

    fpCall call  {nullptr};
    void  *owner {nullptr};

    Callback() = delete;    // force the user to state both fields (use nullptr explicitly if unused)
    constexpr Callback(fpCall c, void *o) : call(c), owner(o) {}

    void execute(const T &data) const { if(nullptr != call) { call(data, owner); } }
};

// ---------------------------------------------------------------------------
// Generic handler. P must provide:
//   struct P::Config;
//   struct P::Callbacks;
//   explicit P(const P::Config &);
//   bool parse(const Frame &, const P::Callbacks &);          // true = frame belongs to P (consumed)
//   bool encode(const Cmd &, std::vector<Frame> &out) const;  // one overload per command
// ---------------------------------------------------------------------------
template<typename P>
class thePROThandler : public Parser
{
public:
    using Config    = typename P::Config;
    using Callbacks = typename P::Callbacks;

    // constexpr: a static thePROThandler is constant-initialised, so no code runs at startup (no static-init order issue)
    constexpr thePROThandler(const Config &config, const Callbacks &callbacks)
        : _protocol(config), _callbacks(callbacks) {}

    // interface of Parser, used by theCANdispatcher: it decodes the frame if it belongs to P
    bool parse(const Frame &frame) override { return _protocol.parse(frame, _callbacks); }

    // Frames recognised by P are decoded and dispatched; the others are appended to `unhandled`
    // (reserve() it upfront to avoid allocations). Returns the number of consumed frames.
    std::size_t process(const std::vector<Frame> &input, std::vector<Frame> &unhandled)
    {
        std::size_t consumed {0};
        for(const Frame &fr : input)
        {
            if(_protocol.parse(fr, _callbacks)) { ++consumed; }
            else                                { unhandled.push_back(fr); }
        }
        return consumed;
    }

    // In-place variant: consumed frames are removed, unhandled ones stay in `frames` (order preserved).
    // Handy to chain handlers on the same vector: px3q.process(v); motor.process(v);
    std::size_t process(std::vector<Frame> &frames)
    {
        std::size_t w {0};
        for(std::size_t r = 0; r < frames.size(); ++r)
        {
            if(false == _protocol.parse(frames[r], _callbacks))
            {
                if(w != r) { frames[w] = frames[r]; }
                ++w;
            }
        }
        const std::size_t consumed = frames.size() - w;
        frames.resize(w);
        return consumed;
    }

    // Encode a command into one or more frames appended to `out` (ready for burst TX).
    // Compile error if P has no encode() overload for Cmd; false if the command values are invalid.
    template<typename Cmd>
    bool encode(const Cmd &cmd, std::vector<Frame> &out) const
    {
        return _protocol.encode(cmd, out);
    }

    const P & protocol() const { return _protocol; }
    P &       protocol()       { return _protocol; }

private:
    P         _protocol;
    Callbacks _callbacks;
};


// ===========================================================================
// PaXini PX3Q joint torque sensor - CAN grammar (manual sec. 5.3)
//
// Bus: classic CAN, 1 Mbps, standard 11-bit IDs, DLC always 8. Default device id = 0x7F.
//
// The 11-bit CAN ID is   ID11 = deviceID + (Type << 7)   (deviceID: 7 bits, Type: 4 bits, see enum Type):
//
//   TX  command   Type::command = 0b0100  (0x200 + id)  data[0] = Opcode, data[1..7] = args
//   RX  reply     Type::reply   = 0b0011  (0x180 + id)  data[0] = status (0 = ok), data[1] = Opcode, data[2..7] = payload
//   RX  stream A  Type::streamA = 0b0101  (0x280 + id)  data[0..3] = Mx, data[4..7] = My   (float32, little endian)
//   RX  stream B  Type::streamB = 0b0111  (0x380 + id)  data[0..3] = Mz, data[4..7] = reserved
//
// One torque sample = stream A followed by stream B. There is no sequence counter,
// so the pair is matched by arrival order (A then B).
// ===========================================================================
struct PX3Qprotocol
{
    // ---- configuration ----------------------------------------------------------
    struct Config
    {
        std::uint8_t deviceID {0x7F};
        constexpr explicit Config(std::uint8_t id) : deviceID(static_cast<std::uint8_t>(id & 0x7F)) {}
    };

    // ---- the 11-bit CAN ID: ID11 = deviceID + (Type << 7) ------------------------
    enum class Type : std::uint8_t
    {
        reply   = 0b0011,   // sensor -> host: 0x180 + id
        command = 0b0100,   // host -> sensor: 0x200 + id
        streamA = 0b0101,   // sensor -> host: 0x280 + id
        streamB = 0b0111    // sensor -> host: 0x380 + id
    };

    static constexpr std::uint32_t typeshift {7};   // deviceID takes the 7 least significant bits of the ID11

    static constexpr std::uint32_t makeid(Type t, std::uint8_t deviceID)
    {
        return (static_cast<std::uint32_t>(t) << typeshift) | static_cast<std::uint32_t>(deviceID & 0x7F);
    }

    // ---- opcodes: byte 0 of a command, byte 1 of its reply ----------------------
    enum class Opcode : std::uint8_t
    {
        none       = 0x00,  // not a command: default value
        setID      = 0x01,  // takes effect only after power cycle
        setRate    = 0x02,  // arg = Hz / 4
        start      = 0x03,  // start automatic streaming
        stop       = 0x04,  // stop automatic streaming
        readOnce   = 0x05,  // single sample, returned on stream A + B
        getVersion = 0x07,
        calibrate  = 0x10,  // zeroing: sensor must be unloaded
        setPort    = 0x11   // CAN <-> CAN FD switch
    };

    // value of the argument byte (byte 1) of a command
    static constexpr std::uint8_t noarg {0x00};          // the commands that have no argument
    static constexpr std::uint8_t calibratearg {0x01};   // as in the manual example of calibration

    // ---- decoded data (RX) ------------------------------------------------------
    struct Torque   // N*m (unit as shown by the PaXini host software; to be confirmed)
    {
        float mx {0.0f};
        float my {0.0f};
        float mz {0.0f};    // joint torque axis
        constexpr Torque(float x, float y, float z) : mx(x), my(y), mz(z) {}
    };

    struct Reply    // acknowledge of a command (frame Type::reply)
    {
        std::uint8_t status  {0xFF};
        Opcode cmd           {Opcode::none};    // the command this reply refers to
        std::uint8_t data[6] {0};
        Reply(std::uint8_t st, Opcode c, const std::uint8_t *d) : status(st), cmd(c)
        {
            if(nullptr != d) { std::memmove(data, d, sizeof(data)); }
        }
        bool ok() const { return 0 == status; }
        // valid only for cmd == Opcode::getVersion: 6 ASCII chars, e.g. "v0.1.1"
        std::string version() const { return std::string(reinterpret_cast<const char *>(data), sizeof(data)); }
    };

    struct Callbacks
    {
        Callback<Torque> onTorque;
        Callback<Reply>  onReply;
        constexpr Callbacks(const Callback<Torque> &t, const Callback<Reply> &r) : onTorque(t), onReply(r) {}
    };

    // ---- commands (TX) ----------------------------------------------------------
    struct CmdSetID
    {
        std::uint8_t newID {0};
        constexpr explicit CmdSetID(std::uint8_t id) : newID(id) {}
    };

    struct CmdSetRate
    {
        std::uint16_t hz {0};   // [4, 8, ..., 1000]: a multiple of 4 in [4, 1000], because the byte of the command is Hz / 4
        constexpr explicit CmdSetRate(std::uint16_t f) : hz(f) {}
    };

    struct CmdStart      { constexpr CmdStart()      = default; };
    struct CmdStop       { constexpr CmdStop()       = default; };
    struct CmdReadOnce   { constexpr CmdReadOnce()   = default; };
    struct CmdGetVersion { constexpr CmdGetVersion() = default; };
    struct CmdCalibrate  { constexpr CmdCalibrate()  = default; };

    struct CmdSetPort    // WARNING: switching to CAN FD makes the sensor deaf on a classic CAN bus
    {
        enum class Bus : std::uint8_t { can, canfd };
        Bus bus {Bus::can};
        constexpr explicit CmdSetPort(Bus b) : bus(b) {}
    };

    // ---- diagnostics ------------------------------------------------------------
    struct Stats
    {
        std::uint32_t samples   {0};    // complete Mx/My/Mz samples delivered
        std::uint32_t lostMz    {0};    // stream A overwritten before its stream B arrived
        std::uint32_t orphanMz  {0};    // stream B without a preceding stream A
        std::uint32_t malformed {0};    // our ID but DLC too short
    };

    // ---- grammar ----------------------------------------------------------------
    constexpr explicit PX3Qprotocol(const Config &c) : _config(c) {}

    // it changes the device id at runtime (e.g. from OPpaxINIT) and drops any half-received sample
    void configure(const Config &c) { _config = c; _pendingA = false; }

    constexpr std::uint8_t deviceID() const { return _config.deviceID; }

    bool parse(const Frame &fr, const Callbacks &cbk)
    {
        if(fr.id == idStreamA())
        {
            if(_pendingA) { ++_stats.lostMz; }
            if(fr.size >= 8)
            {
                _mx = rdf(fr.data, 0);
                _my = rdf(fr.data, 4);
                _pendingA = true;
            }
            else
            {
                _pendingA = false;
                ++_stats.malformed;
            }
            return true;
        }

        if(fr.id == idStreamB())
        {
            if(false == _pendingA)  { ++_stats.orphanMz; }
            else if(fr.size < 4)    { ++_stats.malformed; }
            else
            {
                ++_stats.samples;
                cbk.onTorque.execute(Torque(_mx, _my, rdf(fr.data, 0)));
            }
            _pendingA = false;
            return true;
        }

        if(fr.id == idReply())
        {
            if(fr.size >= 2) { cbk.onReply.execute(Reply(fr.data[0], static_cast<Opcode>(fr.data[1]), &fr.data[2])); }
            else             { ++_stats.malformed; }
            return true;
        }

        return false;   // not ours: leave it to the next handler
    }

    bool encode(const CmdSetID &cmd, std::vector<Frame> &out) const
    {
        if(cmd.newID > 0x7F) { return false; }
        push(out, Opcode::setID, cmd.newID);
        return true;
    }

    bool encode(const CmdSetRate &cmd, std::vector<Frame> &out) const
    {
        if((cmd.hz < 4) || (cmd.hz > 1000) || (0 != (cmd.hz % 4))) { return false; }
        push(out, Opcode::setRate, static_cast<std::uint8_t>(cmd.hz / 4));
        return true;
    }

    bool encode(const CmdStart &,      std::vector<Frame> &out) const { push(out, Opcode::start,      noarg); return true; }
    bool encode(const CmdStop &,       std::vector<Frame> &out) const { push(out, Opcode::stop,       noarg); return true; }
    bool encode(const CmdReadOnce &,   std::vector<Frame> &out) const { push(out, Opcode::readOnce,   noarg); return true; }
    bool encode(const CmdGetVersion &, std::vector<Frame> &out) const { push(out, Opcode::getVersion, noarg); return true; }
    bool encode(const CmdCalibrate &,  std::vector<Frame> &out) const { push(out, Opcode::calibrate,  calibratearg); return true; }

    bool encode(const CmdSetPort &cmd, std::vector<Frame> &out) const
    {
        // byte layout copied verbatim from the manual (sec. 5.4); meaning of single bytes undocumented
        const std::uint8_t fd = (CmdSetPort::Bus::canfd == cmd.bus) ? 0x01 : 0x00;
        std::uint8_t d[8] {static_cast<std::uint8_t>(Opcode::setPort), fd, 0x00, 0x01, 0x03, fd, 0x00, 0x00};
        out.push_back(Frame(idCommand(), 8, d));
        return true;
    }

    // call it when (re)starting the stream, to drop a half-received sample
    void resync() { _pendingA = false; }

    const Stats & stats() const { return _stats; }

private:
    constexpr std::uint32_t idReply()   const { return makeid(Type::reply,   _config.deviceID); }
    constexpr std::uint32_t idCommand() const { return makeid(Type::command, _config.deviceID); }
    constexpr std::uint32_t idStreamA() const { return makeid(Type::streamA, _config.deviceID); }
    constexpr std::uint32_t idStreamB() const { return makeid(Type::streamB, _config.deviceID); }

    void push(std::vector<Frame> &out, Opcode op, std::uint8_t arg) const
    {
        std::uint8_t d[8] {static_cast<std::uint8_t>(op), arg, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        out.push_back(Frame(idCommand(), 8, d));
    }

    // float32 little endian on a little endian host (Cortex-M): memcpy avoids aliasing UB
    static float rdf(const std::uint8_t *d, std::size_t pos)
    {
        float f {0.0f};
        std::memcpy(&f, d + pos, sizeof(f));
        return f;
    }

    Config _config;
    bool   _pendingA {false};
    float  _mx {0.0f};
    float  _my {0.0f};
    Stats  _stats {};
};

} // namespace embot::app::can


// ===========================================================================
// Usage: see amcmj1_paxdriver.cpp. Below, the general sketch
// ===========================================================================
// Usage sketch (inside the thread that owns the CAN bus)
// ===========================================================================
//
// using namespace embot::app::can;
//
// thePROThandler<PX3Qprotocol> px3q {
//     PX3Qprotocol::Config(0x7F),
//     PX3Qprotocol::Callbacks(
//         Callback<PX3Qprotocol::Torque>([](const PX3Qprotocol::Torque &t, void *o) { /* use t.mx, t.my, t.mz */ }, nullptr),
//         Callback<PX3Qprotocol::Reply>( [](const PX3Qprotocol::Reply &r,  void *o) { /* ack: r.cmd, r.ok() */  }, nullptr)
//     )
// };
// thePROThandler<CubeMarsProtocol> motor { ... };
//
// // --- init phase: one command at a time, wait for the reply with matching cmd and ok()
// //     1. CmdGetVersion       -> check firmware (r.version())
// //     2. CmdCalibrate        -> sensor must be unloaded
// //     3. CmdSetRate(1000)
// //     4. CmdStart            -> then protocol().resync(); stream arrives on Type::streamA / Type::streamB
// std::vector<Frame> tx; tx.reserve(4);
// px3q.encode(PX3Qprotocol::CmdCalibrate(), tx);
// // ... put tx into embot::hw::can and transmit, then wait for onReply(cmd == Opcode::calibrate)
//
// // --- RX loop: drain the driver RX queue into rx, then chain the handlers
// std::vector<Frame> rx; rx.reserve(32);
// // ... fill rx from embot::hw::can ...
// px3q.process(rx);     // PX3Q frames consumed, others remain
// motor.process(rx);    // motor frames consumed, others remain
// // whatever is left in rx is unknown traffic: count / log it
//
// // --- stop phase
// px3q.encode(PX3Qprotocol::CmdStop(), tx);
