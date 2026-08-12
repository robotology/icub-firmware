

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------


#ifndef EMBOT_HW_LWIP_H_
#define EMBOT_HW_LWIP_H_



#include "embot_core.h"
#include "embot_net_eth.h"
#include "embot_net_lwip.h"
#include "embot_core.h"

#if 0

this is the public interface for the part of embot::net::lwip that contains the hw driver,
so it must internally manage lwip calls and stm32hal calls. 
as such it is in hw and in ip world. 
#endif


namespace embot::hw::lwip {
    
    using netIF = struct netif;
        
    void netconfig(const embot::net::eth::IPconfig &ipconfig, const embot::core::Callback &onRXframe);
    void checklink();
    void checkinput();
    
    netIF *network();
    
    // it may be safely used also inside: bool embot::hw::eth:;bsp::BSP::init()
    void hal_ETH_init();
    
} // namespace embot::hw::lwip {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

