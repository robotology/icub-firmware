#include "amcmj1_posdriver.h"

#include <cmath>

// 1: no encoder, the position is emulated (a sine). 0: embot::hw::encoder (it needs EMBOT_ENABLE_hw_encoder in the bsp)
#define AMCMJ1_POSDRIVER_EMULATED 1

#if (0 == AMCMJ1_POSDRIVER_EMULATED)
#include "embot_hw_encoder.h"
#if !defined(EMBOT_ENABLE_hw_encoder)
#error AMCMJ1_POSDRIVER_EMULATED is 0 but the bsp has no EMBOT_ENABLE_hw_encoder
#endif
#endif

namespace amcmj1::posdriver {

    // no constructor: it is constant-initialised
    struct Data
    {
        amcmj1::pos::POSstate state {};
        bool started {false};               // init() went well
        bool reading {false};               // a read was started and its callback has not come yet
    };

    Data data {};

    // set by the callback of the read (IRQ of the SPI), cleared by tick() (tGATEWAY)
    volatile bool readover {false};

#if (0 == AMCMJ1_POSDRIVER_EMULATED)

    constexpr embot::hw::ENCODER encoder {embot::hw::ENCODER::one};

    // the chip of the joint: TO BE SET
    constexpr embot::hw::encoder::Type chip {embot::hw::encoder::Type::chipAS5045};

    // the position in deg from the value of the chip. TO BE CHECKED w/ the chip: here the full 16 bits are one turn
    constexpr float todeg(embot::hw::encoder::POS raw)
    {
        return static_cast<float>(raw) * (360.0f / 65536.0f);
    }

    void onread(void *) { readover = true; }

    bool init()
    {
        embot::hw::encoder::Config cfg {};
        cfg.type = chip;
        data.started = (embot::hw::resOK == embot::hw::encoder::init(encoder, cfg));
        return data.started;
    }

    void tick(embot::core::Time /*now*/)
    {
        if(false == data.started)
        {
            return;
        }
        if(true == data.reading)
        {
            if(false == readover)
            {
                data.state.errors++;                // still in progress after 1 ms: wait for it, no new read
                return;
            }
            readover = false;
            data.reading = false;
            embot::hw::encoder::POS raw {0};
            if(embot::hw::resOK == embot::hw::encoder::getValue(encoder, raw))
            {
                data.state.raw = raw;
                data.state.position = todeg(raw);
                data.state.counter++;
                data.state.flags |= amcmj1::pos::flags::valid;
            }
            else
            {
                data.state.errors++;
            }
        }
        if(embot::hw::resOK == embot::hw::encoder::startRead(encoder, embot::core::Callback {onread, nullptr}))
        {
            data.reading = true;
        }
        else
        {
            data.state.errors++;
        }
    }

#else

    bool init()
    {
        data.started = true;
        data.state.flags = amcmj1::pos::flags::emulated;
        return true;
    }

    void tick(embot::core::Time now)
    {
        if(false == data.started)
        {
            return;
        }
        constexpr embot::core::Time period {10 * embot::core::time1second};
        const float phase {static_cast<float>(now % period) / static_cast<float>(period)};
        data.state.position = 30.0f * std::sin(6.2831853f * phase);
        data.state.raw = static_cast<uint16_t>(static_cast<int32_t>(data.state.position * (65536.0f / 360.0f)) & 0xFFFF);
        data.state.counter++;
        data.state.flags = amcmj1::pos::flags::emulated | amcmj1::pos::flags::valid;
    }

#endif

    amcmj1::pos::POSstate state()
    {
        amcmj1::pos::POSstate s {data.state};
        if(true == data.reading)
        {
            s.flags |= amcmj1::pos::flags::reading;
        }
        return s;
    }

} // namespace amcmj1::posdriver
