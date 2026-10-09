

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __AMCMJ1_CTRL_H_
#define __AMCMJ1_CTRL_H_

#include "embot_core.h"
#include "embot_core_binary.h"          // pos2mask
#include "embot_os_Thread.h"
#include "embot_app_eth_SharedVariables.h"

#if 0

the interface of the ctrl thread of amcmj1 w/ the other threads.
- the ctrl thread (app.gtw.04.cpp) sets ctrl::thread and ctrl::sharedvars, and waits for the events
- the operations of the Dictionary (dictionary.amcmj1.cpp) run in the rpc thread: they call ctrl::signal() to wake up the
  ctrl thread w/ one of the events, and return

#endif

// ---- amcmj1::ctrl: the events of the ctrl thread, and how the others signal it

namespace amcmj1::ctrl {

    using embot::core::binary::mask::pos2mask;
    constexpr embot::os::Event evtTick {pos2mask<embot::os::Event>(0)};
    constexpr embot::os::Event evtPAXinit {pos2mask<embot::os::Event>(1)};
    constexpr embot::os::Event evtPAXdeinit {pos2mask<embot::os::Event>(2)};
    constexpr embot::os::Event evtPAXstart {pos2mask<embot::os::Event>(3)};
    constexpr embot::os::Event evtPAXstop {pos2mask<embot::os::Event>(4)};

    // set when the ctrl thread is created
    inline embot::os::EventThread *thread {nullptr};

    // taken once in the startup of the ctrl thread, after theTETHERservice::initialise()
    inline embot::app::eth::SharedVariables *sharedvars {nullptr};

    inline bool signal(embot::os::Event evt)
    {
        if(nullptr == thread)
        {
            return false;                   // -> nak: the ctrl thread is not running
        }
        thread->setEvent(evt);
        return true;
    }

} // namespace amcmj1::ctrl


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

