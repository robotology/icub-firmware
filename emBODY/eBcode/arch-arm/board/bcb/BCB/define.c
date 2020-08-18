/****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  Define configuration                             *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

//#include "stm32f10x.h"
#include "define.h"

// ----------------------------------------------------------------------------
// Define
// ----------------------------------------------------------------------------
#define PB1							0x0010			// PC4		Pushbutton PB1
#define CPU_Green				0x0020			// PC5		Led green PB1
#define CPU_Red					0x0040			// PC6		Led red PB1
#define PB2							0x0080			// PC7		Pushbutton PB2
#define MOTORS_Green		0x0100			// PC8		Led green PB2
#define MOTORS_Red			0x0200			// PC9		Led red PB2
#define LED_BT					0x2000			// PB13		Led red PB2
#define CAN_LED					0x0004			// PB2		Led red PB2
#define SWT							0x0001			// PB0		Switch button SWT
#define FAN1						0x0100			// PB8		FAN1
#define FAN2						0x0200			// PB9		FAN2

#define CAN_LED_ON			GPIOB->ODR   &= ~CAN_LED;			// LED ON
#define CAN_LED_OFF			GPIOB->ODR   |= CAN_LED;			// LED OFF

#define LED_BT_ON				GPIOB->ODR   &= ~LED_BT;			// LED ON
#define LED_BT_OFF			GPIOB->ODR   |= LED_BT;				// LED OFF

#define PB1_LED_GREEN		GPIOC->ODR   |= CPU_Green;				GPIOC->ODR   &= ~CPU_Red;
#define PB1_LED_RED			GPIOC->ODR   &= ~CPU_Green;				GPIOC->ODR   |= CPU_Red;
#define PB1_LED_OFF			GPIOC->ODR   &= ~CPU_Green;				GPIOC->ODR   &= ~CPU_Red;

#define PB2_LED_GREEN		GPIOC->ODR   |= MOTORS_Green;			GPIOC->ODR   &= ~MOTORS_Red;
#define PB2_LED_RED			GPIOC->ODR   &= ~MOTORS_Green;		GPIOC->ODR   |= MOTORS_Red;
#define PB2_LED_OFF			GPIOC->ODR   &= ~MOTORS_Green;		GPIOC->ODR   &= ~MOTORS_Red;

#define FAN1_ON					GPIOB->ODR   &= ~FAN1;		// FAN1 ON
#define FAN1_OFF				GPIOB->ODR   |= FAN1;			// FAN1 OFF
#define FAN2_ON					GPIOB->ODR   &= ~FAN2;		// FAN1 ON
#define FAN2_OFF				GPIOB->ODR   |= FAN2;			// FAN1 OFF

#define DEBOUNCE_CNT		5						// debounce the keys
