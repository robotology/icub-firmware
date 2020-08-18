 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  Global variables                                 *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

#include <stdbool.h>

#ifndef __GLOBALS_H
#define __GLOBALS_H

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
extern bool V12board;
extern bool V12board_F;
extern bool V12motor;
extern bool V12motor_F;
extern bool HSM;
extern bool HSM_PG;
extern bool HSM_F;
extern bool DCDCrestart;

extern int filter_PB1;
extern int filter_PB2;
extern int timer_debounce;
extern int timer_restart;

extern char timer_100us;
extern char timer_1ms;
extern char timer_10ms;
extern char timer_100ms;
extern char BMON_attempt;

extern bool can_led_active;
extern bool toggle;
extern bool togglex2;
extern bool fault;
extern bool jack;

extern bool PB1_debounce;
extern bool PB1_restart;
extern bool PB2_debounce;
extern bool PB2_restart;

extern bool int_fault;

extern unsigned int ledExti; 

#endif
