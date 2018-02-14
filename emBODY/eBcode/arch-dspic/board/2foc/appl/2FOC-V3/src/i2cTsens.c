#include <p33FJ128MC802.h>
#include <libpic30.h>
#include <i2c.h>

#include "i2cTsens.h"

#define WAITFOR(flag, errcode, jump) for (wdog=12800; wdog && (flag); --wdog); if (!wdog) { temperature=errcode; goto jump; }

static BOOL config_sensor(void)
{
    int wdog;
    int temperature;
    
    // start
    IFS1bits.MI2C1IF=0;
    I2C1CONbits.SEN=1;
    WAITFOR(I2C1CONbits.SEN,0xEEEE,config_error)

    // transmit 0xD0
    IFS1bits.MI2C1IF=0;
    I2C1TRN=0xD0;
    WAITFOR(I2C1STATbits.TBF,0xEEEE,config_error)
    WAITFOR(!IFS1bits.MI2C1IF,0xEEEE,config_error) // Wait for 9th clock cycle

    if (!I2C1STATbits.ACKSTAT) //ACK received
    {
        // transmit 0x1B
        IFS1bits.MI2C1IF=0;
        I2C1TRN=0x1B;
        WAITFOR(I2C1STATbits.TBF,0xEEEE,config_error)
        WAITFOR(!IFS1bits.MI2C1IF,0xEEEE,config_error) // Wait for 9th clock cycle
        IFS1bits.MI2C1IF=0;
        WAITFOR(!I2C1STATbits.ACKSTAT,0xEEEE,config_error) //ACK received
    }

    // stop
    IFS1bits.MI2C1IF=0;
    I2C1CONbits.PEN=1;
    WAITFOR(I2C1CONbits.PEN,0xEEEE,config_error)

    return TRUE;

config_error:

    return FALSE;
}

BOOL setupI2CTsens(void)
{
    TRISBbits.TRISB8 = 0;
    RPOR4bits.RP8R = 0;

    TRISBbits.TRISB9 = 0;
    RPOR4bits.RP9R = 0;

    ODCBbits.ODCB8=1;
    ODCBbits.ODCB9=1;

    I2C1BRG = 395;
    I2C1CONbits.I2CEN = 1;

    //EnableIntMI2C1;

    __delay32(5000);

    return config_sensor();
}

int readI2CTsens(void)
{
    int wdog=0;
    int temperature=0xFFFF;

     // start
    IFS1bits.MI2C1IF=0;
    I2C1CONbits.SEN=1;
    WAITFOR(I2C1CONbits.SEN,0xFFF0,I2Ctimeout)

    // transmit 0xD1
    IFS1bits.MI2C1IF=0;
    I2C1TRN=0xD1;
    WAITFOR(I2C1STATbits.TBF, 0xFFF1,I2Ctimeout)
    WAITFOR(!IFS1bits.MI2C1IF,0xFFF2,I2Ctimeout) // Wait for 9th clock cycle

    if (!I2C1STATbits.ACKSTAT) //ACK received
    {
        unsigned int buffer[3]={0xFF,0xFF,0xFF};
        int b;

        for (b=0; b<3; ++b)
        {
            //while(I2C1CON & 0x1F); // lower 5 bits must be 0

            // receive enable
            IFS1bits.MI2C1IF=0;
            I2C1CONbits.RCEN=1;
            WAITFOR(I2C1CONbits.RCEN,0xFFF3,I2Ctimeout)

            IFS1bits.MI2C1IF=0;
            buffer[b]=I2C1RCV;

            //while(I2C1CON & 0x1F); // lower 5 bits must be 0

            IFS1bits.MI2C1IF=0;
            I2C1CONbits.ACKDT=(b==2);
            I2C1CONbits.ACKEN=1;
            WAITFOR(I2C1CONbits.ACKEN,0xFFF4,I2Ctimeout)
        }

        temperature = buffer[0]<<8 | buffer[1];
    }

    // stop
    IFS1bits.MI2C1IF=0;
    I2C1CONbits.PEN=1;
    WAITFOR(I2C1CONbits.PEN,0xFFF5,I2Ctimeout)

    return temperature;

I2Ctimeout:

    // recover
    I2C1CONbits.I2CEN = 0;

    TRISBbits.TRISB8 = 0;
    RPOR4bits.RP8R = 0;
    ODCBbits.ODCB8 = 1;

    int c;
    for (c=0; c<=18; ++c)
    {
        PORTBbits.RB8 = c%2;
        __delay32(400);
    }

    I2C1CONbits.I2CEN = 1;

    __delay32(400);

    IFS1bits.MI2C1IF=0;
    I2C1CON &= 0xFFE0;
    I2C1CONbits.PEN=1;
    WAITFOR(I2C1CONbits.PEN,0xEEEE,recover_error)

    __delay32(400);

    config_sensor();

recover_error:

    return temperature;
}