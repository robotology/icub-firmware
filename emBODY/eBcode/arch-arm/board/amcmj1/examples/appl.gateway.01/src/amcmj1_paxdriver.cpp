
#include "amcmj1_paxdriver.h"
#include <cmath>
#include <string>

namespace {

    enum class State : uint8_t { off, initialised, running };

    State state {State::off};
    amcmj1::pax::PAXcfg config {};
    embot::core::Time lasttx {0};
    uint32_t counter {0};

    // it prints the CAN frame that the real driver would send
    void canframe(const char *what, uint32_t a = 0, uint32_t b = 0)
    {
        embot::core::print(std::string("[paxdriver] CAN tx: ") + what + " " + std::to_string(a) + " " + std::to_string(b));
    }

}

namespace amcmj1::paxdriver {

    bool init(const amcmj1::pax::PAXcfg &cfg)
    {
        if(State::running == state)
        {
            return false;                   // stop it first
        }
        if(failvalue == cfg.par3)
        {
            embot::core::print("[paxdriver] init: the PX3Q does not answer (fake failure)");
            state = State::off;
            return false;
        }
        canframe("set par1, par2", cfg.par1, cfg.par2);
        canframe("set par3", cfg.par3);
        canframe("set tx period (ms)", cfg.txperiodms);
        config = cfg;
        state = State::initialised;
        return true;
    }

    bool deinit()
    {
        if(State::running == state)
        {
            canframe("stop tx");
        }
        canframe("reset");
        state = State::off;
        return true;
    }

    bool start()
    {
        if(State::initialised != state)
        {
            return false;                   // init first, or already running
        }
        canframe("start tx");
        lasttx = 0;
        state = State::running;
        return true;
    }

    bool stop()
    {
        if(State::running != state)
        {
            return false;
        }
        canframe("stop tx");
        state = State::initialised;
        return true;
    }

    bool tick(embot::core::Time now, amcmj1::pax::PAXvalue &value)
    {
        const embot::core::Time period {static_cast<embot::core::Time>(config.txperiodms) * 1000};
        if((State::running != state) || ((0 != lasttx) && ((now - lasttx) < period)))
        {
            return false;
        }
        lasttx = now;
        // fake readings: a slow sine on fz and tz, so they are easy to recognise in the stream
        const float s {std::sin(6.2831853f * static_cast<float>(now % 2000000) / 2000000.0f)};
        value = amcmj1::pax::PAXvalue {};
        value.force[2] = 10.0f * s;
        value.torque[2] = 0.5f * s;
        value.counter = ++counter;
        return true;
    }

} // namespace amcmj1::paxdriver
