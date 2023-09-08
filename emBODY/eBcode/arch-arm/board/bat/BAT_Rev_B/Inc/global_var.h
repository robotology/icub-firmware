/****************************************************
*  BAT board - iCub Facility - 2018                 *
*  Global variables configuration                   *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"
#include "define.h"

#ifndef __GLOBAL_VAR_H
#define __GLOBAL_VAR_H

// ----------------------------------------------------------------------------
// Firmware Version
// ----------------------------------------------------------------------------
extern char Firmware_vers;
extern char Revision_vers;
extern char Build_number;

// ----------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------

extern uint32_t vtol;
extern uint32_t vhyst;
extern uint16_t Battery_charge;
extern uint32_t VTH[7];
extern uint16_t Battery_high;
extern uint16_t Battery_low;

extern uint8_t PB1_debounce;
extern uint8_t PB1_restart;
extern uint8_t PB1_status;
extern uint8_t PB1_counter_debounce;

extern uint8_t PB2_debounce;
extern uint8_t PB2_restart;
extern uint8_t PB2_status;
extern uint8_t PB2_counter_debounce;

extern uint8_t int_fault;
extern uint8_t filter_fault_gpio;
extern uint8_t fault_button;

extern uint8_t timer_debounce;
extern uint8_t timer_restart;
extern uint16_t timer_delay_dcdc;
extern uint16_t timer_delay_board;
extern uint16_t timer_delay_motor;
extern uint16_t timer_delay_board_max;
extern uint16_t timer_delay_motor_max;
extern uint16_t timer_delay_dcdc_max;

extern uint32_t blink_ds;
extern uint8_t toggle_1s;
extern uint8_t toggle_100ms;

extern uint16_t I_V12board_MAX;
extern uint16_t I_V12motor_MAX;
extern uint16_t I_HSM_MAX;
extern uint16_t timer_fault_board;
extern uint16_t timer_fault_motors;
extern uint16_t timer_fault_HSM;
extern uint16_t timer_mosfet_broken;
extern uint16_t timer_rearmHSM;
extern uint16_t rearmHSM_timevalue;

extern uint8_t V12board;
extern uint8_t V12board_F;
extern uint8_t V12motor;
extern uint8_t V12motor_F;
extern uint8_t HSM_broken;
extern uint8_t HSM;
extern uint8_t DCrestart;

extern uint8_t V12board_bdc;
extern uint8_t V12motor_bdc;

extern uint8_t state_fsm;
extern uint8_t state_fsm_boards;
extern uint8_t state_fsm_motors;
extern uint8_t state_fsm_prev;
extern uint8_t state_fsm_boards_prev;
extern uint8_t state_fsm_motors_prev;
extern uint8_t timerFSM;
extern uint8_t timerFSM_boards;
extern uint8_t timerFSM_motors;

extern uint8_t DCDC_status_A;
extern uint8_t DCDC_status_B;
extern uint8_t DCDC_ctrl;
extern uint8_t DCDC_status;
extern uint8_t HSM_PG;
extern uint8_t HSM_F;

extern __IO ITStatus UartReady;
extern uint16_t timeout;
extern uint16_t time_delay;

extern uint8_t can_message;

#endif
