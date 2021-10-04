/** 
 * @file net_thread.h
 *
 * Net thread creation
 */
#ifndef NET_THREAD_H
#define NET_THREAD_H

/* ===============================================================================
 * MACROS
 * =============================================================================== */

/* TX event */
#define NET_THREAD_RX_EVENT	(0x01)

/* RX event */
#define NET_THREAD_TX_EVENT	(0x02)

/* ===============================================================================
 * FUNCTION PROTOTYPES 
 * =============================================================================== */
 
/**
 * Thread initializer
 */
int net_thread_init(void);

#endif
