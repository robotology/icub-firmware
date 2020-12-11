#ifndef __LR17_ENCODER_H__
#define __LR17_ENCODER_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#if defined(USE_STM32HAL)
#include "stm32hal.h"
#else
#include "hal.h"
#endif

extern bool lr17_encoder_init();
extern bool lr17_encoder_acquire(void(*cb)(void *arg), void *arg);
extern bool lr17_encoder_get(int *angle);

#ifdef __cplusplus
  } /* extern "C" */
#endif

#endif
