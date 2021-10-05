/** 
 * @file ipal_hal_eth_types.h
 *
 * Glue types between IP Stack and HAL 
 */
#ifndef IPAL_HAL_ETH_TYPES_H
#define IPAL_HAL_ETH_TYPES_H

#include <stdint.h>

#warning have a look at that

/* =====================================================================================
 * TYPE DEFINITIONS
 * ===================================================================================== */
 
/**
 * COMPATIBLE (castable) to hal_eth_genericframe_t
 */
//typedef struct 
//{
//	/* Length */
//	uint16_t length;
//    
//    uint16_t dummy;
//	
//	/* Payload */
//	void *payload;
//	
//} ipal_eth_frame_t;

/**
 * COMPATIBLE (castable) to hal_eth_onframereception_t	
 * This structure is compatible with hal_eth_onframereception_t
 */
//typedef struct 
//{
//	/* Frame new - Unused */
//	ipal_eth_frame_t * (*frame_new)(uint32_t size);
//    
//	/* Move to higher layer */
//	void (*frame_movetohigherlayer)(ipal_eth_frame_t *frame);
//    
//	/* Alert higher layer */
//	void (*frame_alerthigherlayer)(void);
//	
//} ipal_eth_onframereception_t;


/** 
 * COMPATIBLE to hal_eth_cfg_t
 * This is a structure "compatible" to hal_eth_cfg_t.
 * This should allow the LWIP based stack to be used also in EMS04 board.
 * The compatibility requires the presence of the following fields (types are listed
 * in angular parenthesis)
 *		
 *   <uint8_t>                       							priority;
 *   <hal_eth_onframereception_t * COMPATIBLE>    onframerx;
 *   <hal_eth_macaddress_t 				 COMPATIBLE>    macaddress;
 *
 */
//typedef struct 
//{
//	uint8_t                         priority;
//	uint8_t                       	dummy_0;   
//    uint8_t                       	dummy_1;   
//	uint8_t                       	dummy_2;       
//	ipal_eth_onframereception_t     *onframerx;
//	uint64_t                        macaddress;
//} ipal_eth_cfg_t;

#endif

