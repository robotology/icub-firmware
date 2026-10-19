// the driver of the CubeMars AK70-9 motor over CAN (extended ids, manual v3.2.0), built on thePROThandler<CubeMarsProtocol>.
// it is used only by the ctrl thread, so it needs no protection.
//
// the setpoint is a loop, not a command: the MIT packet (or a servo packet) is sent again every cfg.repeatms for as long as
// the host keeps writing it. the host marks every write by incrementing seq: if seq does not change for cfg.watchdogms the
// driver sends the disable packet once and stops ("no news = stop"). the motor does not ack the packets, so init(), start(),
// stop() and deinit() complete at once: the only reply is the status frame, decoded by take().

#pragma once

#include "embot_core.h"
#include <cstdint>

namespace amcmj1::cube {

    // the MIT ranges of the AK70-9, the same of the encoder in theprothandler.h
    constexpr float pmax {12.56f};          // rad
    constexpr float vmax {30.0f};           // rad/s
    constexpr float tmax {32.0f};           // N*m
    constexpr float kpmax {500.0f};
    constexpr float kdmax {5.0f};

    // conversion of the speed of the status frame: ERPM -> rad/s of the output shaft (14 pole pairs, reduction 9)
    constexpr float erpm2rads {6.283185307f / 60.0f / (14.0f * 9.0f)};

    // the configuration of the driver, given by run<OPcubeINIT, {...}>
    struct CUBEcfg
    {
        uint32_t canid {0x68};              // driver id of the motor, in [1, 255]. CAN ids (29 bit): (packet << 8) | canid
        uint32_t repeatms {10};             // the setpoint is sent again every repeatms: [10, 100], the granularity is the tick of the ctrl thread
        uint32_t watchdogms {100};          // no write of the setpoint for so long -> disable: [2 * repeatms, 10000]

        constexpr CUBEcfg() = default;
        constexpr explicit CUBEcfg(uint32_t id, uint32_t rep, uint32_t wd) : canid(id), repeatms(rep), watchdogms(wd) {}
    };

    constexpr uint32_t canidmin {1};
    constexpr uint32_t canidmax {255};
    constexpr uint32_t repeatmin {10};
    constexpr uint32_t repeatmax {100};
    constexpr uint32_t watchdogmax {10000};

    constexpr bool valid(const CUBEcfg &cfg)
    {
        return (cfg.canid >= canidmin) && (cfg.canid <= canidmax) && (cfg.repeatms >= repeatmin) && (cfg.repeatms <= repeatmax) &&
               (cfg.watchdogms >= 2 * cfg.repeatms) && (cfg.watchdogms <= watchdogmax);
    }

    static_assert(valid(CUBEcfg{}), "the default CUBEcfg must be valid");

    // off -> (init) -> initialised -> (start) -> running -> (stop) -> initialised. error: the CAN refused a frame
    enum class State : uint8_t { off = 0, initialised = 1, running = 2, error = 3 };
    enum class Command : uint8_t { none = 0, init = 1, deinit = 2, start = 3, stop = 4 };

    struct CUBEstatus
    {
        State state {State::off};
        Command lastcommand {Command::none};
        uint16_t errors {0};                // commands refused + frames that the CAN did not accept
        uint32_t commands {0};              // number of commands accepted
        uint32_t watchdogs {0};             // times the watchdog stopped the motor

        constexpr CUBEstatus() = default;
    };

    // the MIT setpoint: the motor applies  torque_out = kp * (pos - pos_motor) + kd * (vel - vel_motor) + torque
    // it is the shared variable vCubeMIT, written by the host: seq must change at every write
    struct CUBEmit
    {
        float kp {0.0f};                    // [0, 500]
        float kd {0.0f};                    // [0, 5]
        float pos {0.0f};                   // rad, +-12.56
        float vel {0.0f};                   // rad/s, +-30
        float torque {0.0f};                // N*m, +-32
        uint32_t seq {0};

        constexpr CUBEmit() = default;
        constexpr explicit CUBEmit(float k, float d, float p, float v, float t, uint32_t s) : kp(k), kd(d), pos(p), vel(v), torque(t), seq(s) {}
    };

    constexpr bool inrange(float x, float lo, float hi) { return (x >= lo) && (x <= hi); }      // false for NaN

    constexpr bool valid(const CUBEmit &m)
    {
        return inrange(m.kp, 0.0f, kpmax) && inrange(m.kd, 0.0f, kdmax) && inrange(m.pos, -pmax, pmax) &&
               inrange(m.vel, -vmax, vmax) && inrange(m.torque, -tmax, tmax);
    }

