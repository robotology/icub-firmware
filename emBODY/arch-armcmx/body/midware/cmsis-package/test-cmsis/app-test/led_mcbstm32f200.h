
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _LED_MCBSTM32F200_H_
#define _LED_MCBSTM32F200_H_

#include "stdint.h"

extern void led_mcbstm32f200_init (void);
extern void led_mcbstm32f200_on (uint32_t pin);
extern void led_mcbstm32f200_off (uint32_t pin);


#endif



