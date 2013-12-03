/***********************************************************************
 * This file provides a basic template for writing dsPIC30F trap       *
 * handlers in C language for the C30 compiler                         *
 *                                                                     *
 * Add this file into your MPLAB project. Build your project, program  *
 * the device and run. If any trap occurs during code execution, the   *
 * processor vectors to one of these routines.                         *
 *                                                                     *
 * For additional information about dsPIC architecture and language    *
 * tools, refer to the following documents:                            *
 *                                                                     *
 * MPLAB C30 Compiler User's Guide                        : DS51284    *
 * dsPIC 30F MPLAB ASM30, MPLAB LINK30 and Utilites                    *
 *                                           User's Guide : DS51317    *
 * Getting Started with dsPIC DSC Language Tools          : DS51316    *
 * dsPIC 30F Language Tools Quick Reference Card          : DS51322    *
 * dsPIC 30F 16-bit MCU Family Reference Manual           : DS70046    *
 * dsPIC 30F General Purpose and Sensor Families                       *
 *                                           Data Sheet   : DS70083    *
 * dsPIC 30F/33F Programmer's Reference Manual            : DS70157    *
 *                                                                     *
 * Template file has been compiled with MPLAB C30 v3.00.               *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 *    Author:                                                          *
 *    Company:                                                         *
 *    Filename:       traps.c                                          *
 *    Date:           04/11/2007                                       *
 *    File Version:   3.00                                             *
 *    Devices Supported:  All PIC24F,PIC24H,dsPIC30F,dsPIC33F devices  *
 *                                                                     *
 **********************************************************************/
#include "hal_can.h"

#include <libpic30.h>

#ifdef __dsPIC30F__

        #include "p30fxxxx.h"

#endif

#ifdef __dsPIC33F__

        #include "p33Fxxxx.h"

#endif

#ifdef __PIC24F__

        #include "p24Fxxxx.h"

#endif

#ifdef __PIC24H__

        #include "p24Hxxxx.h"

#endif


#define _trapISR __attribute__((interrupt,no_auto_psv))

/* ****************************************************************
* Standard Exception Vector handlers if ALTIVT (INTCON2<15>) = 0  *
*                                                                 *
* Not required for labs but good to always include                *
******************************************************************/
void _trapISR _OscillatorFail(void)
{

        INTCON1bits.OSCFAIL = 0;
        while(1);
}

void _trapISR _AddressError(void)
{
    hal_can_frame_t frame;


    //notify error    
    frame.id = 0xA;
    frame.id_type = hal_can_frameID_std;
    frame.frame_type = hal_can_frame_data;
    frame.size = 5;
    frame.data[0] = 'E';
    frame.data[1] = 'a';
    frame.data[2] = 'd';
    frame.data[3] = 'd';
    frame.data[4] = 'r';
       
    hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);

    INTCON1bits.ADDRERR = 0;
    //RETFIE;
//    while(1)
//    {
//            hal_led_on(led_red);
//        __delay32(800000);
//        hal_led_off(led_red);
//        __delay32(800000);
//    }
}

void _trapISR _StackError(void)
{

        INTCON1bits.STKERR = 0;
        while(1);
}

void _trapISR _MathError(void)
{

        INTCON1bits.MATHERR = 0;
        while(1);
}

void _trapISR _DMACError(void)
{
    INTCON1bits.DMACERR = 0;
    hal_can_frame_t frame;


    //notify error    
    frame.id = 0xA;
    frame.id_type = hal_can_frameID_std;
    frame.frame_type = hal_can_frame_data;
    frame.size = 5;
    frame.data[0] = 'E';
    frame.data[1] = 'd';
    frame.data[2] = 'm';
    frame.data[3] = 'a';
    frame.data[4] = 'c';
       
    hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);

    //RETFIE;
    while(1)
    {
        ;
    }

}




/* ****************************************************************
* Alternate Exception Vector handlers if ALTIVT (INTCON2<15>) = 1 *
*                                                                 *
* Not required for labs but good to always include                *
******************************************************************/
void _trapISR _AltOscillatorFail(void)
{

        INTCON1bits.OSCFAIL = 0;
        while(1);
}

void _trapISR _AltAddressError(void)
{

        INTCON1bits.ADDRERR = 0;
        while(1);
}

void _trapISR _AltStackError(void)
{

        INTCON1bits.STKERR = 0;
        while(1);
}

void _trapISR _AltMathError(void)
{

        INTCON1bits.MATHERR = 0;
        while(1);
}



