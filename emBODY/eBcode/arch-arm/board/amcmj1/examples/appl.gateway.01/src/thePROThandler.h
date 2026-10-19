// theprothandler.h
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
class thePROThandler
{
public:
    using Config    = typename P::Config;
    using Callbacks = typename P::Callbacks;

    thePROThandler(const Config &config, const Callbacks &callbacks)
        : _protocol(config), _callbacks(callbacks) {}

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
// Bus: classic CAN, 1 Mbps, standard 11-bit IDs (extended frames are ignored), DLC always 8. Default device id = 0x7F.
//
//   TX  command   0x200 + id   data[0] = cmd, data[1..7] = args
//   RX  reply     0x180 + id   data[0] = status (0 = ok), data[1] = cmd, data[2..7] = payload
//   RX  stream A  0x280 + id   data[0..3] = Mx, data[4..7] = My   (float32, little endian)
//   RX  stream B  0x380 + id   data[0..3] = Mz, data[4..7] = reserved
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

    // ---- decoded data (RX) ------------------------------------------------------
    struct Torque   // N*m (unit as shown by the PaXini host software; to be confirmed)
    {
        float mx {0.0f};
        float my {0.0f};
        float mz {0.0f};    // joint torque axis
        constexpr Torque(float x, float y, float z) : mx(x), my(y), mz(z) {}
    };

    struct Reply    // acknowledge of a command (frame 0x180 + id)
    {
        std::uint8_t status  {0xFF};
        std::uint8_t cmd     {0};
        std::uint8_t data[6] {0};
        Reply(std::uint8_t st, std::uint8_t c, const std::uint8_t *d) : status(st), cmd(c)
        {
            if(nullptr != d) { std::memmove(data, d, sizeof(data)); }
        }
        bool ok() const { return 0 == status; }
        // valid only for cmd == getVersion: 6 ASCII chars, e.g. "v0.1.1"
        std::string version() const { return std::string(reinterpret_cast<const char *>(data), sizeof(data)); }
    };

    struct Callbacks
    {
        Callback<Torque> onTorque;
        Callback<Reply>  onReply;
        constexpr Callbacks(const Callback<Torque> &t, const Callback<Reply> &r) : onTorque(t), onReply(r) {}
    };

    // ---- opcodes ----------------------------------------------------------------
    enum class Opcode : std::uint8_t
    {
        setID      = 0x01,  // takes effect only after power cycle
        setRate    = 0x02,  // arg = Hz / 4
        start      = 0x03,  // start automatic streaming
        stop       = 0x04,  // stop automatic streaming
        readOnce   = 0x05,  // single sample, returned on stream A + B
        getVersion = 0x07,
        calibrate  = 0x10,  // zeroing: sensor must be unloaded
        setPort    = 0x11   // CAN <-> CAN FD switch
    };

    // ---- commands (TX) ----------------------------------------------------------
    struct CmdSetID
    {
        std::uint8_t newID {0};
        constexpr explicit CmdSetID(std::uint8_t id) : newID(id) {}
    };

    struct CmdSetRate
    {
        std::uint16_t hz {0};   // 4 ... 1000, multiple of 4
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
        if(fr.isextended())
        {
            return false;   // the PX3Q speaks standard 11-bit ids only: an extended id (e.g. the CubeMars) is never ours
        }

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
            if(fr.size >= 2) { cbk.onReply.execute(Reply(fr.data[0], fr.data[1], &fr.data[2])); }
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

