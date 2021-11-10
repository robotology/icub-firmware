#include <xc.h>         /* XC8 General Include File */
#include "i2c.h"

// ---------------------------------------------------------------------------
// Init MSSP module in master mode
// ---------------------------------------------------------------------------
void I2cInitMaster(void)
	{
	SCL_PIN = 1;                    // SCL/SDA lines as input
	SDA_PIN = 1;
	SSP1CON1 = 0b00101000;          // Master mode: Clock=Fosc/(4*(SSPADD+1))
	SSP1CON2 = 0;
	SSP1STATbits.CKE = 0;           // SMbus compatible mode off
	SSP1ADD = BAUD;                 // calculated baudrate in i2c.h
	SSP1STATbits.SMP = SLEWRATE;    // slewrate
	PIR1bits.SSP1IF = 0;            // reset flag interrupt serial port
	PIR2bits.BCL1IF = 0;            // reset flag interrupt bus collision
	}

// ---------------------------------------------------------------------------
// Init MSSP module in slave mode
// address = 7bit address
// ---------------------------------------------------------------------------
void I2cInitSlave(const unsigned char address)
	{
        SCL_PIN = 1;                // SCL/SDA lines as inputs
	SDA_PIN = 1;
	SSP1CON1 = 0b00110110;      // slave mode, 7bit address
	SSP1CON2 = 0;
        SSP1CON2bits.SEN = 1;       // Clock stretching is enabled
        SSP1CON3bits.BOEN = 1;      // SSPBUF is updated and NACK is generated
                                    // for a received address/data byte,
                                    // ignoring the state of the SSPOV bit
                                    // only if the BF bit = 0
        SSP1CON3bits.SDAHT = 1;     // Minimum of 300 ns hold time on SDA after
                                    // the falling edge of SCL
        SSP1CON3bits.SBCDE = 1;     // Enable slave bus collision detect interrupts
        
	SSP1ADD = address;          // address, bits A7-A1
	SSP1STATbits.CKE = 0;       // SMbus compatible mode off
	SSP1STATbits.SMP =SLEWRATE; // slewrate
	PIR1bits.SSP1IF = 0;        // Reset flag interrupt serial port
	PIR2bits.BCL1IF = 0;        // Reset flag interrupt bus collision

        PIE2bits.BCL1IE = 1;        // Enable bus collision interrupts
        PIE1bits.SSP1IE = 1;        // Enable SSP interrupts
	INTCONbits.PEIE = 1;
        INTCONbits.GIE = 1;

	}

// ---------------------------------------------------------------------------
// wait current operation finish
// ---------------------------------------------------------------------------
void I2cWait(void)
	{
	while (!PIR1bits.SSP1IF)
		{
		continue;
		}
	PIR1bits.SSP1IF=0;
	}

// ---------------------------------------------------------------------------
// wait bus in idle status or no transmission
// ---------------------------------------------------------------------------
void I2cWaitForIdle(void)
	{
 	while ((SSP1CON2 & 0x1F) | SSP1STATbits.R_nW)
		{
 		continue;
		}
	}

// ---------------------------------------------------------------------------
// start i2c bus
// ---------------------------------------------------------------------------
void I2cStart(void)
	{
 	I2cWaitForIdle();
 	PIR1bits.SSP1IF = 0;
 	SSP1CON2bits.SEN = 1;   // start sequence
 	I2cWait();
	}

// ---------------------------------------------------------------------------
// repeated start
// ---------------------------------------------------------------------------
void I2cRepStart(void)
	{
	I2cWaitForIdle();
 	PIR1bits.SSP1IF = 0;
 	SSP1CON2bits.RSEN = 1;  // repeated start sequence
 	I2cWait();
	}

// ---------------------------------------------------------------------------
// stop i2c bus
// ---------------------------------------------------------------------------
void I2cStop(void)
	{
 	I2cWaitForIdle();
 	PIR1bits.SSP1IF = 0;
 	SSP1CON2bits.PEN = 1;   // stop sequence
 	SSP1CON2bits.ACKEN = 1; // acknowledge sequence
 	I2cWait();
	}

// ---------------------------------------------------------------------------
// slave writes
// ---------------------------------------------------------------------------
void I2cWriteSlave(unsigned char data)
	{
	SSP1CON1bits.CKP = 0;   // clock lock
	SSP1BUF = data;         // transfer data in the buffer
	PIR1bits.SSP1IF = 0;    // interrupt flag reset
	SSP1CON1bits.CKP = 1;   // clock unlock
	I2cWait();
	}

// ---------------------------------------------------------------------------
// slave reads
// ---------------------------------------------------------------------------
unsigned char I2cReadSlave(void)
	{
	char temp;
	I2cWait();
	temp = SSP1BUF;
	return temp;
	}

// ---------------------------------------------------------------------------
// master writes
// returns true when slave send ACK
// ---------------------------------------------------------------------------
unsigned char I2cWriteMaster(unsigned char data)
	{
 	I2cWaitForIdle();
 	SSP1BUF = data;
 	I2cWait();
 	return !SSP1CON2bits.ACKSTAT;
	}

// ---------------------------------------------------------------------------
// master reads
// returns the byte. if ack=1, sends acknowledge
// ---------------------------------------------------------------------------
unsigned char I2cReadMaster(unsigned char ack)
	{
	unsigned char data;
 	I2cWaitForIdle();
 	SSP1CON2bits.RCEN = 1;
 	PIR1bits.SSP1IF = 0;
 	I2cWait();
 	data = SSP1BUF;
 	if (ack)
		{
  		SSP1CON2bits.ACKDT = 0; // sends ACK
		}
 	else
		{
  		SSP1CON2bits.ACKDT = 1; // no ACK
		}
  	SSP1CON2bits.ACKEN = 1; // starts ACK routine
 	I2cWait();
 	return data;
	}

#undef	CLOCK
#undef	FOSC
#undef	BAUD
#undef	SLEWRATE
