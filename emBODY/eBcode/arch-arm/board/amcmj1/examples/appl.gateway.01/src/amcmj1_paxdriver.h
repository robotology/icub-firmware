// the driver of the PaXini PX3Q joint torque sensor over CAN (manual sec. 5.3), built on thePROThandler<PX3Qprotocol>.
// it is used only by the gateway thread, so it needs no protection.
//
// the commands are asynchronous: init(), start(), stop(), deinit() only send the CAN frames and return true if the
// command is accepted. the PX3Q answers with an ack that rx() decodes later. meanwhile the state is busy, then it
// becomes the target state (initialised, running, off) or error if the ack is negative or does not arrive in time.

#pragma once

#include "embot_core.h"
#include <cstdint>

namespace amcmj1::pax {

    // the configuration of the PX3Q, given by run<OPpaxINIT, {...}>
    struct PAXcfg
    {
        uint32_t canid {0x7F};              // device id of the PX3Q, in [1, 127]. CAN IDs: 0x180, 0x200, 0x280, 0x380 + canid
        uint32_t ratehz {1000};             // streaming rate, in [4, 1000] and multiple of 4

        constexpr PAXcfg() = default;
        constexpr explicit PAXcfg(uint32_t id, uint32_t hz) : canid(id), ratehz(hz) {}
    };

    constexpr uint32_t canidmin {1};
    constexpr uint32_t canidmax {127};
    constexpr uint32_t ratemin {4};
    constexpr uint32_t ratemax {1000};

    constexpr bool valid(const PAXcfg &cfg)
    {
        return (cfg.canid >= canidmin) && (cfg.canid <= canidmax) &&
               (cfg.ratehz >= ratemin) && (cfg.ratehz <= ratemax) && (0 == (cfg.ratehz % 4));
    }

    static_assert(valid(PAXcfg{}), "the default PAXcfg must be valid");

    // the status of the PX3Q as seen by the gateway thread, written only by it
    // busy: a command was sent and its ack has not arrived yet
    enum class State : uint8_t { off = 0, initialised = 1, running = 2, error = 3, busy = 4 };
    enum class Command : uint8_t { none = 0, init = 1, deinit = 2, start = 3, stop = 4 };

    struct PAXstatus
    {
        State state {State::off};
        Command lastcommand {Command::none};
        uint16_t errors {0};
        uint32_t commands {0};              // number of commands accepted

        constexpr PAXstatus() = default;
    };

    // a reading of the PX3Q. it is also the shared variable IDpaxVALUE
    // the PX3Q measures only the three torques: torque[0..2] = Mx, My, Mz (Mz is the joint axis). force[] stays at 0
    // and is kept so that the layout of IDpaxVALUE does not change
    struct PAXvalue
    {
        float force[3] {0.0f, 0.0f, 0.0f};
        float torque[3] {0.0f, 0.0f, 0.0f};
        uint32_t counter {0};

        constexpr PAXvalue() = default;
    };

} // namespace amcmj1::pax


namespace amcmj1::paxdriver {

    // it attaches the driver to the CAN bus (amcmj1_canbus.h), which must be open. false if it is not
    bool open();

    // the commands. false if not accepted: wrong state (e.g. start before init, or busy), invalid cfg, CAN not open.
    // if accepted the state becomes busy and the outcome arrives with the ack
    bool init(const amcmj1::pax::PAXcfg &cfg);  // getVersion, setRate
    bool start();                               // start: then the stream arrives on 0x280 + id, 0x380 + id
    bool stop();                                // stop
    bool deinit();                              // stop if running, then off

    // call it after amcmj1::canbus::rx(), which gives the frames to this driver. the acks are handled there.
    // true if at least one new reading arrived since the last call: value is the latest one
    bool take(amcmj1::pax::PAXvalue &value);

    // call it at every tick of the gateway thread: it detects a missing ack
    void tick(embot::core::Time now);

    amcmj1::pax::State state();

    // diagnostics of the decoding: complete samples, lost / orphan / malformed frames. unknown frames: see amcmj1_canbus.h
    struct Stats
    {
        uint32_t samples {0};
        uint32_t lostMz {0};
        uint32_t orphanMz {0};
        uint32_t malformed {0};
        constexpr Stats() = default;
    };
    Stats stats();

    // the version string of the PX3Q (e.g. "v0.1.1"), valid after init() succeeded
    const char * version();

} // namespace amcmj1::paxdriver
