#include <p33FJ128MC802.h>
#include <libpic30.h>

#include <stdint.h>
#include <i2c.h>
#include <xc.h>

#include "i2cTsens.h"

#if 0

void I2CInit(void)
{
    I2C1BRG = 393;          // @100kHz; (FCY/FSCL - FCY/1e7) - 1
    I2C1CONbits.I2CEN = 0;  // Disable I2C
    I2C1CONbits.DISSLW = 1; // Disable slew rate control
    I2C1CONbits.A10M = 0;   // 7-bit slave addr
    I2C1CONbits.SCLREL = 1; // SCL release control
    I2C1CONbits.I2CEN = 1;  // Enable I2C
    IEC1bits.MI2C1IE = 0;   // Master I2C interrupt
    IFS1bits.MI2C1IF = 0;   // MI2C Flag
}
 
void I2CAck(void)
{
    I2C1CONbits.ACKDT = 0;      // Send ACK
    I2C1CONbits.ACKEN = 1;      // Initiate Acknowledge and transmit ACKDT
    while(I2C1CONbits.ACKEN);
}

void I2CNack(void)
{
    I2C1CONbits.ACKDT = 1;      // Send NACK
    I2C1CONbits.ACKEN = 1;      // Initiate Acknowledge and transmit ACKDT
    while(I2C1CONbits.ACKEN);  
}
 
void I2CStop(void)
{
    I2C1CONbits.RCEN = 0;       // receive mode not in progress
    I2C1CONbits.PEN = 1;        // Stop condition
    while(I2C1CONbits.PEN);
}
 
void I2CStart(void)
{
    I2C1CONbits.ACKDT = 0;      // Reset any ACK
    I2C1CONbits.SEN = 1;        // Start
    while(I2C1CONbits.SEN);
}
 
void I2CRestart(void)
{
    I2C1CONbits.RSEN = 1;       // Repeated Start Condition
    while(I2C1CONbits.RSEN);
    I2C1CONbits.ACKDT = 0;      // Send ACK
    I2C1STATbits.TBF = 0;       // I2C1TRN is empty
}
 
void I2CWaitACK(void)
{
    while(I2C1STATbits.ACKSTAT);
}
 
void I2CIdle(void)
{
    while(I2C1STATbits.TRSTAT);
}
 
void I2CWrite(unsigned char c)
{
    I2C1TRN = c;
    while(I2C1STATbits.TBF);
}

void I2CRead(void)
{
    I2C1CONbits.RCEN = 1;
    Nop();
    while(!I2C1STATbits.RBF);
}
 
void I2CWriteReg(char addr, char byteHigh, char byteLow, char value)
{
    // Start Condition
    I2CStart();
    // EEPROM Addr
    I2CWrite((addr<<1)&0xFE);
    I2CIdle();
    // Addr High Byte
    I2CWrite(byteHigh);
    I2CIdle();
    // Addr Low Byte
    I2CWrite(byteLow);
    I2CIdle();
    // Value
    I2CWrite(value);
    I2CIdle();
    // Stop
    I2CStop();
}
 

char I2CReadReg(char addr, char byteHigh, char byteLow)
{
    char temp;
    // Start Condition
    I2CStart();
    // EEPROM Addr
    I2CWrite((addr<<1)&0xFE);
    I2CIdle();
    // Addr High Byte
    I2CWrite(byteHigh);
    I2CIdle();
    // Addr Low Byte
    I2CWrite(byteLow);
    I2CIdle();
    // Restart
    I2CRestart();
    I2CWrite((addr<<1)|0x01);
    I2CIdle();  
    I2CRead();
    I2CNack();
    I2CStop();
    temp = I2C1RCV;
    return temp;
}
 
void I2CSequentialWriteReg(char addr, char byteHigh, char byteLow, char* value, int length)
{
    int j;
    // Start Condition
    I2CStart();
    // EEPROM Addr
    I2CWrite((addr<<1)&0xFE);
    I2CIdle();
    // Addr High Byte
    I2CWrite(byteHigh);
    I2CIdle();
    // Addr Low Byte
    I2CWrite(byteLow);
    I2CIdle();
    // Value
    for(j = 0; j < length; j++)
    {
        I2CWrite(value[j]);
        I2CIdle();
    }
    // Stop
    I2CStop();    
}
 
