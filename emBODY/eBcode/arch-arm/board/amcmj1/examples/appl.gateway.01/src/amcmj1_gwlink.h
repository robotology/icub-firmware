// the link of amcmj1 w/ the Speedgoat: what travels on the commands, rpc and state sockets, and the rules of the gate.
// it has no socket and no thread: the gateway thread (appl.gateway.01.cpp) gives it the received payloads and sends the
// frames it builds, so the rules can be tested on a PC (test/host).
//
//   Speedgoat --UDP 5001 commands--> tGATEWAY --CAN 29 bit--> CubeMars
//   Speedgoat <--UDP 5003 state----- tGATEWAY <--CAN-------- CubeMars, PX3Q
//   Speedgoat --UDP 5002 rpc-------> tGATEWAY (enable / disable)
//
// the frames are TETHER frames (embot_app_eth_ROP.h: header of 16 bytes + slots of ROPs, little endian).
//
// commands: a frame arrives at every step of the Speedgoat (10 ms now, 1 ms later), also when it has nothing to say.
//   - set<IDcubeMIT, {kp, kd, pos, vel, torque, seq}>   the MIT setpoint. it goes to the CAN at once if the motor is enabled
//   - any other ROP is counted and ignored (the second set<> of the CM7 motor is not wired yet)
//   - a frame w/out a MIT sends the last MIT again. every valid frame feeds the watchdog
//   the limits (GWlimits) are applied here: a value out of them is clamped and counted, a NaN rejects the MIT.
//   the packets are the only source of the MIT: the rpc of TETHER cannot write it
//
// state: one frame per command frame, to the sender of the command frame, w/ 5 sig<> ROPs (slots of 40 bytes):
//   sig<IDcubeSTATUS>, sig<IDcubeSTATE>, sig<IDpaxTORQUE>, sig<IDmotorSTATE>, sig<IDgwSTATS>
//   GWstats::seq echoes the seq of the last MIT: the sender can measure the round trip
//
// rpc of the gateway: run[+ack]<OPcubeENABLE>, <OPcubeDISABLE>, <OPpaxENABLE>, <OPpaxDISABLE> (no argument), executed by the gateway thread
//   at once, w/out the rpc thread of TETHER. the reply goes to the sender. any other ROP gets a nak if it asks for a reply

#pragma once

#include "embot_core.h"
#include "amcmj1_cubedriver.h"
#include <cstddef>
#include <cstdint>

namespace amcmj1::gateway {

    // the IDs of the Dictionary that the link knows by number: dictionary.amcmj1.cpp checks them w/ static_assert
    namespace ids {
        constexpr uint8_t cubeSTATUS {0x0B};
        constexpr uint8_t cubeSTATE {0x0C};
        constexpr uint8_t cubeMIT {0x0D};
        constexpr uint8_t paxTORQUE {0x10};
        constexpr uint8_t motorSTATE {0x11};
        constexpr uint8_t gwSTATS {0x12};
        constexpr uint8_t OPcubeENABLE {0xD1};
        constexpr uint8_t OPcubeDISABLE {0xD2};
        constexpr uint8_t OPpaxENABLE {0xD4};
        constexpr uint8_t OPpaxDISABLE {0xD5};
    }

    // the limits of the setpoint given by the Speedgoat. they can only be lower than the ranges of the motor (cube::pmax ...)
    // the initial values are prudent: they are changed w/ run<OPcubeLIMITS, {...}> when the motor is not enabled
    struct GWlimits
    {
        float kp {25.0f};                   // [0, kp]
        float kd {3.0f};                    // [0, kd]
        float pos {cube::pmax};             // [-pos, pos] rad
        float vel {10.0f};                  // [-vel, vel] rad/s
        float torque {3.0f};                // [-torque, torque] N*m

        constexpr GWlimits() = default;
        constexpr explicit GWlimits(float k, float d, float p, float v, float t) : kp(k), kd(d), pos(p), vel(v), torque(t) {}
    };

    constexpr bool valid(const GWlimits &l)     // false for NaN
    {
        return cube::inrange(l.kp, 0.0f, cube::kpmax) && cube::inrange(l.kd, 0.0f, cube::kdmax) && cube::inrange(l.pos, 0.0f, cube::pmax) &&
               cube::inrange(l.vel, 0.0f, cube::vmax) && cube::inrange(l.torque, 0.0f, cube::tmax);
    }

