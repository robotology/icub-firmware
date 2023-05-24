#include <p33FJ128MC802.h>
#include <stdlib.h>
#include <qei.h>

#include "Faults.h"
#include "qep.h"
#include "2FOC.h"

volatile EncoderConfig_t gEncoderConfig={0,90,0};
volatile tQEError gEncoderError;

volatile BOOL QE_READY = FALSE;

volatile int16_t QE_TICKS = 0;
volatile int32_t QE_RESOLUTION = 0;
volatile int16_t QE_NUM_POLES = 0;
volatile int16_t QE_RES_POLE = 0;
volatile int16_t QE_RES_POLE_HALF = 0;
volatile int16_t POSCNT_OLD = 0;

volatile int32_t QE_COUNTER = 0;

volatile int16_t QE_ERR_THR = 0;
//volatile int32_t position_check = 0;

void QEinit(int16_t ticks, int8_t num_poles, BOOL use_index)
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

    QE_READY = FALSE;
    
    QE_NUM_POLES = num_poles;
    
    if (ticks < 0)
    {
        QEICONbits.SWPAB = 1;
        QE_TICKS = -ticks;
    }
    else
    {
        QEICONbits.SWPAB = 0;
        QE_TICKS =  ticks;
    }
    
    QE_RESOLUTION = 4*QE_TICKS;
    QE_RES_POLE = QE_RESOLUTION/QE_NUM_POLES;
    QE_RES_POLE_HALF = QE_RES_POLE/2;
    
    QE_ERR_THR = (QE_TICKS * (long)gEncoderConfig.tolerance) / 3600;
    
    gEncoderConfig.elettr_deg_per_rev = 360*QE_NUM_POLES;

    //QEICONbits.SWPAB = 0; // iCub3 motor
    //QEICONbits.SWPAB = 1; // old motor and CER motor
    // do not output direction on IO pin
    QEICONbits.PCDOUT = 0;
    
    // Digital filters output enabled for QEn and IDX pins
    DFLTCONbits.QEOUT = 1;
    // 1:4 clock divide for digital filter for QEn
    // main CK = 40Mhz. Filter CK = 10 Mhz
    // Filter accept edge if it persist over 3 CK
    // cycles. So max QEP pin freq is about 3.3Mhz
    DFLTCONbits.QECK = 2;
    DFLTCONbits.IMV  = 3; // iCub
    //DFLTCONbits.IMV  = 2; // ergoCub ?

    // Initialize the QEP module counter to run in modulus.
    //
    // Example (drawn in X1 mode): encoder 3ppr, MAXCNT = 2;
    //      __      __      __      __      __      __      __      __      __      __
    //     |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |
    //  ___|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |
    // 2   0       1       2       0       1       2       0       1       2       0
    //                       |                       |                       |
    //                     round                   round                   round
    
    QE_COUNTER = 0;
    
    // Reset position counter
    POSCNT = 0;
    MAXCNT = QE_RES_POLE-1;
    POSCNT_OLD = 0;

    // Index pulse doesn't reset position counter
    QEICONbits.POSRES = 0;
    
    if (use_index)
    {
        // Count error interrupts enabled
        DFLTCONbits.CEID = 0;
        
        // Counter reset by index 
        QEICONbits.QEIM = 6;
        
        // Enable interrupts
        IEC3bits.QEI1IE = 1;
    }
    else
    {
        // Count error interrupts disabled
        DFLTCONbits.CEID = 1;
             
        // Disable interrupts
        IEC3bits.QEI1IE = 0;
        
        // Counter reset by match with MAXCNT
        QEICONbits.QEIM = 7;
    }
    
    // reset interrupt flag
    IFS3bits.QEI1IF = 0;
}

inline void QEcountErrorClear() __attribute__((always_inline));
inline void QEcountErrorClear()
{
    QEICONbits.CNTERR = FALSE;
}

inline int QEgetElettrDeg() __attribute__((always_inline));
inline int QEgetElettrDeg()
{
    uint16_t degrees = 0;
    
    if (QE_RES_POLE) degrees = __builtin_divud(__builtin_muluu(POSCNT,360),(unsigned)QE_RES_POLE);
        
    return (7200 + gEncoderConfig.offset + (int)degrees) % 360;
}

//extern volatile int position_fbk;

inline int QEgetPosition() __attribute__((always_inline));
inline int QEgetPosition()
{    
    int16_t delta = (int16_t)POSCNT - POSCNT_OLD;
    
    POSCNT_OLD = POSCNT;
    
    while (delta < -QE_RES_POLE_HALF) delta += QE_RES_POLE;
    while (delta > +QE_RES_POLE_HALF) delta -= QE_RES_POLE;
    
    QE_COUNTER += delta;
    
    while (QE_COUNTER >= QE_RESOLUTION) QE_COUNTER -= QE_RESOLUTION;
    while (QE_COUNTER < 0)              QE_COUNTER += QE_RESOLUTION;
    
    //position_fbk = QE_COUNTER;
        
    return __builtin_divsd(QE_COUNTER<<14,QE_TICKS);
} 

//inline int32_t QEgetPosCheck() __attribute__((always_inline));
//inline int32_t QEgetPosCheck()
//{
//    return position_check;
//}

// this is the same of the index simulated by the hall sensors 
// so that it must be triggered every num_poles transitions 
// 1->6 or 6->1
void QEHESCrossed(BOOL up)
{
    extern volatile long gQEPosition;
    
    POSCNT = up ? 0 : MAXCNT;
    
    if (!QE_READY)
    {     
        gQEPosition = 0;
        QE_READY = TRUE;
    }
}

// QE zero crossing interrupt manager
void __attribute__((__interrupt__,no_auto_psv)) _QEI1Interrupt(void)
{   
    // disable interrupts
    IEC3bits.QEI1IE = 0;
    
    extern volatile long gQEPosition;
    
    static int UPDN = -1;
    static int watchdog = 0;
    
    if (QEICONbits.CNTERR) // ROLLOVER
    {
        if (QE_READY)
        {
            watchdog += QEICONbits.UPDN ? 1 : -1;
        
            if (abs(watchdog) > QE_NUM_POLES) gEncoderError.index_broken = TRUE;
        }
    }
    else // INDEX
    {        
        if (QE_READY)
        {  
            if (QEICONbits.UPDN == UPDN)
            {
                if (watchdog == 0)
                {
                    gEncoderError.phase_broken = TRUE;
                }
                else
                {
                    if (QEICONbits.UPDN)
                    {
                        if (POSCNT < QE_RES_POLE-QE_ERR_THR) gEncoderError.dirty = TRUE;
                    }
                    else
                    {
                        if (POSCNT >             QE_ERR_THR) gEncoderError.dirty = TRUE;
                    }
                }
            }
             
            UPDN = QEICONbits.UPDN;
        }
        else
        {
            UPDN = -1;
                        
            gQEPosition = 0;
            
            QE_READY = TRUE;
        }
        
        watchdog = 0;
    }
      
    POSCNT = QEICONbits.UPDN ? 0 : MAXCNT;
    
    //POSCNT_OLD = POSCNT;
    //QE_COUNTER = QEICONbits.UPDN ? 0 : -1;

    QEICONbits.CNTERR = FALSE;
    
    // clear interrupt flag
    IFS3bits.QEI1IF = 0;
    
    // enable interrupts
    IEC3bits.QEI1IE = 1;
}
