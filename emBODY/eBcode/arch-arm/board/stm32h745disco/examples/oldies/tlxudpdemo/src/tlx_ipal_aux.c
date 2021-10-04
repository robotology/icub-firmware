/**
 * @file tlx_ipal_aux.c
 *
 * Implementation of function prototypes defined in tlx_ipal_aux.h
 */
#include "tlx_ipal_aux.h"

/* ======================================================================================
 * TYPE DEFINITIONS
 * ====================================================================================== */

typedef union 
{
	ipal_ipv4addr_t value;
	
	struct
	{
		uint8_t b0;
		uint8_t b1;
		uint8_t b2;
		uint8_t b3;
	} bytes;
	
} ipal_ipv4addr_union_t;

/* ======================================================================================
 * PUBLIC FUNCTIONS
 * ====================================================================================== */
 
/*
 * tlx_ipal_get_IPAL_ipv4addr
 * See header file for details 
 */
ipal_ipv4addr_t tlx_ipal_get_IPAL_ipv4addr(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3)
{
	ipal_ipv4addr_union_t ret;
	
	ret.bytes.b0 = ip0;
	ret.bytes.b1 = ip1;
	ret.bytes.b2 = ip2;
	ret.bytes.b3 = ip3;
	
	return ret.value;
}

/* 
 * tlx_ipal_IPALaddr_to_array
 * See header file for details 
 */
void tlx_ipal_IPALaddr_to_array(ipal_ipv4addr_t ipal_addr, uint8_t *addr)
{
	ipal_ipv4addr_union_t temp_addr;
	
	temp_addr.value = ipal_addr;
	
	addr[0] = temp_addr.bytes.b0;
	addr[1] = temp_addr.bytes.b1;
	addr[2] = temp_addr.bytes.b2;
	addr[3] = temp_addr.bytes.b3;
}



