
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "ipal_cfg2.h"

// external dependencies
#include "embot_hw_eth.h"

#include "embot_core.h"
    

static void ipal_app_on_fatal_error(ipal_fatalerror_t errorcode, const char * errormsg)
{
    /* Avoid compiler warnings*/ 
    errorcode = errorcode;
    errormsg = errormsg;
    volatile uint32_t n = 0;
    volatile const char * m = errormsg;
    m = errormsg;
    
//    itm_puts(errormsg);
    
    for(;;)
    {
       n = errorcode; 
       m = errormsg;
    }
}

/* ===============================================================================================
 * IPAL CONFIGURATION
 * =============================================================================================== */
 

constexpr ipal_cfg2_support_t suppcfg2 = 
{
    .arch_ipstack = ipal_ipstack_iitmod_lwip,
    .memorymodel = ipal_memmodel_static,
    .supportedmodules = ipal_cfg2_supportedflag_eth | ipal_cfg2_supportedflag_arp | 
                        ipal_cfg2_supportedflag_udp | ipal_cfg2_supportedflag_igmp |
                        ipal_cfg2_supportedflag_extfn2,
    .filler = {0, 0, 0, 0}           
};

constexpr ipal_cfg2_system_t syscfg2 = 
{
    .sys_timetick = 10*embot::core::time1millisec,
    .sys_mempoolsize = 0,
    .filler = {0, 0}        
};

constexpr ipal_cfg2_eth_t ethcfg2 =
{
    .eth_mac = IPAL_mac48addr(0x1E,0x30,0x6C,0xA2,0x45,0x5E),
    .eth_ip = IPAL_ipv4addr(10, 0, 1, 99),
    .eth_mask = IPAL_ipv4addr(255, 255, 255, 0),
    .eth_isrpriority = 0,
    .filler = {0, 0, 0, 0, 0, 0, 0},
    .hal_eth_init = embot::hw::eth::init,
    .hal_eth_enable = embot::hw::eth::enable,
    .hal_eth_disable = embot::hw::eth::disable,
    .hal_eth_sendframe = embot::hw::eth::sendframe,
    .get_frame_size = embot::hw::eth::get_frame_size,
    .get_frame = embot::hw::eth::get_frame       
};

constexpr ipal_cfg2_arp_t arpcfg2 =
{
    .arp_cachetimeout = 255*embot::core::time1second,
    .arp_retrytimeout = 0,
    .arp_cachesize = 32,
    .arp_retrymaxnum = 5,
    .arp_autonotify = 0,
    .filler = {0, 0, 0, 0, 0}
};

constexpr ipal_cfg2_udp_t udpcfg2 = 
{
    .udp_socketnum = 5,
    .filler = {0, 0, 0, 0}   
};

constexpr ipal_cfg2_igmp_t igmpcfg2 = 
{
    .igmp_groupsnum = 1,
    .filler = {0, 0, 0, 0, 0, 0, 0}    
};

void ipal_app_usr_on_rx_frame(void)
{
    static volatile uint32_t rxnum = 0;
    rxnum++;   
    embot::core::print("received an ETH frame");    
}

void ipal_app_usr_on_tx_frame(void)
{
    static volatile uint32_t txnum = 0;
    txnum++;    
}


// osal_* can also be nullptr
void * osal_mutex_new(void)
{
    static uint32_t value = 0;
    return &value;    
}

ipal_result_t osal_mutex_take(void *mutex, ipal_reltime_t tout)
{
    return ipal_res_OK;
}

ipal_result_t osal_mutex_release(void *mutex)
{
    return ipal_res_OK;
}

constexpr uint32_t tout = 10;

constexpr ipal_cfg2_extfn_t extfncfg2 = 
{
    .usr_on_fatal_error = ipal_app_on_fatal_error,
    .signal_rx_frame = ipal_app_usr_on_rx_frame,
    .signal_tx_frame = ipal_app_usr_on_tx_frame,
    .osal_mutex_new = osal_mutex_new,
    .osal_mutex_take = osal_mutex_take,
    .osal_mutex_release = osal_mutex_release,
    .osal_param_tout_forever = 10,
    .dummy = 0 
};

ipal_cfg2_t ipal_cfg2 = 
{
    .support = (ipal_cfg2_support_t*) &suppcfg2,
    .system = (ipal_cfg2_system_t*) &syscfg2,
    .eth = (ipal_cfg2_eth_t*) &ethcfg2,
    .arp = (ipal_cfg2_arp_t*) &arpcfg2,
    .udp = (ipal_cfg2_udp_t*) &udpcfg2,
    .igmp = (ipal_cfg2_igmp_t*) &igmpcfg2,
    .dhcp = NULL,
    .extfn2 = (ipal_cfg2_extfn_t*) &extfncfg2    
};


// eof - keep a line after that


