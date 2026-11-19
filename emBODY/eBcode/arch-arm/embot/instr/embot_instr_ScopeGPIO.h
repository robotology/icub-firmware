/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_INSTR_SCOPEGPIO_H_
#define __EMBOT_INSTR_SCOPEGPIO_H_

// a Scope on a pin: on() puts it in Config::on, off() in the other state. to see the code on an oscilloscope or a logic
// analyser. it needs embot::hw::gpio (embot_hw_gpio.cpp in the project).
//
// it comes from embot::app::scope::SignalGPIO (embot/app/embot_app_scope.h). unlike that one, it uses Config::gpiocfg.

#include "embot_instr.h"
#include "embot_hw_gpio.h"

namespace embot::instr {

    class ScopeGPIO : public Scope
    {
    public:

        struct Config
        {
            embot::hw::GPIO gpio {};
            embot::hw::gpio::State on {embot::hw::gpio::State::SET};
            embot::hw::gpio::Config gpiocfg {embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::medium};

            constexpr Config() = default;
            constexpr explicit Config(embot::hw::GPIO g, embot::hw::gpio::State o, const embot::hw::gpio::Config &gc)
                : gpio(g), on(o), gpiocfg(gc) {}

            constexpr bool isvalid() const { return gpio.isvalid() && gpiocfg.isvalid(); }
        };

        // it initialises the pin and puts it off. w/ a Config not valid, on() and off() do nothing
        explicit ScopeGPIO(const Config &cfg);
        ~ScopeGPIO() override = default;

        void on() override;
        void off() override;

    private:
        Config config {};
        embot::hw::gpio::State stateoff {embot::hw::gpio::State::RESET};
        bool valid {false};
    };

} // namespace embot::instr


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
