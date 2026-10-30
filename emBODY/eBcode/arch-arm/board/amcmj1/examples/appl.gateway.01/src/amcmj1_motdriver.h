// the macromotor as seen by the gateway (CM4): the motor driven by amc.mot on the CM7. it is used only by the gateway thread
// (tGATEWAY), so it needs no protection.
//
// what the Speedgoat asks (amcmj1_gwlink.h):
//   rpc socket 5002       run<OPmotorPIDS, MOTORpids>    the current PID and the velocity PID
//                         run<OPmotorSETUP, MOTORsetup>  the current limits and the motor config (SET_MOTOR_CONFIG 0x77)
//                         run<OPmotorMODE, uint8_t>      the control mode: idle, current, speedvoltage, openloop
//   commands socket 5001  set<IDmotorTARGET, MOTORtarget> the setpoint, at every step (1 ms) while the mode is not idle
// what the gateway tells: sig<IDmotorSTATE, MOTORstate> in every state frame.
//
// the rules here are the same of amc.mot: a control mode other than idle is accepted only after the pids and the setup,
// and the setpoint is used only if it is for the active control mode. no setpoint for watchdogms -> idle ("no news = stop").
//
// the ICC is not here yet: the functions toamcmot*() of amcmj1_motdriver.cpp are the place where the messages for amc.mot
// will be sent, and onamcmot() is the one where the state from amc.mot will arrive. until then, w/ AMCMJ1_MOTDRIVER_EMULATED
// (in amcmj1_motdriver.cpp) the gateway plays amc.mot w/ a simple model, so that the Speedgoat side can be tested.

#pragma once

#include "embot_core.h"
#include <cstdint>

namespace amcmj1::mot {

    // the control modes of amc.mot (embot::prot::can::motor::polling::ControlMode)
    enum class ControlMode : uint8_t
    {
        idle = 0x00,
        current = 0x06,                     // the setpoint is a current [A]
        speedvoltage = 0x0A,                // the setpoint is a velocity [deg/s]
        openloop = 0x50,                    // the setpoint is a voltage, as pwm [%] in [-100, 100]
        hwfault = 0xA0                      // only reported by amc.mot: it cannot be asked
    };

    constexpr bool askable(ControlMode m)
    {
        return (ControlMode::idle == m) || (ControlMode::current == m) || (ControlMode::speedvoltage == m) || (ControlMode::openloop == m);
    }

    // the flags of the motor config (SET_MOTOR_CONFIG 0x77)
    namespace flags {
        constexpr uint8_t hasQE {0x01};             // quadrature encoder for the rotor
        constexpr uint8_t hasHALL {0x02};           // hall sensors
        constexpr uint8_t hasTSENS {0x04};          // temperature sensor
        constexpr uint8_t useINDEX {0x08};          // index of the rotor encoder
        constexpr uint8_t hasSPEEDQE {0x10};        // speed rotor encoder
        constexpr uint8_t enaVERBOSE {0x20};        // verbose
    }

    // the bits of MOTORstate::faults that the gateway looks at (the mask of amc.mot has 32 bits)
    namespace faults {
        constexpr uint32_t externalfault {1u << 7}; // the external fault button is pressed
    }

    // the argument of OPmotorPIDS and the variable IDmotorPIDS (the last ones given to amc.mot)
    struct MOTORpids
    {
        float curkp {0.0f};                 // current PID
        float curki {0.0f};
        float curkd {0.0f};
        float velkp {0.0f};                 // velocity PID
        float velki {0.0f};
        float velkd {0.0f};

        constexpr MOTORpids() = default;
        constexpr explicit MOTORpids(float ckp, float cki, float ckd, float vkp, float vki, float vkd)
            : curkp(ckp), curki(cki), curkd(ckd), velkp(vkp), velki(vki), velkd(vkd) {}
    };

    // the argument of OPmotorSETUP and the variable IDmotorSETUP: the current limits and the motor config. the fields of the
    // config are the ones of SET_MOTOR_CONFIG (0x77), in an order that has no padding. the ICC message is built from them
    struct MOTORsetup
    {
        float nominal {0.0f};               // current limits [A]
        float peak {0.0f};
        float overload {0.0f};
        uint8_t flags {0};                  // mot::flags
        uint8_t poles {0};                  // number of poles of the motor
        uint8_t entlr {0};                  // encoder tolerance
        uint8_t reserved {0};
        int16_t rotencres {0};              // resolution of the rotor encoder
        int16_t rotindexoff {0};            // offset of the rotor encoder