    // the servo packets of the manual (sec. 4.1): the meaning of a, b, c depends on the packet
    enum class Servo : uint8_t
    {
        duty = 0,                           // a: duty [-1, 1]
        current = 1,                        // a: current [-60, 60] A  (torque control)
        brake = 2,                          // a: brake current [0, 60] A
        velocity = 3,                       // a: speed [-100000, 100000] ERPM (electrical rpm)
        position = 4,                       // a: position [-36000, 36000] deg
        origin = 5,                         // a: 0 temporary, 1 permanent. sent once
        posvel = 6                          // a: position deg, b: speed ERPM [-327680, 327670], c: acceleration ERPM/s2 [0, 327670]
    };

    // the argument of OPcubeSERVO
    struct ServoCmd
    {
        Servo packet {Servo::current};
        float a {0.0f};
        float b {0.0f};
        float c {0.0f};

        constexpr ServoCmd() = default;
        constexpr explicit ServoCmd(Servo p, float x, float y, float z) : packet(p), a(x), b(y), c(z) {}
    };

    constexpr bool valid(const ServoCmd &s)
    {
        switch(s.packet)
        {
            case Servo::duty:       return inrange(s.a, -1.0f, 1.0f);
            case Servo::current:    return inrange(s.a, -60.0f, 60.0f);
            case Servo::brake:      return inrange(s.a, 0.0f, 60.0f);
            case Servo::velocity:   return inrange(s.a, -100000.0f, 100000.0f);
            case Servo::position:   return inrange(s.a, -36000.0f, 36000.0f);
            case Servo::origin:     return inrange(s.a, 0.0f, 1.0f);
            case Servo::posvel:     return inrange(s.a, -36000.0f, 36000.0f) && inrange(s.b, -327680.0f, 327670.0f) && inrange(s.c, 0.0f, 327670.0f);
            default:                return false;
        }
    }

    // the servo setpoint as a shared variable vCubeServo: written by OPcubeSERVO, which increments seq
    struct CUBEservo
    {
        Servo packet {Servo::current};
        float a {0.0f};
        float b {0.0f};
        float c {0.0f};
        uint32_t seq {0};

        constexpr CUBEservo() = default;
    };

    // what the motor tells, in the units of the MIT packet. it is the shared variable vCubeState
    struct CUBEstate
    {
        float pos {0.0f};                   // rad. the motor sends 0.1 deg resolution, or 0.0001 deg if enabled w/ CmdFeedbackCfg
        float vel {0.0f};                   // rad/s
        float current {0.0f};               // A
        int16_t temp {0};                   // degrees C
        uint16_t error {0};                 // 0 none, 1 over-temp, 2 over-current, 3 over-voltage, 4 under-voltage, 5 encoder, 6 phase imbalance
        uint32_t counter {0};               // status frames received

        constexpr CUBEstate() = default;
    };

    // the shared variables and the argument of the operations are at most 36 bytes (a ROP carries 36 bytes of data)
    static_assert(sizeof(CUBEcfg) == 12 && sizeof(CUBEstatus) == 12 && sizeof(CUBEmit) == 24 &&
                  sizeof(ServoCmd) == 16 && sizeof(CUBEservo) == 20 && sizeof(CUBEstate) == 20, "layout of the Cube structs: the host decodes them");

} // namespace amcmj1::cube


namespace amcmj1::cubedriver {

    // it attaches the driver to the CAN bus (amcmj1_canbus.h), which must be open. false if it is not
    bool open();

    // the commands. false if not accepted: wrong state (init while running, start before init), invalid cfg, CAN not open.
    bool init(const amcmj1::cube::CUBEcfg &cfg);
    bool start();                           // from now on the setpoints written by the host are sent
    bool stop();                            // disable packet, then the setpoints are ignored
    bool deinit();                          // stop if running, then off

    // call them at every tick of the ctrl thread, w/ the shared variables, before tick(): when seq changes the setpoint becomes
    // the active one and is sent at once. a setpoint that was already there at start() is ignored. a servo packet "origin"
    // is sent once and does not become the active setpoint
    void update(const amcmj1::cube::CUBEmit &mit, embot::core::Time now);
    void update(const amcmj1::cube::CUBEservo &servo, embot::core::Time now);

    // call it at every tick of the ctrl thread: it sends again the active setpoint, or disables the motor if the watchdog expired
    void tick(embot::core::Time now);

    // call it after amcmj1::canbus::rx(): true if at least one status frame arrived since the last call; state is the latest
    bool take(amcmj1::cube::CUBEstate &state);

    amcmj1::cube::State state();

    struct Stats
    {
        uint32_t txframes {0};              // frames sent
        uint32_t txerrors {0};              // frames that the CAN did not accept, or setpoints out of range
        uint32_t watchdogs {0};             // times the watchdog stopped the motor
        uint32_t status {0};                // 0x29 frames
        uint32_t position32 {0};            // 0x2A frames
        uint32_t starts {0};                // 0x2C frames: the motor has just been powered
        uint32_t malformed {0};
        constexpr Stats() = default;
    };
    Stats stats();

} // namespace amcmj1::cubedriver