    bool encode(const CmdStart &,      std::vector<Frame> &out) const { push(out, Opcode::start,      0x00); return true; }
    bool encode(const CmdStop &,       std::vector<Frame> &out) const { push(out, Opcode::stop,       0x00); return true; }
    bool encode(const CmdReadOnce &,   std::vector<Frame> &out) const { push(out, Opcode::readOnce,   0x00); return true; }
    bool encode(const CmdGetVersion &, std::vector<Frame> &out) const { push(out, Opcode::getVersion, 0x00); return true; }
    bool encode(const CmdCalibrate &,  std::vector<Frame> &out) const { push(out, Opcode::calibrate,  0x01); return true; }

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
    constexpr std::uint32_t idReply()   const { return 0x180u + _config.deviceID; }
    constexpr std::uint32_t idCommand() const { return 0x200u + _config.deviceID; }
    constexpr std::uint32_t idStreamA() const { return 0x280u + _config.deviceID; }
    constexpr std::uint32_t idStreamB() const { return 0x380u + _config.deviceID; }

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


// ===========================================================================
// CubeMars AK series (AK70-9 KV60) - CAN grammar (manual v3.2.0, sec. 4.1, 4.2, 4.3)
//
// Bus: CAN 1 Mbps, EXTENDED 29-bit IDs (standard frames are ignored), DLC 8, big endian.
//   ID29 = (packet << 8) | driverID          default driverID = 0x68
//
//   TX  servo packets  0 duty (int32 x100000)        1 current (int32 mA)       2 brake current (int32 mA)
//                      3 velocity (int32 ERPM)       4 position (int32 deg x10000)
//                      5 origin (u8: 0 temporary, 1 permanent)
//                      6 pos-vel (int32 pos deg x10000, int16 ERPM/10, int16 (ERPM/s2)/10)
//                      15 disable (the motor answers once w/ a 0x29 frame w/ data[7] = 0x77)
//                      16 feedback config (writes the flash of the motor: not for the loop)
//   TX  MIT packet 8   kp 12 bit [0, 500], kd 12 bit [0, 5], pos 16 bit +-12.56 rad, vel 12 bit +-30 rad/s,
//                      torque 12 bit +-32 N*m. it must be sent again and again: it is the setpoint of a loop
//   RX  0x29 status    pos int16 (0.1 deg), speed int16 (x10 ERPM), current int16 (0.01 A), temp int8, error u8
//   RX  0x2A           pos int32 (0.0001 deg), only if enabled w/ CmdFeedbackCfg
//   RX  0x2C           FA FB FC FD: the motor has just started
// the format of the reply to a MIT packet is not documented: it is assumed to be the same 0x29 / 0x2A (to verify)
// ===========================================================================
struct CubeMarsProtocol
{
    // ---- the ranges of the MIT packet, per model
    struct Limits
    {
        float pmax {12.56f};    // rad
        float vmax {30.0f};     // rad/s
        float tmax {32.0f};     // N*m
        float kpmax {500.0f};
        float kdmax {5.0f};
        constexpr Limits() = default;
        constexpr explicit Limits(float p, float v, float t, float kp, float kd) : pmax(p), vmax(v), tmax(t), kpmax(kp), kdmax(kd) {}
    };

    // ---- configuration ----------------------------------------------------------
    struct Config
    {
        std::uint8_t driverID {0x68};
        Limits limits {};
        constexpr explicit Config(std::uint8_t id) : driverID(id) {}
        constexpr explicit Config(std::uint8_t id, const Limits &l) : driverID(id), limits(l) {}
    };

    // ---- decoded data (RX) ------------------------------------------------------
    struct Status           // frame 0x29
    {
        float posdeg {0.0f};        // output shaft, 0.1 deg resolution, wraps at +-3276.7 deg
        float speederpm {0.0f};     // ERPM (electrical rpm)
        float currentA {0.0f};      // phase current
        std::int8_t temp {0};       // degrees C
        std::uint8_t error {0};     // 0 none, 1 over-temp, 2 over-current, 3 over-voltage, 4 under-voltage, 5 encoder, 6 phase imbalance
        bool disableack {false};    // data[7] == 0x77: the answer to CmdDisable (error is not meaningful)
        constexpr Status() = default;
    };

    struct Position32       // frame 0x2A
    {
        float posdeg {0.0f};        // 0.0001 deg resolution
        constexpr explicit Position32(float p) : posdeg(p) {}
    };

