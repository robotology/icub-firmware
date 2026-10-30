// the position sensor of the joint (sensor_position for the Speedgoat): an SPI encoder read by the amcmj1 (CM4) through
// embot::hw::encoder. it is used only by the gateway thread (tGATEWAY), so it needs no protection, except the flag that the
// callback of the read sets (it runs in the IRQ of the SPI).
//
// the reading: tick() is called every 1 ms (evtPOStick of a 1 ms timer). it starts a non-blocking read w/
// embot::hw::encoder::startRead(); the SPI transaction lasts a few us and its end calls back onread(), which only raises a
// flag. at the next tick() the value is taken w/ embot::hw::encoder::getValue(), converted, and a new read is started.
// so the value is at most 1 ms old. a read that is not over at the next tick counts as an error and is not restarted.
//
// what the gateway tells: sig<IDposSTATE, POSstate> in every state frame (slot 5), and the variable IDposSTATE.
//
// PLACEHOLDER: the encoder of the joint is not wired yet. w/ AMCMJ1_POSDRIVER_EMULATED 1 (in amcmj1_posdriver.cpp) the
// position is a slow sine (+-30 deg, 10 s), so that the Speedgoat side can be tested. to use the encoder:
//   1. the bsp: EMBOT_ENABLE_hw_encoder and EMBOT_ENABLE_hw_spi_one in cfg/embot_hw_bsp_amcmj1_custom_config.h (SPI1 is the
//      encoder one of the amcmj1, see embot_hw_encoder_bsp_amcmj1.cpp)
//   2. the chip: POScfg::type below (AS5045, MA730, MB049) and the conversion in tick() (bits of the chip)
//   3. AMCMJ1_POSDRIVER_EMULATED 0

#pragma once

#include "embot_core.h"
#include <cstdint>

namespace amcmj1::pos {

    namespace flags {
        constexpr uint8_t valid {0x01};             // position holds a value read (or emulated)
        constexpr uint8_t emulated {0x02};          // the value is emulated: no encoder yet
        constexpr uint8_t reading {0x04};           // a read is in progress (only a snapshot)
    }

    // what the sensor tells. sig<IDposSTATE> of the state frame. 16 bytes
    struct POSstate
    {
        float position {0.0f};              // deg
        uint32_t counter {0};               // readings done (also the emulated ones)
        uint32_t errors {0};                // reads that failed or were not over after 1 ms
        uint16_t raw {0};                   // the last value of the chip (embot::hw::encoder::POS)
        uint8_t flags {0};                  // pos::flags
        uint8_t reserved {0};

        constexpr POSstate() = default;
    };

    static_assert(sizeof(POSstate) == 16, "layout of POSstate: the host decodes it");

} // namespace amcmj1::pos


namespace amcmj1::posdriver {

    // once, in the startup of the gateway thread. false if the encoder does not initialise (the state says not valid)
    bool init();

    // every 1 ms: takes the value of the last read and starts the next one
    void tick(embot::core::Time now);

    // the last state, for the state frame and the variable IDposSTATE
    amcmj1::pos::POSstate state();

} // namespace amcmj1::posdriver
