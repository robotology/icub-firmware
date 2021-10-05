/**
 * @file ipal_debug.c
 */
#include "ipal_debug.h"
#include <stdint.h>

/* ==============================================================================
 * GLOBAL SYMBOLS
 * ============================================================================== */ 
 
/* RX frame detected by interrupt handler */ 
bool dbg_ipal_irqrx_frame_detected = false;

/* RX frame detected by foreground */
bool dbg_ipal_rx_frame_detected = false;

/* ARP timeout expired, etharp_tmr is invoked to refresh cache */
bool dbg_etharp_tmr = false;

/* IPAL timetick increment */
bool dbg_ipal_timetick = false;

/* Count number of received frame */
volatile uint32_t dbg_rcv_frame = 0;