    struct Callbacks
    {
        Callback<Status>     onStatus;
        Callback<Position32> onPosition32;
        constexpr Callbacks(const Callback<Status> &s, const Callback<Position32> &p) : onStatus(s), onPosition32(p) {}
    };

    // ---- packets ----------------------------------------------------------------
    enum class Packet : std::uint8_t
    {
        duty = 0, current = 1, currentBrake = 2, velocity = 3, position = 4, origin = 5, posvel = 6,
        mit = 8, disable = 15, feedbackCfg = 16
    };
    enum class Feedback : std::uint8_t { status = 0x29, position32 = 0x2A, start = 0x2C };

    // ---- commands (TX), in physical units ---------------------------------------
    struct CmdDuty        { float duty;  constexpr explicit CmdDuty(float d) : duty(d) {} };               // [-1, 1]
    struct CmdCurrent     { float amps;  constexpr explicit CmdCurrent(float a) : amps(a) {} };            // [-60, 60] A
    struct CmdBrake       { float amps;  constexpr explicit CmdBrake(float a) : amps(a) {} };              // [0, 60] A
    struct CmdVelocity    { float erpm;  constexpr explicit CmdVelocity(float e) : erpm(e) {} };           // +-100000
    struct CmdPosition    { float deg;   constexpr explicit CmdPosition(float d) : deg(d) {} };            // +-36000
    struct CmdOrigin      { bool permanent; constexpr explicit CmdOrigin(bool p) : permanent(p) {} };
    struct CmdPosVel
    {
        float deg; float erpm; float accel;                                                                // accel in ERPM/s2
        constexpr explicit CmdPosVel(float d, float e, float a) : deg(d), erpm(e), accel(a) {}
    };
    struct CmdMIT
    {
        float kp; float kd; float pos; float vel; float torque;                                            // -, -, rad, rad/s, N*m
        constexpr explicit CmdMIT(float k, float d, float p, float v, float t) : kp(k), kd(d), pos(p), vel(v), torque(t) {}
    };
    struct CmdDisable     { constexpr CmdDisable() = default; };
    struct CmdFeedbackCfg
    {
        bool pos32; bool singleturn;
        constexpr explicit CmdFeedbackCfg(bool p, bool s) : pos32(p), singleturn(s) {}
    };

    // ---- diagnostics ------------------------------------------------------------
    struct Stats
    {
        std::uint32_t status    {0};    // 0x29 frames
        std::uint32_t position32 {0};   // 0x2A frames
        std::uint32_t starts    {0};    // 0x2C frames
        std::uint32_t malformed {0};    // our packet but wrong DLC / content
    };

    // ---- grammar ----------------------------------------------------------------
    constexpr explicit CubeMarsProtocol(const Config &c) : _config(c) {}

    void configure(const Config &c) { _config = c; }
    constexpr std::uint8_t driverID() const { return _config.driverID; }
    constexpr const Limits & limits() const { return _config.limits; }

    bool parse(const Frame &fr, const Callbacks &cbk)
    {
        if((false == fr.isextended()) || (driverid(fr.id) != _config.driverID))
        {
            return false;   // a standard frame (e.g. the PX3Q), or another driver
        }
        const std::uint32_t pk {(fr.id >> 8) & 0x1FFFFFu};
        switch(pk)
        {
            case static_cast<std::uint32_t>(Feedback::status):
            {
                if(8 != fr.size) { ++_stats.malformed; return true; }
                Status s {};
                s.posdeg = static_cast<float>(get16(fr.data)) * 0.1f;
                s.speederpm = static_cast<float>(get16(fr.data + 2)) * 10.0f;
                s.currentA = static_cast<float>(get16(fr.data + 4)) * 0.01f;
                s.temp = static_cast<std::int8_t>(fr.data[6]);
                s.error = fr.data[7];
                s.disableack = (0x77 == fr.data[7]);
                ++_stats.status;
                cbk.onStatus.execute(s);
                return true;
            }
            case static_cast<std::uint32_t>(Feedback::position32):
            {
                if(fr.size < 4) { ++_stats.malformed; return true; }
                ++_stats.position32;
                cbk.onPosition32.execute(Position32(static_cast<float>(get32(fr.data)) * 0.0001f));
                return true;
            }
            case static_cast<std::uint32_t>(Feedback::start):
            {
                if((4 == fr.size) && (0xFA == fr.data[0]) && (0xFB == fr.data[1]) && (0xFC == fr.data[2]) && (0xFD == fr.data[3]))
                {
                    ++_stats.starts;
                }
                else
                {
                    ++_stats.malformed;
                }
                return true;
            }
            default:
            {
                return false;   // a command packet (our own echo, or another master): not ours to decode
            }
        }
    }

