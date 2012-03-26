
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _LED_MCBSTM32F400_H_
#define _LED_MCBSTM32F400_H_

#include "stdint.h"

extern void led_mcbstm32f400_init (void);
extern void led_mcbstm32f400_on (uint32_t pin);
extern void led_mcbstm32f400_off (uint32_t pin);


#endif



