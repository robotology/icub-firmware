#include <p33FJ128MC802.h>
#include <qei.h>

#include "Faults.h"
#include "qep.h"

volatile tQEError gEncoderError;

volatile long gQEPosition = 0;
volatile int  gQEVelocity = 0;

unsigned int QE_RESOLUTION = 0;
BOOL SWPAB = FALSE;

inline int QEgetPos() __attribute__((always_inline));
inline int QEgetPos()
{
    int poscnt = (int)POSCNT;

    return __builtin_divsd(((long)poscnt)<<16,QE_RESOLUTION);
}

void QEconfig(int resolution)
{
    if (resolution<0)
    {
        QE_RESOLUTION = -resolution;
        SWPAB = TRUE;
    }
    else
    {
        QE_RESOLUTION = resolution;
        SWPAB = FALSE;
    }
}

void QEstart()
{
    // init the quadrature encoder peripheral
    // Configure QEI pins as digital inputs
    // ADPCFG |= (1<<5 | 1<<6 | 1<<7); //0x0038;

    // Disable QEI Module
    QEICONbits.QEIM = 0;
    // Clear any count errors
    QEICONbits.CNTERR = 0;
    // Continue operation during sleep
    QEICONbits.QEISIDL = 0;

    // QEA and QEB eventually swapped
    //QEICONbits.SWPAB = 0; // iCub3 motor
    //QEICONbits.SWPAB = 1; // old motor and CER motor
    QEICONbits.SWPAB = SWPAB;

    // do not output direction on IO pin
    QEICONbits.PCDOUT = 0;
    // Index pulse resets position counter
    QEICONbits.POSRES = 1;

    // Count error interrupts disabled
    DFLTCONbits.CEID = 1;
    // Digital filters output enabled for QEn and IDX pins
    DFLTCONbits.QEOUT = 1;
    // 1:4 clock divide for digital filter for QEn
    // main CK = 40Mhz. Filter CK = 10 Mhz
    // Filter accept edge if it persist over 3 CK
    // cycles. So max QEP pin freq is about 3.3Mhz
    DFLTCONbits.QECK = 5;

    DFLTCONbits.IMV = 3;

    //#endif
    // Initialize the QEP module counter to run in modulus.
    //
    // Example (drawn in X1 mode): encoder 3ppr, MAXCNT = 2;
    //      __      __      __      __      __      __      __      __      __      __
    //     |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |
    //  ___|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |
    // 2   0       1       2       0       1       2       0       1       2       0
    //                       |                       |                       |
    //                     round                   round                   round

    MAXCNT = 0xFFFF;//QE_RESOLUTION - 1;

    // default not to use zero. This is needed for generic
    // rotor alignment algo that does not make assumption
    // about the encoder in use and it does not expect the
    // encoder to autoreset magically.
    // If we use a "smarter" rotor alignment algo that is
    // aware of the encoder type (knows we have a QE and
    // knows what to do with it), it will eventually enable
    // the zero-reset by itself
    //QEPSetUseZero(0);

    // X2 mode with position counter reset by match with MAXCNT
    QEICONbits.QEIM = 7;
    IEC3bits.QEI1IE = 0;
    IFS3bits.QEI1IF = 0;
    POSCNT = 0;

    gEncoderError.uncalibrated = TRUE;
}

static volatile unsigned int position_old = 0;
static volatile int index = 0;
static volatile long buffer[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void QEupdateOdometry()
{
    unsigned int position = QEgetPos();
    int delta = position - position_old;
    position_old = position;

    gQEPosition += delta;

    buffer[index++] = gQEPosition;
    //index %= 20;
    gQEVelocity = (gQEVelocity + (int)(gQEPosition - buffer[index%=20]))/2;
}

void QEresetOdometry()
{
    int i;

    for (i=0; i<20; ++i) buffer[i] = 0;

    POSCNT = 0;
    index = 0;
    gQEPosition = 0;
    gQEVelocity = 0;
    position_old = 0;
}