    static_assert(valid(GWlimits{}), "the default GWlimits must be valid");

    // the counters of the link. IDgwSTATS and the last sig<> of the state frame
    struct GWstats
    {
        uint32_t seq {0};                   // seq of the last MIT received (echo)
        uint32_t packets {0};               // valid command frames
        uint32_t discarded {0};             // MIT dropped right after the enable
        uint32_t clamped {0};               // MIT w/ a value beyond the limits: it was clamped
        uint32_t rejected {0};              // MIT w/ a NaN, or of the wrong size: it was not used
        uint32_t ignored {0};               // ROP that the link does not know
        uint32_t lost {0};                  // gaps in seq: packets that did not arrive
        uint32_t badframes {0};             // frames that are not valid TETHER frames
        uint32_t notrunning {0};            // MIT received while the motor was not enabled

        constexpr GWstats() = default;
    };

    // what the CM7 will tell about the motor that it drives. a placeholder for now: always zero
    struct MOTORstate
    {
        float pos {0.0f};                   // rad
        float vel {0.0f};                   // rad/s
        float current {0.0f};               // A
        uint8_t state {0};
        uint8_t error {0};
        uint16_t reserved {0};
        uint32_t counter {0};

        constexpr MOTORstate() = default;
    };

    // the three torques of the PX3Q (Mx, My, Mz) and the number of samples
    struct PAXtorque
    {
        float torque[3] {0.0f, 0.0f, 0.0f};
        uint32_t counter {0};

        constexpr PAXtorque() = default;
    };

    // the load of the gateway thread: is the 1 kHz affordable? IDgwLOAD
    struct GWload
    {
        uint32_t wakes {0};                 // times the thread woke up
        uint32_t wakespersecond {0};        // the same in the last second
        uint32_t cmdframes {0};             // command frames per second
        uint32_t maxcanframes {0};          // the most CAN frames drained by one canbus::rx()
        uint32_t maxloopus {0};             // the longest pass of the event handler, in microseconds

        constexpr GWload() = default;
    };

    static_assert(sizeof(GWlimits) == 20 && sizeof(GWstats) == 36 && sizeof(MOTORstate) == 20 && sizeof(PAXtorque) == 16 && sizeof(GWload) == 20,
                  "layout of the gateway structs: the host decodes them");


    // ---- the commands socket

    enum class Frame : uint8_t
    {
        bad = 0,                            // not a valid TETHER frame: nothing happened
        withmit = 1,                        // it carried a MIT: it went to the driver
        withoutmit = 2                      // no valid MIT: the last one was sent again
    };

    void setlimits(const GWlimits &limits);
    GWlimits limits();

    // a payload received on the commands socket. it calls amcmj1::cubedriver::command() or heartbeat()
    Frame oncommands(const void *payload, size_t size, embot::core::Time now);

    // the frame of the state. false if the capacity is too small. size is the number of bytes to send
    constexpr size_t statesize {16 + 5 * 40};
    bool buildstate(void *buffer, size_t capacity, size_t &size, embot::core::Time now, const amcmj1::cube::CUBEstatus &status,
                    const amcmj1::cube::CUBEstate &state, const PAXtorque &torque, const MOTORstate &motor);

    // the last MIT accepted (after the limits), for IDcubeMIT. true if it is new since the last call
    bool takelastmit(amcmj1::cube::CUBEmit &mit);

    GWstats stats();
    void resetstats();


    // ---- the rpc socket

    // enable / disable: the CubeMars. paxenable / paxdisable: the PX3Q
    enum class Request : uint8_t { none = 0, enable = 1, disable = 2, unknown = 3, paxenable = 4, paxdisable = 5 };

    struct Rpc
    {
        Request request {Request::none};
        bool ackrequired {false};
        uint8_t id {0};                     // of the ROP, for the reply
    };

    // the first ROP of a payload received on the rpc socket. Request::none if the frame is not valid
    Rpc onrpc(const void *payload, size_t size);

    // the reply to a Rpc: ack, or nak w/ the text. false if there is nothing to reply or the capacity is too small
    bool buildreply(void *buffer, size_t capacity, size_t &size, embot::core::Time now, const Rpc &rpc, bool done, const char *why);

} // namespace amcmj1::gateway
