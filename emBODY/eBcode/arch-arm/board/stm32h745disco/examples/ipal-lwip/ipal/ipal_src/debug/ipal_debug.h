/**
 * @file ipal_debug.h
 */
#ifndef IPAL_DEBUG_H
#define IPAL_DEBUG_H

#include <stdbool.h>
#include <stdint.h>

/* ==============================================================================
 * SYMBOLS EXTERNALIZATION
 * ============================================================================== */ 

extern bool dbg_ipal_irqrx_frame_detected;
extern bool dbg_ipal_rx_frame_detected;
extern bool dbg_etharp_tmr;
extern bool dbg_ipal_timetick;
extern volatile uint32_t dbg_rcv_frame;

#endif

