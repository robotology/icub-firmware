

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __AMCMJ1_GATEWAY_H_
#define __AMCMJ1_GATEWAY_H_

#include "embot_core.h"
#include "embot_core_binary.h"          // pos2mask
#include "embot_os_Thread.h"
#include "embot_app_eth_SharedVariables.h"

#if 0

the interface of the gateway thread (tGATEWAY) of amcmj1 w/ the other threads.
- the gateway thread (appl.gateway.01.cpp) sets gateway::thread and gateway::sharedvars, and waits for the events
- the operations of the Dictionary (dictionary.amcmj1.cpp) run in the rpc thread of TETHER: they call gateway::signal() to
  wake up the gateway thread w/ one of the events, and return
- the sockets of the Speedgoat link (commands, rpc, state) belong to the gateway thread: their rx callbacks only set an event

#endif

// ---- amcmj1::gateway: the events of the gateway thread, and how the others signal it

namespace amcmj1::gateway {

    using embot::core::binary::mask::pos2mask;
    constexpr embot::os::Event evtTick {pos2mask<embot::os::Event>(0)};
    constexpr embot::os::Event evtPAXinit {pos2mask<embot::os::Event>(1)};
    constexpr embot::os::Event evtPAXdeinit {pos2mask<embot::os::Event>(2)};
    constexpr embot::os::Event evtPAXstart {pos2mask<embot::os::Event>(3)};
    constexpr embot::os::Event evtPAXstop {pos2mask<embot::os::Event>(4)};
    constexpr embot::os::Event evtCANrx {pos2mask<embot::os::Event>(5)};       // from the CAN rx IRQ: frames to drain (PX3Q and CubeMars)
    constexpr embot::os::Event evtCUBEinit {pos2mask<embot::os::Event>(6)};
    constexpr embot::os::Event evtCUBEdeinit {pos2mask<embot::os::Event>(7)};
    constexpr embot::os::Event evtCUBEstart {pos2mask<embot::os::Event>(8)};
    constexpr embot::os::Event evtCUBEstop {pos2mask<embot::os::Event>(9)};
    constexpr embot::os::Event evtCMDrx {pos2mask<embot::os::Event>(10)};      // a packet on the commands socket: the clock of the MIT
    constexpr embot::os::Event evtRPCrx {pos2mask<embot::os::Event>(11)};      // a packet on the rpc socket of the gateway
    constexpr embot::os::Event evtCUBEenable {pos2mask<embot::os::Event>(12)}; // OPcubeENABLE: init if needed, then start
    constexpr embot::os::Event evtCUBEdisable {pos2mask<embot::os::Event>(13)};// OPcubeDISABLE: stop
    constexpr embot::os::Event evtCUBElimits {pos2mask<embot::os::Event>(14)}; // OPcubeLIMITS: vCubeLimits has changed
    constexpr embot::os::Event evtPAXenable {pos2mask<embot::os::Event>(15)};  // OPpaxENABLE: init if needed, then start
    constexpr embot::os::Event evtPAXdisable {pos2mask<embot::os::Event>(16)}; // OPpaxDISABLE: stop
    constexpr embot::os::Event evtPOStick {pos2mask<embot::os::Event>(17)};    // every 1 ms: the read of the position sensor

    // set when the gateway thread is created
    inline embot::os::EventThread *thread {nullptr};

    // taken once in the startup of the gateway thread, after theTETHERservice::initialise()
    inline embot::app::eth::SharedVariables *sharedvars {nullptr};

    inline bool signal(embot::os::Event evt)
    {
        if(nullptr == thread)
        {
            return false;                   // -> nak: the gateway thread is not running
        }
        thread->setEvent(evt);
        return true;
    }

} // namespace amcmj1::gateway


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

