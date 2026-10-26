// the CAN bus of amcmj1, shared by the drivers of the devices on it: the PX3Q (standard ids) and the CubeMars (extended ids).
// it is used only by the gateway thread, so it needs no protection.
//
// it owns embot::hw::can: it inits it once (also for extended ids), drains the rx queue and gives the frames to the drivers
// in the order they were attached. every driver removes from the vector the frames it decodes; what remains is counted as
// unknown. the drivers send with embot::hw::can::put() and transmit() by themselves.

#pragma once

#include "embot_core.h"
#include "embot_hw_can.h"
#include <cstdint>
#include <vector>

namespace amcmj1::canbus {

    using Frames = std::vector<embot::hw::can::Frame>;

    // a driver gives a function that decodes the frames of its device and erases them from the vector
    using Consumer = void (*)(Frames &frames);

    // it inits the CAN bus (1 Mbps, standard + extended ids). rxsignal is called by the CAN rx IRQ handler: it must only wake
    // the gateway thread, which then calls rx(). the first call decides: the other calls just return true.
    // false if the CAN is not available on this core
    bool open(const embot::core::Callback &rxsignal);

    // the drivers attach their consumer once, in open(). at most 4
    bool attach(Consumer consumer);

    // call it when rxsignal wakes the gateway thread: it drains the rx queue and calls the consumers
    void rx();

    struct Stats
    {
        uint32_t unknown {0};               // frames on the bus that no driver decodes
        uint32_t rxoverflow {0};            // times the rx queue was found full
        uint32_t drains {0};                // calls of rx() that found frames
        uint32_t maxframes {0};             // the most frames drained by one call of rx(): the load of the gateway thread
        constexpr Stats() = default;
    };
    Stats stats();

    constexpr embot::hw::CAN bus {embot::hw::CAN::one};

} // namespace amcmj1::canbus