    // false if a value is out of range (nothing is appended)
    bool encode(const CmdDuty &c, std::vector<Frame> &out) const
    {
        return inrange(c.duty, -1.0f, 1.0f) && pushi32(out, Packet::duty, c.duty * 100000.0f);
    }
    bool encode(const CmdCurrent &c, std::vector<Frame> &out) const
    {
        return inrange(c.amps, -60.0f, 60.0f) && pushi32(out, Packet::current, c.amps * 1000.0f);
    }
    bool encode(const CmdBrake &c, std::vector<Frame> &out) const
    {
        return inrange(c.amps, 0.0f, 60.0f) && pushi32(out, Packet::currentBrake, c.amps * 1000.0f);
    }
    bool encode(const CmdVelocity &c, std::vector<Frame> &out) const
    {
        return inrange(c.erpm, -100000.0f, 100000.0f) && pushi32(out, Packet::velocity, c.erpm);
    }
    bool encode(const CmdPosition &c, std::vector<Frame> &out) const
    {
        return inrange(c.deg, -36000.0f, 36000.0f) && pushi32(out, Packet::position, c.deg * 10000.0f);
    }
    bool encode(const CmdOrigin &c, std::vector<Frame> &out) const
    {
        std::uint8_t d[8] {static_cast<std::uint8_t>(c.permanent ? 1 : 0), 0, 0, 0, 0, 0, 0, 0};
        out.push_back(Frame(makeid(Packet::origin), 1, d, true));
        return true;
    }
    bool encode(const CmdPosVel &c, std::vector<Frame> &out) const
    {
        if(!inrange(c.deg, -36000.0f, 36000.0f) || !inrange(c.erpm, -327680.0f, 327670.0f) || !inrange(c.accel, 0.0f, 327670.0f))
        {
            return false;
        }
        std::uint8_t d[8] {};
        put32(d, round(c.deg * 10000.0f));
        put16(d + 4, round(c.erpm / 10.0f));
        put16(d + 6, round(c.accel / 10.0f));
        out.push_back(Frame(makeid(Packet::posvel), 8, d, true));
        return true;
    }
    bool encode(const CmdMIT &c, std::vector<Frame> &out) const
    {
        const Limits &l {_config.limits};
        if(!inrange(c.kp, 0.0f, l.kpmax) || !inrange(c.kd, 0.0f, l.kdmax) || !inrange(c.pos, -l.pmax, l.pmax) ||
           !inrange(c.vel, -l.vmax, l.vmax) || !inrange(c.torque, -l.tmax, l.tmax))
        {
            return false;
        }
        const std::uint16_t k {f2u(c.kp, 0.0f, l.kpmax, 12)};
        const std::uint16_t d {f2u(c.kd, 0.0f, l.kdmax, 12)};
        const std::uint16_t p {f2u(c.pos, -l.pmax, l.pmax, 16)};
        const std::uint16_t v {f2u(c.vel, -l.vmax, l.vmax, 12)};
        const std::uint16_t t {f2u(c.torque, -l.tmax, l.tmax, 12)};
        std::uint8_t b[8] {};
        b[0] = static_cast<std::uint8_t>(k >> 4);
        b[1] = static_cast<std::uint8_t>(((k & 0xF) << 4) | (d >> 8));
        b[2] = static_cast<std::uint8_t>(d & 0xFF);
        b[3] = static_cast<std::uint8_t>(p >> 8);
        b[4] = static_cast<std::uint8_t>(p & 0xFF);
        b[5] = static_cast<std::uint8_t>(v >> 4);
        b[6] = static_cast<std::uint8_t>(((v & 0xF) << 4) | (t >> 8));
        b[7] = static_cast<std::uint8_t>(t & 0xFF);
        out.push_back(Frame(makeid(Packet::mit), 8, b, true));
        return true;
    }
    bool encode(const CmdDisable &, std::vector<Frame> &out) const
    {
        std::uint8_t d[8] {};
        out.push_back(Frame(makeid(Packet::disable), 0, d, true));
        return true;
    }
    bool encode(const CmdFeedbackCfg &c, std::vector<Frame> &out) const
    {   // it writes the flash of the motor: send it rarely, never in a loop
        const std::uint16_t flags {static_cast<std::uint16_t>((c.pos32 ? 0x8000 : 0) | (c.singleturn ? 0x4000 : 0))};
        std::uint8_t d[8] {0, 0, 0, 0, 0, 0, static_cast<std::uint8_t>(flags >> 8), static_cast<std::uint8_t>(flags & 0xFF)};
        out.push_back(Frame(makeid(Packet::feedbackCfg), 8, d, true));
        return true;
    }

