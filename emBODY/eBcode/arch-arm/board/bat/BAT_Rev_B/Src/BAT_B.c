/****************************************************
*  BAT board - iCub Facility - 2018                 *
*  Global variables configuration                   *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"
#include "BAT_B.h"

char Firmware_vers = 1;
char Revision_vers = 3;
char Build_number  = 3;

uint32_t vtol=100;  // voltage tolerance for hysteresis
uint32_t vhyst=0;    // voltage hysteresis
uint16_t Battery_charge;

uint8_t PB1_debounce    = 0;
uint8_t PB1_restart     = 0;
uint8_t PB1_status      = 1;
uint8_t PB1_counter_debounce;

uint8_t PB2_debounce    = 0;
uint8_t PB2_restart     = 0;
uint8_t PB2_status      = 1;
uint8_t PB2_counter_debounce;

uint8_t int_fault         = 0;
uint8_t filter_fault_gpio = 0;
uint8_t fault_button      = 0;

uint8_t timer_debounce  = 10;             // 10x (10ms timer) = 100ms
uint8_t timer_restart   = 100;            // 100x (10ms timer) = 1s
uint16_t timer_delay_dcdc  = 0;           // delay between switching on V12board and then V12motor
uint16_t timer_delay_board = 0;
uint16_t timer_delay_motor = 0;
uint16_t timer_delay_board_max = 500;
uint16_t timer_delay_motor_max = 10000;
uint16_t timer_delay_dcdc_max  = 10000;   // delay between switching on regulator

uint32_t blink_ds=100;
uint8_t toggle_1s = 0;
uint8_t toggle_100ms = 0;

uint16_t I_V12board_MAX      = 10000;    // threshold in mA
uint16_t I_V12motor_MAX      = 10000;    // threshold in mA
uint16_t I_HSM_MAX           = 38000;    // threshold in mA
uint16_t timer_fault_board   = 0;
uint16_t timer_fault_motors  = 0;
uint16_t timer_fault_HSM     = 0;
uint16_t timer_mosfet_broken = 0;
uint16_t timer_rearmHSM      = 0;
uint16_t rearmHSM_timevalue  = 20000;    // (value in second) = rearmHSM_timevalue * (loop in ms where is "timer_rearmHSM++")

uint8_t V12board      = 0;		// DCDC board control
uint8_t V12board_F    = 0;		// fault
uint8_t V12motor      = 0;		// DCDC motor control
uint8_t V12motor_F    = 0;		// fault
uint8_t HSM_broken    = 0;		// HSM transistors broken
uint8_t HSM           = 0;		// HSM control
uint8_t HSM_PG        = 0;
uint8_t HSM_F         = 0;
uint8_t DCrestart     = 0;

//HSM Hw-Sw Fault flags
uint8_t HSM_HW_F         = 0;
uint8_t HSM_SW_F         = 0;

// Instantaneous Currents at fault to be sent to UART
int32_t Current_board_in_fault = 0;
int32_t Current_HSM_in_fault = 0;
int32_t Current_motor_in_fault = 0;

uint8_t TX_ENABLED = 0;

uint8_t V12board_bdc  = 1;    // +++++++++++++++++++++++++++++++++++++++++++++ da cambiare logica ++++++++++++++++++
uint8_t V12motor_bdc  = 1;    // +++++++++++++++++++++++++++++++++++++++++++++ da cambiare logica ++++++++++++++++++

uint8_t state_fsm        = START;
uint8_t state_fsm_boards = START;
uint8_t state_fsm_motors = START;
uint8_t state_fsm_prev;
uint8_t state_fsm_boards_prev;
uint8_t state_fsm_motors_prev;
uint8_t timerFSM      = 0;
uint8_t timerFSM_boards = 0;
uint8_t timerFSM_motors = 0;

uint8_t DCDC_status_A = 0;
uint8_t DCDC_status_B = 0;
uint8_t DCDC_ctrl     = 0;

uint16_t timeout    = 0;
uint16_t time_delay = 500;

uint8_t can_message = 0;

__IO ITStatus UartReady = RESET;

#ifdef BAT_B_R1
uint32_t VTH[7]={23000, 24000, 25000, 26000, 27000, 28000, 29000};   // threshold in mV R1 Battery
uint16_t Battery_high=4200*7;   // 7s5p battery
uint16_t Battery_low=3300*7;    // 7s5p battery
#endif 

#ifdef BAT_B_Generic
uint32_t VTH[7]={32000, 34000, 36000, 38000, 40000, 42000, 44000};   // threshold in mV iCub 2.5 Battery
uint16_t Battery_high=4200*10;   // 10s3p battery
uint16_t Battery_low=3300*10;    // 10s3p battery
#endif

adc_measure_t adc_measure = {0};  // initialize all adc values to 0
adc_measure_t mean = {0};         // initialize all average values to 0
uint32_t adc_values[9];           // contains all ADC channels conversion
uint32_t vBatterydV = 0;          // varibale used for sending mean.V_BATTERY to EMS in deciVolt

uint16_t adc_sample = 0;


fifo adc_samples = {0};
