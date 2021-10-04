/**
 * @file ipal_hid.h
 *
 * Main include file for "hid" modules.
 *
 * @author Tecnologix
 */
#ifndef IPAL_HID_H
#define IPAL_HID_H

#include "ipal_modules.h"

/* ==========================================================================================
 * INCLUSIONS
 * ========================================================================================== */

#ifdef IPAL_USE_ARP
	#include "ipal_arp_hid.h"
#endif
    
#ifdef IPAL_USE_BASE
	#include "ipal_base_hid.h"
#endif

#ifdef IPAL_USE_DHCP
	#include "ipal_dhcp_hid.h"
#endif

#ifdef IPAL_USE_ETH
	#include "ipal_eth_hid.h"
#endif

#ifdef IPAL_USE_FTP
	#include "ipal_ftp_hid.h"
#endif

#ifdef IPAL_USE_IGMP
	#include "ipal_igmp_hid.h"
#endif

#ifdef IPAL_USE_SYS
	#include "ipal_sys_hid.h"
#endif

#ifdef IPAL_USE_TCP
	#include "ipal_tcp_hid.h"
#endif

#ifdef IPAL_USE_TELNET
	#include "ipal_telnet_hid.h"
#endif

#ifdef IPAL_USE_TFTP
	#include "ipal_tftp_hid.h"
#endif

#ifdef IPAL_USE_UDP
	#include "ipal_udp_hid.h"
#endif

#ifdef IPAL_USE_FIFO
	#include "ipal_fifo_hid.h"
#endif


#endif