    const Stats & stats() const { return _stats; }

    static constexpr std::uint8_t driverid(std::uint32_t id) { return static_cast<std::uint8_t>(id & 0xFF); }

private:
    constexpr std::uint32_t makeid(Packet p) const { return (static_cast<std::uint32_t>(p) << 8) | _config.driverID; }

    static constexpr bool inrange(float x, float lo, float hi) { return (x >= lo) && (x <= hi); }   // false for NaN too
    static constexpr std::int32_t round(float x) { return static_cast<std::int32_t>(x >= 0.0f ? x + 0.5f : x - 0.5f); }
    static constexpr std::uint16_t f2u(float x, float lo, float hi, unsigned bits)
    {
        return static_cast<std::uint16_t>((x - lo) * static_cast<float>((1u << bits) - 1u) / (hi - lo));
    }
    static void put32(std::uint8_t *d, std::int32_t v)
    {
        const std::uint32_t u {static_cast<std::uint32_t>(v)};
        d[0] = static_cast<std::uint8_t>(u >> 24); d[1] = static_cast<std::uint8_t>(u >> 16);
        d[2] = static_cast<std::uint8_t>(u >> 8);  d[3] = static_cast<std::uint8_t>(u);
    }
    static void put16(std::uint8_t *d, std::int32_t v)
    {
        const std::uint32_t u {static_cast<std::uint32_t>(v)};
        d[0] = static_cast<std::uint8_t>(u >> 8); d[1] = static_cast<std::uint8_t>(u);
    }
    static std::int16_t get16(const std::uint8_t *d) { return static_cast<std::int16_t>((d[0] << 8) | d[1]); }
    static std::int32_t get32(const std::uint8_t *d)
    {
        return static_cast<std::int32_t>((std::uint32_t(d[0]) << 24) | (std::uint32_t(d[1]) << 16) | (std::uint32_t(d[2]) << 8) | d[3]);
    }
    bool pushi32(std::vector<Frame> &out, Packet p, float v) const
    {
        std::uint8_t d[8] {};
        put32(d, round(v));
        out.push_back(Frame(makeid(p), 4, d, true));
        return true;
    }

    Config _config;
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
// //     4. CmdStart            -> then protocol().resync(); stream arrives on 0x280+id / 0x380+id
// std::vector<Frame> tx; tx.reserve(4);
// px3q.encode(PX3Qprotocol::CmdCalibrate(), tx);
// // ... put tx into embot::hw::can and transmit, then wait for onReply(cmd == 0x10)
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
