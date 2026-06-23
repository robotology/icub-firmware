#ifndef __QEP_H__
#define __QEP_H__

#include "UserTypes.h"

typedef union
{
    struct
    {
        unsigned dirty:1;
        unsigned stuck:1;
        unsigned index_broken:1;
        unsigned phase_broken:1;
        unsigned uncalibrated:1;
    };

    unsigned char bitmask;
} __attribute__((__packed__)) tQEError;
volatile extern tQEError gEncoderError;

volatile extern long gQEPosition;
volatile extern int  gQEVelocity;

#define QEgetRaw() POSCNT

#define QEcountError() (QEICONbits.CNTERR == TRUE)
#define QEcountErrorClear() QEICONbits.CNTERR = FALSE

extern void QEconfig(int qe_resolution);
extern void QEstart();
extern void QEresetOdometry();
extern void QEupdateOdometry();
extern int QEgetPos();

#endif