void I2CSequentialReadReg(char addr, char byteHigh, char byteLow, char* buff, int length)
{
    int j;
    // Start Condition
    I2CStart();
    // EEPROM Addr
    I2CWrite((addr<<1)&0xFE);
    I2CIdle();
    // Addr High Byte
    I2CWrite(byteHigh);
    I2CIdle();
    // Addr Low Byte
    I2CWrite(byteLow);
    I2CIdle();
    // Restart
    I2CRestart();
    I2CWrite((addr<<1)|0x01);
    I2CIdle();  
    for(j = 0; j < length; j++)
    {
        I2CRead();
        buff[j] = I2C1RCV;  
        I2CAck();
    }
    buff[j] = '\0';
    I2CNack();
    I2CStop();
    
    
}
#endif





volatile int synthTempCounter = 0;//timer for generation of synthetic data
int I2Cwdog = 0;
volatile int I2Ccomerrwdog = 0; //counts number of continuos communication error
volatile int I2Cerrcode = 0;
volatile char I2Cdead = 0;
volatile uint16_t I2Cerrors = 0;
volatile BOOL overheating = FALSE;

//25600
#define WHILE(flag, errcode, jump) for (I2Cwdog=6400; I2Cwdog && (flag); --I2Cwdog); if (!I2Cwdog) { I2Cerrcode=errcode; goto jump; }

//////////////////////////////////

static int config_sensor(void)
{
    I2Cerrcode = 0;
    
    // start
    I2C1CONbits.ACKDT = 0; // Reset any ACK
    I2C1CONbits.SEN = 1;
    WHILE(I2C1CONbits.SEN, -1, config_error)

    // transmit 0xD2        
    I2C1TRN = 0xD2;
    WHILE(I2C1STATbits.TBF, -2, config_error)
    // wait Idle
    WHILE(I2C1STATbits.TRSTAT, -3, config_error);
    // wait ACK
    WHILE(I2C1STATbits.ACKSTAT, -4, config_error)
    
    // transmit 0x19        
    I2C1TRN = 0x19;
    WHILE(I2C1STATbits.TBF, -5, config_error)        
    // wait Idle
    WHILE(I2C1STATbits.TRSTAT, -6, config_error);
    // wait ACK
    WHILE(I2C1STATbits.ACKSTAT, -7, config_error)        
            
    // stop
    I2C1CONbits.RCEN = 0;
    I2C1CONbits.PEN=1;
    WHILE(I2C1CONbits.PEN, -8, config_error)

config_error:

    return I2Cerrcode;
}

int setupI2CTsens(void)
{
    TRISBbits.TRISB8 = 0;
    RPOR4bits.RP8R = 0;

    TRISBbits.TRISB9 = 0;
    RPOR4bits.RP9R = 0;

    ODCBbits.ODCB8=1;
    ODCBbits.ODCB9=1;

    I2C1BRG = 196;//393;          // @100kHz; (FCY/FSCL - FCY/1e7) - 1
    I2C1CONbits.I2CEN = 0;  // Disable I2C
    I2C1CONbits.DISSLW = 1; // Disable slew rate control
    I2C1CONbits.A10M = 0;   // 7-bit slave addr
    I2C1CONbits.SCLREL = 1; // SCL release control
    I2C1CONbits.I2CEN = 1;  // Enable I2C
    IEC1bits.MI2C1IE = 0;   // Master I2C interrupt
    IFS1bits.MI2C1IF = 0;   // MI2C Flag

    __delay32(5000);

    return config_sensor();
}


#define TEST_ON_ROBOT

#ifdef TEST_ON_ROBOT
//-30 deg
#define ERR_VAL_0X19 -378
//-50 deg
#define ERR_VAL_0X10 -631
//-70 deg
#define ERR_NUM_10 -886
// -90 deg
#define DEF_VAL_TEMP -1141
// 10 deg for increments of synth data
#define INCR_DEG_10 127

