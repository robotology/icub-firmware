#ifndef __QEP_H__
#define __QEP_H__

#include <stdint.h>

#include "UserTypes.h"

typedef struct
{
    int resolution;
    int offset;
    int elettr_deg_per_rev;
    int tolerance;
    unsigned char numPoles;
    BOOL full_calibration;
} EncoderConfig_t;
volatile extern EncoderConfig_t gEncoderConfig;

volatile extern int gQERawPosition;

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
#define QE_RISE_WARNING(e) gEncoderError.e=TRUE
#define QE_ELETTR_DEG_PER_REV() (gEncoderConfig.elettr_deg_per_rev)

extern volatile BOOL qe_index_found;
extern volatile BOOL is_qe_index_just_found;
extern volatile BOOL qe_index_found_debug;
extern volatile int QE_RESOLUTION;

#define QEIndexFound() qe_index_found

extern void QEinit(int qe_resolution,int motor_num_poles,char use_index);
extern unsigned int QEgetRaw();
extern int QEcountError();
extern void QEcountErrorClear();
extern int QEgetElettrDeg();
extern int QEgetPos();

#endif
