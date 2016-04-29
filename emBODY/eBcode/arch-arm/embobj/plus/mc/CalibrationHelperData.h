#ifndef MC_CALIBRATION_HELPER_DATA_H___
#define MC_CALIBRATION_HELPER_DATA_H___

#include "EOemsControllerCfg.h"

typedef struct // TripodCalib
{
    int32_t pwm;
    int32_t zero;
    int32_t max_delta;
    int32_t start_pos[3];
    
} TripodCalib;

typedef struct // CableCalib
{
    int32_t pwm;
    int32_t delta;
    int32_t target;
    int32_t cable_range;
} CableCalib;

#endif