#else
//-30 deg
#define ERR_VAL_0X19 -2729
// -50 deg
#define ERR_VAL_0X10 -4613
// -70 deg
#define ERR_NUM_10 -6549
// -90 deg
#define DEF_VAL_TEMP -8541
// 10 deg for increments of synth data
#define INCR_DEG_10 897
#endif
    

int readI2CTsens(volatile int* temperature)
{   
    I2Cerrcode = 0;
    *temperature = DEF_VAL_TEMP;
    
    // start
    I2C1CONbits.ACKDT = 0; // Reset any ACK
    I2C1CONbits.SEN = 1;
    WHILE(I2C1CONbits.SEN, -9, I2Ctimeout)

    // transmit 0xD3        
    I2C1TRN = 0xD3;
    WHILE(I2C1STATbits.TBF, -10, I2Ctimeout)
    // wait Idle
    WHILE(I2C1STATbits.TRSTAT, -11, I2Ctimeout)
    // wait ACK
    //WHILE(I2C1STATbits.ACKSTAT, -12, I2Ctimeout)           
    
    if (!I2C1STATbits.ACKSTAT)        
    {
        unsigned int buffer[3]={0xFF,0xFF,0xFF};
        int b;

        for (b=0; b<3; ++b)
        {
            // receive enable
            I2C1CONbits.RCEN = 1;
            Nop();
            WHILE(!I2C1STATbits.RBF, -13, I2Ctimeout)
            buffer[b]=I2C1RCV;
            I2C1CONbits.ACKDT = (b==2); // Send ACK/NACK
            I2C1CONbits.ACKEN = 1;      // Initiate Acknowledge and transmit ACKDT
            WHILE(I2C1CONbits.ACKEN, -14, I2Ctimeout)
        }
        
        if(buffer[2] == 0x19) //data valid
        {
            int cur_tmp = buffer[0]<<8 | buffer[1];
            
            if(cur_tmp == 0x7fff)
            {
                I2Cerrcode = -18;
                I2Ccomerrwdog++;
            }
            else
            {
                //if i'm here I have a correct value
                *temperature = cur_tmp;
                I2Ccomerrwdog = 0; // clean the communication error watchdog
            }
        }         
        else
        {
            I2Ccomerrwdog++;
            //data not valid
            if(buffer[2] == 0x10) // sensor re-initialized itself --> configure again
            {
                I2Cerrcode = -20;
                *temperature = ERR_VAL_0X10;
            }
            else
            {
                *temperature = ERR_VAL_0X19;
                I2Cerrcode = -19;
            }
            goto I2Ctimeout; //--> go to reinit the I2C
        }
    }
    else
    {
        ++I2Ccomerrwdog;
        I2Cerrcode = -17;
    }
    
    if(I2Ccomerrwdog > 100) // cannot communicate
    {
        I2Cerrcode = -21;
        
        if(( *temperature != ERR_VAL_0X10) && (*temperature != ERR_VAL_0X19))
        {
            *temperature = ERR_NUM_10; 
        }
    }
    
    // stop
    I2C1CONbits.RCEN = 0;
    I2C1CONbits.PEN = 1;
    WHILE(I2C1CONbits.PEN, -15, I2Ctimeout)

    return I2Cerrcode;

I2Ctimeout:

    ++I2Cerrors;

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

    I2C1CON &= 0xFFE0;
    I2C1CONbits.PEN = 1;
    WHILE(I2C1CONbits.PEN, -16, fatal_error)

    __delay32(400);
    
    config_sensor();
    
    return I2Cerrcode;

fatal_error:

    I2Cdead = TRUE;

    config_sensor();

    return I2Cerrcode;
}

// function used for generating synthetic temperature data useful for testing
// the increase is done in raw value for a PT1000 and the value is 10 deg each 2 call 
void generateI2CTsensSynthetic(volatile int* temperature)
{
    
    // increase temp of 10 raw unit 
    if(synthTempCounter >= 2)
    {
        *temperature += 897;
        synthTempCounter = 0;
    }
    else
    {
        ++synthTempCounter;
    }
}
