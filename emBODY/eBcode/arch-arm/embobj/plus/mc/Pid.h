#ifndef MC_PID_H___
#define MC_PID_H___

#include "EoCommon.h"
#include "EoMotionControl.h"

#include "EOemsControllerCfg.h"

typedef struct //PID 
{
    float Ko;
    float Kp;
    float Kd;
    float Ki;
    
    float Kff;
    float Kbemf;
    float Ktau;
    
    float Dn;
    float En;
    float In;
    float Imax;
    
    float A,B;
    
    float stiction_up;
    float stiction_down;
  
    float out_max;
    float out_lpf;
    float out;
    
    uint8_t filter;
} PID; 

PID* PID_new(uint8_t n);
extern void PID_init(PID* o);
extern void PID_config(PID* o, eOmc_PID_t* config);
extern void PID_config_friction(PID *o, float Kbemf, float Ktau);
extern void PID_config_filter(PID *o, uint8_t filter);

extern void PID_reset(PID* o);
extern void PID_get_state(PID* o, float *out, float *err);

extern float PID_do_out(PID* o, float En);
extern float PID_do_friction_comp(PID *o, float vel, float Tr);

#endif
