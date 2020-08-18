 /****************************************************
 *  BDC board - iCub Facility - 2013                 *
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
extern bool HSM_PG;
extern bool HSM_F;

extern char timer_100us;
extern char timer_1ms;
extern char timer_10ms;
extern char timer_100ms;

extern bool can_led_active;
extern bool toggle_timer;

#endif
