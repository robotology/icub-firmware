 /****************************************************
 *  BDC board - iCub Facility - 2013                 *
 *  Define configuration                             *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
#ifndef __DEFINE_H
#define __DEFINE_H


// ----------------------------------------------------------------------------
// Define
// ----------------------------------------------------------------------------
enum {  START,
        READY,
        ON_BOARD,
        ON_BOARD_MOTORS,
        OFF_BOARD_MOTORS,
        FAULT_BOARD,
        FAULT_MOTORS,
        FAULT_BOARD_MOTORS
};

#define V48motor_PG         0x0040      // PA6		OUTPUT	Led green DL1
#define V48motor_FLT        0x0080      // PA7		OUTPUT	Led red DL2
#define LED1                0x0100      // PA8		OUTPUT	Led blue DL3
#define EN_V12motor         0x0200      // PA9		OUTPUT	Enable 12V DC/DC converter for motors
#define EN_V12board         0x0400      // PA10		OUTPUT	Enable 12V DC/DC converter for motors
#define HSM_EN              0x0001      // PB0		OUTPUT	Enable Hot Swap Manager for 48V motors
#define HSM_nFLT            0x0002      // PB1		INPUT   Hot Swap Manager FAULT
#define HSM_nPG             0x0004      // PB2		INPUT   Enable Hot Swap POWER GOOD
#define CAN_LED             0x0010      // PB4		OUTPUT	Led orange DL7


#define LED1_ON             GPIOA->ODR   &= ~LED1;              // LED ON
#define LED1_OFF            GPIOA->ODR   |= LED1;               // LED OFF

#define V48motor_PG_ON		GPIOA->ODR   &= ~V48motor_PG;		// LED ON
#define V48motor_PG_OFF		GPIOA->ODR   |= V48motor_PG;		// LED OFF

#define V48motor_FLT_ON		GPIOA->ODR   &= ~V48motor_FLT;      // LED ON
#define V48motor_FLT_OFF	GPIOA->ODR   |= V48motor_FLT;		// LED OFF

#define EN_V12motor_ON		GPIOA->ODR   |= EN_V12motor;		// 
#define EN_V12motor_OFF		GPIOA->ODR   &= ~EN_V12motor;		// 

#define EN_V12board_ON		GPIOA->ODR   |= EN_V12board;		// 
#define EN_V12board_OFF		GPIOA->ODR   &= ~EN_V12board;		// 

#define CAN_LED_ON          GPIOB->ODR   &= ~CAN_LED;           // LED ON
#define CAN_LED_OFF         GPIOB->ODR   |= CAN_LED;            // LED OFF

#define HSM_EN_ON           GPIOB->ODR   |= HSM_EN;             // 
#define HSM_EN_OFF          GPIOB->ODR   &= ~HSM_EN;            // 


#endif
