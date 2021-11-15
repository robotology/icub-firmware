/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */
#include "i2c.h"
#include <pic16f1824.h>

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
extern char timer_100us;
extern char timeout;
extern unsigned int adc_buffer;

extern unsigned char I2C_Array[];

#define RX_ELMNTS	4
/*
volatile unsigned char I2C_Array[RX_ELMNTS] =   // array for master to write to
{0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0xFA,
0xEA, 0xDA, 0xCA, 0xBA, 0xFB, 0xFC, 0xFD, 0xFE,
0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

 */

unsigned int index_i2c = 0;            // used as an index pointer in array
unsigned char junk = 0;                // used to place unnecessary data
unsigned char first = 1;               // used to determine whether data address


void interrupt isr(void)
{
    if(PIR1bits.TMR1IF){		// Interrupt Timer1
	timer_100us=1;
	//TMR1H=0xF8;   // 50uS
	//TMR1L=0x72;   // 50uS
        TMR1H=0xF0;
	TMR1L=0xA2;
        PIR1bits.TMR1IF = 0;            //clear timer1 int flag bit
        //RA2=RA2^1;            // Test preciso
    }

    if(PIR1bits.ADIF){                  // Interrupt ADC
        adc_buffer = (ADRESH<<2) + (ADRESL>>6);
        PIR1bits.ADIF = 0;              //clear timer1 int flag bit
        //RC4=RC4^1;
    }


    if (PIR2bits.BCL1IF == 1){
        PIR2bits.BCL1IF = 0;
        //RC4=RC4^1;
        //I2cInitSlave(0x5A);
    }

    if (PIR1bits.SSP1IF == 1){                    // Interrupt I2C

        PIR1bits.SSP1IF = 0;

        timeout = 0;

        if (SSP1STATbits.R_nW){                   // Master read (R_nW = 1)
            if (!SSP1STATbits.D_nA){              // Last byte was an address (D_nA = 0)
                SSP1BUF = I2C_Array[index_i2c++];
                SSP1CON1bits.CKP = 1;             // Release CLK
            }
            if (SSP1STATbits.D_nA){               // Last byte was data (D_nA = 1)
                SSP1BUF = I2C_Array[index_i2c++];
                SSP1CON1bits.CKP = 1;             // Release CLK
            }
        }
        if (!SSP1STATbits.R_nW){                  // Master write (R_nW = 0)
            if (!SSP1STATbits.D_nA){              // Last byte was an address (D_nA = 0)
                first = 1;                        // last byte was address, next will be data location
                junk = SSPBUF;                    // read buffer to clear BF
                SSP1CON1bits.CKP = 1;             // Release CLK
            }
            if (SSP1STATbits.D_nA){               // Last byte was data (D_nA = 1)
                if (first){
                    index_i2c = SSP1BUF;          // load index with array location
                    first = 0;                    // now clear this since we have
                }                                 // location to read from/write to
                else{
                    if (index_i2c < RX_ELMNTS){   // make sure index is not out of range of array
                        I2C_Array[index_i2c++] = SSPBUF; //load array with data
                    }
                    else{
                        junk = SSP1BUF;           //array location not valid, discard data
                    }
                }
                if (SSP1CON1bits.WCOL){           // Did a write collision occur?
                    SSP1CON1bits.WCOL = 0;        // clear WCOL
                    junk = SSPBUF;                // dummy read to clear BF bit
                }
                SSP1CON1bits.CKP = 1;             // Release CLK
            }
        }
   //   if (SSP1STATbits.BF){           // Did a write collision occur?
                    //SSP1CON1bits.BF = 0;        // clear WCOL
    //                junk = SSPBUF;                // dummy read to clear BF bit
    //            }
    }
}



