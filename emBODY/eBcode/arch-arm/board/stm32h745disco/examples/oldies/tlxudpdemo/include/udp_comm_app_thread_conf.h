/** 
 * @file udp_comm_app_thread_conf.h
 *
 * UDP Communication App example configuration
 */
#ifndef UDP_COMM_APP_THREAD_CONF_H
#define UDP_COMM_APP_THREAD_CONF_H

/* This (local) Configuration */
#define TLX_UDP_PORT							(3333)

/* Remote configuration */
#define TLX_UDP_REMOTE_ADDR_TYPE	NET_ADDR_IP4
#if defined(USE_PC104_ADDRESSING)
#define TLX_UDP_REMOTE_IP0				10
#define TLX_UDP_REMOTE_IP1				0
#define TLX_UDP_REMOTE_IP2				1
#define TLX_UDP_REMOTE_IP3				104
#define TLX_UDP_REMOTE_PORT				11111
#define TLX_UDP_REMOTE_PORT_TXTO    	3333
#warning using USE_PC104_ADDRESSING
#else
#define TLX_UDP_REMOTE_IP0				192
#define TLX_UDP_REMOTE_IP1				168
#define TLX_UDP_REMOTE_IP2				0
#define TLX_UDP_REMOTE_IP3				235
#define TLX_UDP_REMOTE_PORT				2000
#endif

#define TLX_UDP_REMOTE_ADDRESS		{TLX_UDP_REMOTE_ADDR_TYPE, 	\
																	 TLX_UDP_REMOTE_PORT,				\
																	 TLX_UDP_REMOTE_IP0,				\
																	 TLX_UDP_REMOTE_IP1,				\
																	 TLX_UDP_REMOTE_IP2,				\
																	 TLX_UDP_REMOTE_IP3}
	


#endif
