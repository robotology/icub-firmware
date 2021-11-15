/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */


// mssp module clock frequency, expressed as kHz
#define	CLOCK	200	// (100, 400 or 1000)

// Note:
// pullup resistors values are in function of clock frequency

// Fosc frequency as Hz
#define FOSC	40000000UL

// I2C pins
#define SCL_PIN	RC0
#define SDA_PIN	RC1

// baud/slewrate calculation
#define BAUD (char)((FOSC/(4000UL*CLOCK))-2) // SSPADD = (FOSC/(4*Clock))-1
//#define BAUD 22
#define SLEWRATE 0 // 0 for 400KHz, 1 for 100KHz and 1MHz
//#define SLEWRATE 1

// function prototypes
void I2cInitMaster (void); // Init MSSP in Master mode
void I2cInitSlave (const unsigned char address); // Init MSSP in Slave mode (7 bit address)
void I2cWait(void); // waiting interrupt flag
void I2cWaitForIdle(void); // waiting bus in idle
void I2cStart(void); // start sequence
void I2cRepStart(void); // repeated start sequence
void I2cStop(void); // stop sequence
unsigned char I2cReadMaster(unsigned char ack); // master reads
unsigned char I2cWriteMaster(unsigned char I2cWriteData); // master writes
unsigned char I2cReadSlave(); // slave reads
void I2cWriteSlave(unsigned char data); // slave writes