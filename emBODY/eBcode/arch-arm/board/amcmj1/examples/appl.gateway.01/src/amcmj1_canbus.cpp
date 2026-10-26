
#include "amcmj1_canbus.h"

namespace {

    constexpr std::uint8_t txcapacity {8};
    constexpr std::uint8_t rxcapacity {64};     // at 1 kHz the PX3Q sends 2000 frames/s and the CubeMars up to 1000: the gateway
                                                // thread must drain the queue at least every ~20 ms
    constexpr std::size_t maxconsumers {4};

    bool opened {false};
    amcmj1::canbus::Consumer consumers[maxconsumers] {};
    std::size_t numconsumers {0};
    amcmj1::canbus::Stats counters {};
    amcmj1::canbus::Frames rxframes {};

} // namespace

namespace amcmj1::canbus {

    bool open(const embot::core::Callback &rxsignal)
    {
        if(true == opened)
        {
            return true;
        }
        if(false == embot::hw::can::supported(bus))
        {
            embot::core::print("[canbus] the CAN is not enabled on this core: see EMBOT_ENABLE_hw_can in the bsp");
            return false;
        }
        // acceptextended: the CubeMars uses 29-bit ids. the PX3Q decoder ignores them and the CubeMars one ignores the 11-bit ones
        const embot::hw::can::Config cfg {txcapacity, rxcapacity, {}, {}, rxsignal, true};
        if((embot::hw::resOK != embot::hw::can::init(bus, cfg)) || (embot::hw::resOK != embot::hw::can::enable(bus)))
        {
            return false;
        }
        rxframes.reserve(rxcapacity);
        opened = true;
        return true;
    }

    bool attach(Consumer consumer)
    {
        if((nullptr == consumer) || (numconsumers >= maxconsumers))
        {
            return false;
        }
        for(std::size_t i = 0; i < numconsumers; ++i)
        {
            if(consumers[i] == consumer) { return true; }       // already attached
        }
        consumers[numconsumers++] = consumer;
        return true;
    }

    void rx()
    {
        if(false == opened)
        {
            return;
        }
        if(embot::hw::can::inputqueuesize(bus) >= rxcapacity)
        {
            counters.rxoverflow++;
        }
        // drain the queue in chunks of rxcapacity frames: the frames of the drivers are consumed, the others remain
        std::uint8_t remaining {embot::hw::can::inputqueuesize(bus)};
        std::uint32_t drained {0};
        while(remaining > 0)
        {
            rxframes.clear();
            embot::hw::can::Frame f {};
            while((remaining > 0) && (rxframes.size() < rxcapacity))
            {
                if(embot::hw::resOK != embot::hw::can::get(bus, f, remaining))
                {
                    remaining = 0;
                    break;
                }
                rxframes.push_back(f);
                drained++;
            }
            for(std::size_t i = 0; i < numconsumers; ++i)
            {
                consumers[i](rxframes);
            }
            counters.unknown += rxframes.size();
        }
        if(drained > 0)
        {
            counters.drains++;
            if(drained > counters.maxframes) { counters.maxframes = drained; }
        }
    }

    Stats stats() { return counters; }

} // namespace amcmj1::canbus
