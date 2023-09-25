/****************************************************
*  iCub Facility - 2019                             *
*  uOLED Display Utility                            *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"
#include "Display_uOLED.h"
#include "usart.h"

// -----------------------------------------------------------------------------------------------------------------------------
// Display_uOLED_128_G2
// -----------------------------------------------------------------------------------------------------------------------------
uint16_t uOLED_Parameters[18] = {0};
uint16_t Display_qualifier = 0x01;


void Display_uOLED_init(){
  uOLED_Parameters[0]  = ((sizeof(uOLED_Parameters))<<8) +  Display_qualifier; 
  uOLED_Parameters[1] = (Firmware_vers<<8) + (Revision_vers<<4) + (Build_number);
}


void Display_uOLED_128_G2(){
  if(mean.V_BATTERY > Battery_high)       {Battery_charge=100;}
  else if(mean.V_BATTERY < Battery_low)   {Battery_charge=0;}
  else                                    {Battery_charge = 100*(mean.V_BATTERY-Battery_low)/(Battery_high-Battery_low);}
  
  //uOLED_Parameters[0] 
  //uOLED_Parameters[1] 
  uOLED_Parameters[2] = (state_fsm<<8) + (state_fsm_boards<<4) + (state_fsm_motors);
  uOLED_Parameters[3]  = Battery_charge;
  uOLED_Parameters[4]  = mean.V_VINPUT;
  uOLED_Parameters[5]  = mean.V_EXTPS;
  uOLED_Parameters[6]  = mean.V_BATTERY;
  uOLED_Parameters[7]  = mean.V_V12board;
  uOLED_Parameters[8]  = mean.V_V12motor;
  uOLED_Parameters[9]  = mean.I_V12board;
  uOLED_Parameters[10] = mean.I_V12motor;
  uOLED_Parameters[11] = mean.I_HSM;
  
  uOLED_Parameters[12] = (100*HSM_PG) + (10*V12motor) + (V12board);
  uOLED_Parameters[13] = (100*HSM_F) + (10*V12motor_F) + (V12board_F);
  uOLED_Parameters[14] = (DCDC_status_B << 8) | DCDC_status_A;
  
  uOLED_Parameters[15] = Current_board_in_fault;
  uOLED_Parameters[16] = Current_motor_in_fault;
  uOLED_Parameters[17] = Current_HSM_in_fault;
  
  HAL_UART_Transmit_DMA(&huart1, (uint8_t*)uOLED_Parameters, sizeof(uOLED_Parameters));
}

