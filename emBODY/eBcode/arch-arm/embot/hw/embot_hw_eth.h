
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ETH_H_
#define __EMBOT_HW_ETH_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_gpio.h"
#include <array>

#include "ipal.h"

#if 0
 

#endif  

namespace embot::hw::eth {
    
    enum class PORT : uint8_t { one = 0, two = 1, three = 2, none = 31, maxnumberof = 3 };
    enum class ERR : uint8_t { crc = 0, unicast = 1, RxByteCnt = 2, TxByteCnt = 3};

    // standard api
    bool supported(embot::hw::EtH b);    
    bool initialised(embot::hw::EtH b);    
    result_t init(embot::hw::EtH b);
//    result_t deinit(embot::hw::ETH b);
    
    // these are the functions required by IPAL
    // which need to go inside ipal_cfg2_eth_t
    // so far we dont add any extra functions
    // we may later on add also PHY functions in here (e.g., link status, crc errors).
    
    ipal_result_t init(ipal_hal_eth_cfg_t *cfg);
    ipal_result_t enable(void);
    ipal_result_t disable(void);
    ipal_result_t sendframe(ipal_hal_eth_frame_t *frame);
    uint16_t get_frame_size(void);
    void get_frame(uint16_t length, uint8_t* frame);
    
    uint64_t ipal_get_mac(void);   

    bool islinkup(embot::hw::PHY phy);
    
    uint64_t getnumberoferrors(embot::hw::PHY phy, ERR e);
    
} // namespace embot::hw::eth {
    
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


