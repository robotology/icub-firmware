
// a fake driver of the PaXini PX3Q: it prints the CAN frames it would send and it produces fake readings.
// it is used only by the ctrl thread, so it needs no protection

#pragma once

#include "embot_core.h"
#include <cstdint>

namespace amcmj1::pax {

    // the configuration of the PX3Q, given by run<OPpaxINIT, {...}>
    struct PAXcfg
    {
        uint32_t par1 {0};
        uint32_t par2 {0};
        uint32_t par3 {0};
        uint32_t txperiodms {0};            // the PX3Q transmits every txperiodms

        PAXcfg() = default;
        explicit PAXcfg(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t tx)
            : par1(p1), par2(p2), par3(p3), txperiodms(tx) {}
    };

    constexpr uint32_t txperiodmin {1};
    constexpr uint32_t txperiodmax {1000};

    // the status of the PX3Q as seen by the ctrl thread, written only by it
    enum class State : uint8_t { off = 0, initialised = 1, running = 2, error = 3 };
    enum class Command : uint8_t { none = 0, init = 1, deinit = 2, start = 3, stop = 4 };

    struct PAXstatus
    {
        State state {State::off};
        Command lastcommand {Command::none};
        uint16_t errors {0};
        uint32_t commands {0};              // number of commands executed

        PAXstatus() = default;
    };

    // a reading of the PX3Q: force and torque. it is also the shared variable IDpaxVALUE
    struct PAXvalue
    {
        float force[3] {0.0f, 0.0f, 0.0f};
        float torque[3] {0.0f, 0.0f, 0.0f};
        uint32_t counter {0};

        PAXvalue() = default;
    };

} // namespace amcmj1::pax


namespace amcmj1::paxdriver {

    // the commands. false if not allowed in the current state (e.g. start before init) or if the PX3Q fails
    bool init(const amcmj1::pax::PAXcfg &cfg);
    bool deinit();
    bool start();
    bool stop();

    // call it at every tick of the ctrl thread. true if a new reading is in value (every txperiodms, only if started)
    bool tick(embot::core::Time now, amcmj1::pax::PAXvalue &value);

    // the fake can fail on purpose: init() fails if par3 == failvalue. to test the error path from the PC
    constexpr uint32_t failvalue {0xDEAD};

} // namespace amcmj1::paxdriver
