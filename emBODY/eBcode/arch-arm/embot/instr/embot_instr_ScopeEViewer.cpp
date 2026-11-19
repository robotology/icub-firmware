/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_instr_ScopeEViewer.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "eventviewer.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

static_assert(sizeof(evEntityId_t) == sizeof(uint8_t), "ScopeEViewer keeps the IDs of the eventviewer in uint8_t");

embot::instr::ScopeEViewer::ScopeEViewer(const Config &cfg)
{
    valid = cfg.isvalid();
    if(true == valid)
    {
        eventviewer_init();
        evid = static_cast<uint8_t>(ev_ID_first_usrdef + embot::core::tointegral(cfg.label));
        prev = ev_ID_idle;
        eventviewer_load(evid, cfg.name);
    }
}

void embot::instr::ScopeEViewer::on()
{
    if(true == valid)
    {
        prev = eventviewer_switch_to(evid);
    }
}

void embot::instr::ScopeEViewer::off()
{
    if(true == valid)
    {
        eventviewer_switch_to(prev);
    }
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
