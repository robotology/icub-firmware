/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_instr_ScopeGPIO.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

embot::instr::ScopeGPIO::ScopeGPIO(const Config &cfg)
    : config(cfg)
{
    stateoff = (embot::hw::gpio::State::SET == config.on) ? embot::hw::gpio::State::RESET : embot::hw::gpio::State::SET;
    valid = config.isvalid() && (embot::hw::resOK == embot::hw::gpio::init(config.gpio, config.gpiocfg));
    off();
}

void embot::instr::ScopeGPIO::on()
{
    if(true == valid)
    {
        embot::hw::gpio::set(config.gpio, config.on);
    }
}

void embot::instr::ScopeGPIO::off()
{
    if(true == valid)
    {
        embot::hw::gpio::set(config.gpio, stateoff);
    }
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
