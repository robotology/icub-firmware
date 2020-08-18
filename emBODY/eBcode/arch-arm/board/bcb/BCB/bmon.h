 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  I2C configuration                                *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


//------------------------------------------------------------------------------
// I/O Port Defines
//------------------------------------------------------------------------------
//#define  	SDATA			0x0080			// Serial data
//#define  	SCLK			0x0040			// Serial clock
#define     TIMEOUT_MAX     0x0500          // The value of the maximal timeout for I2C waiting loops


// I2C port definitions
#define BMON_I2C_DR                       (I2C1_BASE+0x10)

// BMON DMA definitions
#define BMON_DMA                          DMA1
#define BMON_DMA_CLK                      RCC_AHBPeriph_DMA1
#define BMON_DMA_TX_CHANNEL               DMA1_Channel6
#define BMON_DMA_RX_CHANNEL               DMA1_Channel7
#define BMON_DMA_TX_TCFLAG                DMA1_FLAG_TC6
#define BMON_DMA_RX_TCFLAG                DMA1_FLAG_TC7



// BMON DMA Direction
typedef enum
{
    BMON_DMA_TX = 0,
    BMON_DMA_RX = 1
}BMON_DMADirection_TypeDef;


extern void DMA1_Channel7_IRQHandler(void);


// Functions defined in module i2c.c
void BMON_Init      (void);
//void BMON_start   (void);						//	Sends I2C Start Trasfer

uint8_t BMON_ReadRegister(uint8_t DeviceAddr, uint8_t RegisterAddr, uint8_t* BMON_BufferRX, uint8_t bufferSize);

static void BMON_DMA_Config(BMON_DMADirection_TypeDef Direction, uint8_t* buffer, uint8_t bufferSize);

uint8_t BMON_TimeoutUserCallback(void);


//void i2c_stop           (void);						//	Sends I2C Stop Trasfer
//void i2c_write          (unsigned char input_data);	//	Writes data over the I2C bus
//unsigned char i2c_read  (void);                     //	Reads data from the I2C bus

#endif // __I2C_H


