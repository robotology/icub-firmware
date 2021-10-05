
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef __IPAL_HAL_ETH_H_
#define __IPAL_HAL_ETH_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       ipal_hal_eth.h
    @brief      This header file implements public interface between the ipal and an hardware abstraction layer 
    @author     marco.accame@iit.it
    @date       29 sept 2021
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "ipal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty section

// - declaration of public user-defined types ------------------------------------------------------------------------- 


// this is required so that we can keep backard-compatibility w/ hal2 
// which was shaped to the needs of tcpnet
// this type is memory compatible w/ hal_eth_frame_t and hl_eth_frame_t
// it was done in such a way so that it was memory compatible w/ tcpnet
typedef struct 
{
    uint16_t            length;
    uint16_t            dummy;
    void*               payload;	
} ipal_hal_eth_frame_t; __ipalVERIFYsizeof(ipal_hal_eth_frame_t, 8)

// this is required so that we can keep backard-compatibility w/ hal2 
// which was shaped to the needs of tcpnet
// this is required so that it is memory compatible w/ hal_eth_onframereception_t
typedef struct 
{
    ipal_hal_eth_frame_t* (*frame_new)(uint32_t size);  // unused by lwip
	void (*frame_movetohigherlayer)(ipal_hal_eth_frame_t *frame);    
	void (*frame_alerthigherlayer)(void);    
} ipal_hal_eth_onframereception_t; __ipalVERIFYsizeof(ipal_hal_eth_onframereception_t, 12)

// this is required so that we can keep backard-compatibility w/ hal2 
// which was shaped to the needs of tcpnet
// this is required so that it is memory compatible w/ hal_eth_cfg_t
typedef struct
{
    uint8_t                 priority;
	uint8_t                 dummy[3];        
	ipal_hal_eth_onframereception_t *onframerx;
    ipal_mac48addr_t        macaddress;
} ipal_hal_eth_cfg_t; __ipalVERIFYsizeof(ipal_hal_eth_cfg_t, 16)


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section

 
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

