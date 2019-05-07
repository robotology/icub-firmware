/****************************************************
*  3SG board - iCub Facility - 2019                 *
*  Si705x configuration                             *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

// The 7-bit base slave address is 0x40
// the R/W bit is set high for a read command and low for a write command

// A6 A5 A4 A3 A2 A1 A0 R/W
//  1  0  0  0  0  0  0  0

//Measure Temperature, Hold Master Mode 0xE3
//Measure Temperature, No Hold Master Mode 0xF3
//Reset 0xFE
//Write User Register 1 0xE6
//Read User Register 1 0xE7
//Read Electronic ID 1st Byte 0xFA 0x0F
//Read Electronic ID 2nd Byte 0xFC 0xC9
//Read Firmware Revision 0x84 0xB8

// Temperature(C) = 175.72*Temp_Code/65536 – 46.85


#include "stm32l4xx_hal.h"
#include "i2c.h"
#include "Si705x.h"

#include "stm32l4xx_ll_gpio.h"

const uint8_t Si705x_I2C_ADDRESS=0x80;    // left shifted device address 0x40 
//uint8_t MEASURE_CMD=0xF3;
uint8_t MEASURE_CMD=0xE3;

uint8_t Si705x_I2C3_TxBuffer[1] = {0};
uint8_t Si705x_I2C3_RxBuffer[2] = {0};

uint16_t Temperature_code;
uint16_t Temperature_board;
uint16_t Temperature_code_prev = 0;

uint8_t *MPU9250_SensorValue;

void Si705x_init(void){
	
}

void Si705x_deinit(void){

}

void Si705x_ReadTemperature(void){
	Si705x_init();
  Si705x_I2C3_TxBuffer[0]=MEASURE_CMD;
  HAL_I2C_Master_Transmit_DMA(&hi2c3, (uint16_t)Si705x_I2C_ADDRESS, (uint8_t*)Si705x_I2C3_TxBuffer, sizeof(Si705x_I2C3_TxBuffer));
}







