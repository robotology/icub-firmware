//
//
// TODO : completare con una bella descrizione!
//
//
#include <p33FJ128MC802.h>
#include <qei.h>

#include "UserParms.h"

//volatile int zero_crossed;

void QEPSetUseZero(int enable_use_zero)
{
    if (enable_use_zero)
    {
        QEICONbits.QEIM = 6;
        IFS3bits.QEI1IF = 0;
    }
    else
    {
        QEICONbits.QEIM = 7;
    }
}

// called by the system.c module when the IDX pin changes state

void QEPInit()
// init the quadrature encoder peripheral
{
    // Configure QEI pins as digital inputs
    // ADPCFG |= 0x0038;
    // ADPCFG |= (1<<5 | 1<<6 | 1<<7); //0x0038;

    // Disable QEI Module
    QEICONbits.QEIM = 0;

    // Clear any count errors
    QEICONbits.CNTERR = 0;
    // Continue operation during sleep
    QEICONbits.QEISIDL = 0;
    // QEA and QEB eventually swapped
    QEICONbits.SWPAB = QE_SWAPAB;
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
    DFLTCONbits.QECK = 2;

    DFLTCONbits.IMV = 1;

    //  IFS3bits.QEI1IF = 0;
    // I was joking
    //  IEC3bits.QEI1IE = 1;

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

    MAXCNT = QE_RESOLUTION - 1;
    //MAXCNT = 14399;

    // default not to use zero. This is needed for generic
    // rotor alignment algo that does not make assumption
    // about the encoder in use and it does not expect the
    // encoder to autoreset magically.
    // If we use a "smarter" rotor alignment algo that is
    // aware of the encoder type (knows we have a QE and
    // knows what to do with it), it will eventually enable
    // the zero-reset by itself
    //QEPSetUseZero(0);

    // Reset position counter.
    //POSCNT = 0; //QE_RESOLUTION -1;
    POSCNT = 32767;

    // X4 mode with position counter reset by match with MAXCNT
    // QEICONbits.QEIM = 7;
    // X4 mode with position counter reset by Index Pulse
    QEICONbits.QEIM = 6;
    IFS3bits.QEI1IF = 0;
    //IEC3bits.QEI1IE = 1;
}

unsigned int QEPGet()
{
    return POSCNT;
}
