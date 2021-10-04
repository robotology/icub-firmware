/**
 * @file tlx_ipal_aux.h
 *
 * Some uxiliary functions which helps to access IPAL data structures from user-specifi application
 */
#ifndef TLX_IPAL_AUX_H
#define TLX_IPAL_AUX_H

#include "ipal.h"
#include <stdint.h>

/* ======================================================================================
 * FUNCTIONS PROTOTYPES
 * ====================================================================================== */
 
/** 
 * Gets ipal IPV4ADDR from four uint8_t address components111
 */
ipal_ipv4addr_t tlx_ipal_get_IPAL_ipv4addr(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3);

/** 
 * From IPAL address gets "standard" 43 bytes IP array
 */
void tlx_ipal_IPALaddr_to_array(ipal_ipv4addr_t ipal_addr, uint8_t *addr);

#endif
