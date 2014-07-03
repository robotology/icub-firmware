#define I2C_WR		0b0
#define I2C_RD		0b1
#define	ACK			0
#define	NACK		1
#define I2Cbit    1 //the duration of a bit 10 is about 100Khz, 1 is about 500KHz

//-------------------------------------
//Pin assignment for the I2C interface

#define CH0     0
#define CH1     1

#define	MCO_0	      PORTFbits.RF6    // CLK in I2C mode 
#define	DO1           PORTDbits.RD0 //0x01    //LATDbits.LATD0    // SDA1 in I2C mode
#define	DO2           PORTDbits.RD2 //0x03    //LATDbits.LATD2    // SDA2 in I2C mode
#define	DO3           PORTDbits.RD3 //0x04    //LATDbits.LATD3    // SDA3 in I2C mode
#define	DO4           PORTDbits.RD1 //0x02    //LATDbits.LATD1    // SDA4 in I2C mode

#define MCE_0         TRISFbits.TRISF6 // SCO  Input/Output	 
#define DE1           TRISDbits.TRISD0 // SDA1 Input/Output	
#define DE2           TRISDbits.TRISD2 // SDA2 Input/Output	
#define DE3           TRISDbits.TRISD3 // SDA3 Input/Output	
#define DE4           TRISDbits.TRISD1 // SDA4 Input/Output	

//#warning "change to 0x0f" 
#define DO_0on        PORTD=0x0f; //DO1=1; DO4=1; DO2=1; DO3=1; 
#define DO_0off       PORTD=0x00; //DO1=0; DO4=0; DO2=0; DO3=0; 
#define DE_0input     DE1=1; DE2=1; DE3=1; DE4=1; 
#define DE_0output    DE1=0; DE2=0; DE3=0; DE4=0; 
#define MCE_0input    MCE_0=1;
#define MCE_0output   MCE_0=0;


#define MCO_1      	  PORTEbits.RE1    // CLK in I2C mode
#define	DO5	          PORTEbits.RE0 //0x01    //LATDbits.LATD0    // SDA1 in I2C mode

	
#define MCE_1      	  TRISEbits.TRISE1 // SCO  Input/Output
#define DE5        	  TRISEbits.TRISE0 // SDA1 Input/Output


#define DO_1on        PORTEbits.RE0=0x01; //DO1=1; DO4=1; DO2=1; DO3=1;
#define DO_1off       PORTE &=0xFFFE; //DO1=0; DO4=0; DO2=0; DO3=0;
#define DE_1input     DE5=1; 
#define DE_1output    DE5=0; 
#define MCE_1input    MCE_1=1;
#define MCE_1output   MCE_1=0;



//======================================= I2C interface initialization ========================

//TRISFbits.TRISF6=0; // CLK in I2C mode

#define I2CCLKPW		1             //This value define the CLK frequency
#define REGISTER_LENGTH	16

typedef enum
{
    sda1 = 0,
    sda2 = 1,
    sda3 = 2,
    sda4 = 3,
}i2c_sda_num_t;

#define i2c_sda_num_max 4



void I2C_Init(unsigned char Channel);
void I2C_test(unsigned char Channel);
//Local functions
//===============
//High level functions
unsigned char WriteViaI2C(unsigned char Channel, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToWrite, unsigned int *DataBuffer, const unsigned int OffsetInBuffer);
extern unsigned char WriteByteViaI2C(unsigned char Channel, unsigned char DeviceAddress, const unsigned char RegisterStartAddress, unsigned char DataBuffer);
unsigned char WriteViaI2C_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToWrite, unsigned int *DataBuffer, const unsigned int OffsetInBuffer);
unsigned char ReadBurstViaI2C(unsigned char Channel, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToRead, unsigned int *DataBuffer);
extern unsigned char ReadViaI2C(unsigned char Channel,unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToRead, unsigned int *DataBuffer1, unsigned int *DataBuffer2, unsigned int *DataBuffer3, unsigned int *DataBuffer4, const unsigned int OffsetInBuffer);
unsigned char ReadByteViaI2C(unsigned char Channel, unsigned char DeviceAddress, const unsigned char RegisterAddress, unsigned char *DataBuffer );
//Low level functions
void InitialiseI2CMaster(unsigned char Channel);
void StartI2CMaster(unsigned char Channel);
void StopI2CMaster(unsigned char Channel);
unsigned char SendByteI2CMaster(unsigned char Channel,unsigned char ByteToSend);
void ReceiveByteI2CMaster(unsigned char Channel,unsigned char ackn);  //modificated bit with unsigned char

void SetValReg(i2c_sda_num_t sdaNum, unsigned char val);
void SetSdaReg(i2c_sda_num_t sdaNum, unsigned char input);
void StartI2CMaster_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum);
void StopI2CMaster_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum);
unsigned char SendByteI2CMaster_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum, unsigned char ByteToSend);
