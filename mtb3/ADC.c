/*
Copyright: Microchop Technology Inc. 2005
License: Microchip
Author:  Hrushikesh Vasuki
*/

/**********************************************************************
* © 2005 Microchip Technology Inc.
*
* FileName:        ADC.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC30Fxxxx
* Compiler:        MPLAB® C30 v3.00 or higher
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Hrushikesh Vasuki 07/29/05  First release of source file
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
* This file contains two functions - ADC_Init() and _ADCInterrupt().
*
**********************************************************************/

#include "p30f4011.h"
#include "ADC.h"
#include "LED.h"
extern   int AN2; //Accelerometer X axes
extern   int AN3; //Accelerometer Y axes
extern   int AN4; //Accelerometrt Z axes

//Functions and Variables with Global Scope:



//Functions:

void ADC_Init(void)
{
        //ADCON1 Register
        //Set up A/D for Automatic Sampling
        //Use internal counter (SAMC) to provide sampling time
        //Set up A/D conversrion results to be read in integer
        //number format.
        //Set up Sequential sampling for multiple S/H amplifiers
        //All other bits to their default state
        ADCON1bits.FORM = 0;
        ADCON1bits.SSRC = 7;
        #warning "auto no"
      //  ADCON1bits.ASAM = 1;
        ADCON1bits.SIMSAM = 0;

        //ADCON2 Register
        //Set up A/D for interrupting after 4 samples get filled in the buffer
        //Set up to sample on 4 S/H amplifiers - CH0, CH1, CH2 and CH3
        //All other bits to their default state
        ADCON2bits.SMPI = 3;
        ADCON2bits.CHPS = 3;
	//ADCON2bits.VCFG = 3; //Ideally use external references

        //ADCON3 Register
        //We would like to set up a sampling rate of 250 kSPS
        //Total Conversion Time= 1/Sampling Rate = 125 microseconds
        //At 250 kSPS, Tcy = 40 ns = Instruction Cycle Time
        //The A/D converter will take 12*Tad periods to convert each sample
        //So for ~1 MSPS we need to have Tad close to 307.2ns
        //Using equation in the Family Reference Manual we have
        //ADCS = 2*Tad/Tcy - 1
        ADCON3bits.SAMC = 0;
        ADCON3bits.ADCS =13;//53;//63;//60 corrisponde a circa 160us;//13

        //ADCHS Register
        //Set up A/D Channel Select Register to convert AN2 AN3 AN4 on Mux A input
        //of CH0 and CH1 S/H amplifiers
        ADCHS = 0x0022;//  0000|0000|0010|0010

        //ADCSSL Register
        //Channel Scanning AN2 AN3 AN4
        ADCSSL = 0x000E;

        //ADPCFG Register
        //Set up channels AN2 AN3 AN4 as analog inputs and configure rest as digital
        //Recall that we configured all A/D pins as digital when code execution
        //entered main() out of reset
        ADPCFG = 0xFFFF;
        ADPCFGbits.PCFG2 = 0;
        ADPCFGbits.PCFG3 = 0;
        ADPCFGbits.PCFG4 = 0;
        //Clear the A/D interrupt flag bit
        IFS0bits.ADIF = 0;

        //Set the A/D interrupt enable bit
        IEC0bits.ADIE = 1;

        //Turn on the A/D converter
        //This is typically done after configuring other registers
        ADCON1bits.ADON = 1;

}

//_ADCInterrupt() is the A/D interrupt service routine (ISR).
//The routine must have global scope in order to be an ISR.
//The ISR name is chosen from the device linker script.
void __attribute__((interrupt, no_auto_psv)) _ADCInterrupt(void)
{

	AN2 = ADCBUF0;
	AN3 = ADCBUF1;
	AN4 = ADCBUF2;
    //Clear the A/D Interrupt flag bit or else the CPU will
    //keep vectoring back to the ISR
    IFS0bits.ADIF = 0;

}

