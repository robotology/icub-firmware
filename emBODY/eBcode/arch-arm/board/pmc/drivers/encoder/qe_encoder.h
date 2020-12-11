#ifndef __QE_ENCODER_H__
#define __QE_ENCODER_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include <stdbool.h>

#if defined(USE_STM32HAL)
#include "stm32hal.h"
#else
#include "hal.h"
#endif

typedef struct {
    TIM_HandleTypeDef *htim;
} qe_encoder_cfg_t;

typedef struct {
    qe_encoder_cfg_t cfg;
} qe_encoder_t;

extern bool qe_encoder_init(qe_encoder_t *e, qe_encoder_cfg_t *c);
extern bool qe_encoder_get(qe_encoder_t *e, int *val);

#ifdef __cplusplus
  } /* extern "C" */
#endif
  
#endif
