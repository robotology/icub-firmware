#ifndef __QEP_H__
#define __QEP_H__

#include "UserTypes.h"

typedef struct
{
    int ticks;
    int offset;
    int elettr_deg_per_rev;
    int tolerance;
    char numPoles;
    BOOL full_calibration;
} EncoderConfig_t;
volatile extern EncoderConfig_t gEncoderConfig;

typedef union
{
    struct
    {
        unsigned dirty:1;
        unsigned stuck:1;
        unsigned index_broken:1;
        unsigned phase_broken:1;
        unsigned uncalibrated:1;
        unsigned autocalib:1;
    };

    unsigned char bitmask;
} __attribute__((__packed__)) tQEError;
volatile extern tQEError gEncoderError;

#define QE_RISE_ERROR(e) gEncoderError.e=TRUE; SysError.EncoderFault=TRUE; FaultConditionsHandler()
#define QE_ELETTR_DEG_PER_REV() (gEncoderConfig.elettr_deg_per_rev)

extern volatile BOOL qe_ready;
#define QEready() qe_ready

// MECAPION
//#define QEgetRaw() (POSCNT<<5)
//#define QEgetElettrDeg() ((int)(__builtin_muluu(POSCNT,360)>>11))

extern void QEinit(int ticks,int motor_num_poles,BOOL use_index);
extern void QEcountErrorClear();
extern  int QEgetElettrDeg();
extern  int QEgetPosition();
extern void QEHESCrossed(int up);
//extern int32_t QEgetPosCheck();
#endif