        constexpr MOTORsetup() = default;
        constexpr explicit MOTORsetup(float nom, float pk, float ovl, uint8_t f, uint8_t p, uint8_t e, int16_t res, int16_t off)
            : nominal(nom), peak(pk), overload(ovl), flags(f), poles(p), entlr(e), reserved(0), rotencres(res), rotindexoff(off) {}
    };

    // the setpoint, carried by the command packets: set<IDmotorTARGET, MOTORtarget>. value is in the unit of mode. it is
    // used only if mode is the active control mode. seq is incremented by the sender at every packet
    struct MOTORtarget
    {
        float value {0.0f};                 // current [A], speedvoltage [deg/s], openloop [%]
        ControlMode mode {ControlMode::idle};
        uint8_t reserved[3] {0, 0, 0};
        uint32_t seq {0};

        constexpr MOTORtarget() = default;
        constexpr explicit MOTORtarget(float v, ControlMode m, uint32_t s) : value(v), mode(m), reserved{0, 0, 0}, seq(s) {}
    };

    // what amc.mot tells. sig<IDmotorSTATE> of the state frame
    struct MOTORstate
    {
        float current {0.0f};               // A
        float velocity {0.0f};              // deg/s
        float position {0.0f};              // deg
        float pwm {0.0f};                   // applied pwm [%]
        uint32_t faults {0};                // the fault mask of amc.mot: see mot::faults
        uint32_t counter {0};               // states received from amc.mot (or computed by the emulation)
        ControlMode controlmode {ControlMode::idle};
        uint8_t configured {0};             // bit 0: the pids, bit 1: the setup were given to amc.mot
        uint8_t emulated {0};               // 1: the gateway plays amc.mot (no ICC yet)
        uint8_t gwfault {0};                // 1: the gateway holds the EXTFAULT (OPgwFAULT): it refuses both motors.
                                            // written by the gateway (amcmj1_gwlink) in the state frame, 0 from motdriver

        constexpr MOTORstate() = default;
    };

    // the counters of the gateway about the macromotor: the variable IDmotorSTATUS
    struct MOTORstatus
    {
        uint32_t targets {0};               // setpoints used
        uint32_t rejected {0};              // setpoints not used: not finite, or for another control mode
        uint32_t watchdogs {0};             // times the watchdog went back to idle
        uint32_t modes {0};                 // control modes accepted
        uint32_t refused {0};               // pids, setup or control modes refused

        constexpr MOTORstatus() = default;
    };

    constexpr uint8_t configuredPIDS {0x01};
    constexpr uint8_t configuredSETUP {0x02};
    constexpr uint8_t configuredALL {configuredPIDS | configuredSETUP};

    // no setpoint for so long while the control mode is not idle -> idle
    constexpr embot::core::relTime watchdog {100 * embot::core::time1millisec};

    // the shared variables and the arguments of the operations are at most 36 bytes (a ROP carries 36 bytes of data)
    static_assert(sizeof(MOTORpids) == 24 && sizeof(MOTORsetup) == 20 && sizeof(MOTORtarget) == 12 && sizeof(MOTORstate) == 28 &&
                  sizeof(MOTORstatus) == 20, "layout of the macromotor structs: the host decodes them");

} // namespace amcmj1::mot


namespace amcmj1::motdriver {

    // the outcome of a request. why() tells it w/ a text for the nak of the rpc
    enum class Result : uint8_t { ok = 0, notidle = 1, notconfigured = 2, badvalue = 3, fault = 4 };
    const char *why(Result r);

    // the pids and the setup: only in idle. they are given to amc.mot at once
    Result setpids(const amcmj1::mot::MOTORpids &pids);
    Result setsetup(const amcmj1::mot::MOTORsetup &setup);

    // the control mode. idle is always accepted. the others need the pids and the setup, and no external fault
    Result setmode(amcmj1::mot::ControlMode mode, embot::core::Time now);

    // a setpoint from a command packet: it goes to amc.mot at once if it is for the active control mode. it feeds the watchdog
    enum class Target : uint8_t { used = 0, idle = 1, wrongmode = 2, rejected = 3 };
    Target target(const amcmj1::mot::MOTORtarget &t, embot::core::Time now);

    // call it at every tick of the gateway thread: the watchdog, and the emulation of amc.mot when the setpoints do not arrive
    void tick(embot::core::Time now);

    // the state from amc.mot. the ICC will call it at every message of state (not used w/ the emulation)
    void onamcmot(const amcmj1::mot::MOTORstate &state);

    amcmj1::mot::MOTORstate state();
    amcmj1::mot::MOTORpids pids();
    amcmj1::mot::MOTORsetup setup();
    amcmj1::mot::MOTORtarget lasttarget();
    amcmj1::mot::MOTORstatus status();

} // namespace amcmj1::motdriver
