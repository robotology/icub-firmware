#include<p30fxxxx.h>
#include "I2C.h"
//---------------------------------
//Function prototypes
//---------------------------------

//External functions
//==================
extern void Wait(unsigned int value);
extern const unsigned char AD7147_ADD[4];

static unsigned char ReceivedByte[4]={0,0,0,0};
//---------------------------------
//Function definitions
//---------------------------------


/********************************************************************************************************/
/****************** High level functions, need to be customised according to the device *****************/
/********************************************************************************************************/

void I2C_Init(unsigned char Channel)
{
	OC1CON= 0; 
	OC2CON= 0; 
	IC1CON= 0; 
	IC2CON= 0; 
	

	switch (Channel)
	{
            case CH0:
            {
                MCE_0output
                DE_0output
            }
            break;
            case CH1:
            {
                MCE_1output
                DE_1output
            }
            break;
	}
}


void I2C_test(unsigned char Channel)
{
	switch (Channel)
	{

            case CH0:
 	    {
                DE_0output
                MCO_0=1;
                Wait(I2Cbit);// Wait(I2Cbit);
                MCO_0=0;
                DO_0on;
    		Wait(I2Cbit);//Wait(I2Cbit);
    		MCO_0=1;
    		DO_0on;
    		Wait(I2Cbit);//Wait(I2Cbit);
    		MCO_0=0;
    		DO_0off;
    		Wait(I2Cbit);//Wait(I2Cbit);
            }
            break;
            case CH1:
            {
                DE_1output
                MCO_1=1;
                Wait(I2Cbit);//Wait(I2Cbit);
                MCO_1=0;
                DO_1on;
    		Wait(I2Cbit);//Wait(I2Cbit);
    		MCO_1=1;
    		DO_1on;
    		Wait(I2Cbit);//Wait(I2Cbit);
    		MCO_1=0;
    		DO_1off;
    		Wait(I2Cbit);//Wait(I2Cbit);	
            }
            break;
 	}   
}   

//---------------------------------
//WriteAD7147ViaI2C()
//---------------------------------
//Function that writes one byte via the I2C port . It sends first the device
//address including the write bit, then the register address and finally the
//data. The function returns "1" if successfull otherwise "0".
//--------------------------------------------------------------------------------
unsigned char WriteByteViaI2C(unsigned char Channel, unsigned char DeviceAddress, const unsigned char RegisterStartAddress, unsigned char DataBuffer)
{

    unsigned char ByteAddress;
    unsigned char AcknError;
    unsigned char DeviceAddressHeader;
    AcknError=1; //No error on initialisation

    //Add the write bit to the device address
    DeviceAddressHeader = DeviceAddress<<1 | I2C_WR;
    //Split the address in two bytes 
    ByteAddress = RegisterStartAddress & 0xFF;
  
    //Start the I2C transfer
    InitialiseI2CMaster(Channel);
    StartI2CMaster(Channel);
    //Send device address
    if (!SendByteI2CMaster(Channel,DeviceAddressHeader))
    {
        //Send register address if the acknowledgement is there
        if (!SendByteI2CMaster(Channel,ByteAddress))
        {
             SendByteI2CMaster(Channel,DataBuffer);
               
             StopI2CMaster(Channel);
             AcknError=0;
        } else //No acknowledgement was found therefore send the stop condition
        {
            StopI2CMaster(Channel);
            AcknError=0;
        }
    } else //No acknowledgement was found therefore send the stop condition
    {
        StopI2CMaster(Channel);
        AcknError=0;
    }
    return(AcknError);
}

//---------------------------------
//WriteAD7147ViaI2C()
//---------------------------------
//Function that writes  via the I2C port pair of 16 bits data. It sends first the device
//address including the write bit, then the register address and finally the
//data. The function returns "1" if successfull otherwise "0".
//--------------------------------------------------------------------------------
unsigned char WriteViaI2C(unsigned char Channel,unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToWrite, unsigned int *DataBuffer, const unsigned int OffsetInBuffer)
{
    unsigned int  DataToWrite;
    unsigned char LowByteAddress, HighByteAddress;
    unsigned char LowByteData, HighByteData;
    unsigned char r, AcknError;
    unsigned char DeviceAddressHeader;

    AcknError=1; //No error on initialisation

    //Add the write bit to the device address
    DeviceAddressHeader = DeviceAddress<<1 | I2C_WR;
    //Split the address in two bytes 
    HighByteAddress = (RegisterStartAddress & 0xFF00) >>8;
    LowByteAddress = RegisterStartAddress & 0x00FF;

    //Start the I2C transfer
    InitialiseI2CMaster(Channel);
    StartI2CMaster(Channel);
    //Send device address
    if (!SendByteI2CMaster(Channel,DeviceAddressHeader))
    {
        //Send register address if the acknowledgement is there
        if (!SendByteI2CMaster(Channel,HighByteAddress))
        {
            if (!SendByteI2CMaster(Channel,LowByteAddress))
            {
                //Perform block write
                for (r=0;r<NumberOfRegistersToWrite;r++)
                {
                    DataToWrite = DataBuffer[OffsetInBuffer+r];
                    LowByteData = DataToWrite & 0x00FF;
                    HighByteData = (DataToWrite & 0xFF00)>>8;
                    if (!SendByteI2CMaster(Channel,HighByteData))
                    {
                        SendByteI2CMaster(Channel,LowByteData);
                    } else //No acknowledgement was found therefore send the stop condition
                    {
                        StopI2CMaster(Channel);
                        AcknError=0;
                    }
                }
                //Stop transfer
                StopI2CMaster(Channel);
            } else //No acknowledgement was found therefore send the stop condition
            {
                StopI2CMaster(Channel);
                AcknError=0;
            }
        } else //No acknowledgement was found therefore send the stop condition
        {
            StopI2CMaster(Channel);
            AcknError=0;
        }
    } else //No acknowledgement was found therefore send the stop condition
    {
        StopI2CMaster(Channel);
        AcknError=0;
    }
    return(AcknError);
}



//---------------------------------
//WriteAD7147ViaI2C()
//---------------------------------
//Function that writes  via the I2C port pair of 16 bits data. It sends first the device
//address including the write bit, then the register address and finally the
//data. The function returns "1" if successfull otherwise "0".
//--------------------------------------------------------------------------------
unsigned char WriteViaI2C_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToWrite, unsigned int *DataBuffer, const unsigned int OffsetInBuffer)
{
    unsigned int  DataToWrite;
    unsigned char LowByteAddress, HighByteAddress;
    unsigned char LowByteData, HighByteData;
    unsigned char r, AcknError;
    unsigned char DeviceAddressHeader;

    AcknError=1; //No error on initialisation

    //Add the write bit to the device address
    DeviceAddressHeader = DeviceAddress<<1 | I2C_WR;
    //Split the address in two bytes 
    HighByteAddress = (RegisterStartAddress & 0xFF00) >>8;
    LowByteAddress = RegisterStartAddress & 0x00FF;

    //Start the I2C transfer
    InitialiseI2CMaster(Channel);
    StartI2CMaster_onSdaX(Channel, sdaNum);
    //Send device address
    if (!SendByteI2CMaster_onSdaX(Channel, sdaNum, DeviceAddressHeader))
    {
        //Send register address if the acknowledgement is there
        if (!SendByteI2CMaster_onSdaX(Channel, sdaNum, HighByteAddress))
        {
            if (!SendByteI2CMaster_onSdaX(Channel, sdaNum, LowByteAddress))
            {
                //Perform block write
                for (r=0;r<NumberOfRegistersToWrite;r++)
                {
                    DataToWrite = DataBuffer[OffsetInBuffer+r];
                    LowByteData = DataToWrite & 0x00FF;
                    HighByteData = (DataToWrite & 0xFF00)>>8;
                    if (!SendByteI2CMaster_onSdaX(Channel, sdaNum, HighByteData))
                    {
                        SendByteI2CMaster_onSdaX(Channel, sdaNum, LowByteData);
                    } else //No acknowledgement was found therefore send the stop condition
                    {
                        StopI2CMaster_onSdaX(Channel, sdaNum);
                        AcknError=0;
                    }
                }
                //Stop transfer
                StopI2CMaster_onSdaX(Channel, sdaNum);
            } else //No acknowledgement was found therefore send the stop condition
            {
                StopI2CMaster_onSdaX(Channel, sdaNum);
                AcknError=0;
            }
        } else //No acknowledgement was found therefore send the stop condition
        {
            StopI2CMaster_onSdaX(Channel, sdaNum);
            AcknError=0;
        }
    } else //No acknowledgement was found therefore send the stop condition
    {
        StopI2CMaster_onSdaX(Channel, sdaNum);
        AcknError=0;
    }
    return(AcknError);
}



//---------------------------------
//ReadByteViaI2C()
//---------------------------------
//Function that reads a Byte from  via the I2C port. It sends first the device
//address including the write bit, then the register address and finally reads data
//back. The function returns "1" if successfull otherwise "0". If an error occurs,
//Then the stop condition is sent.
//--------------------------------------------------------------------------------
unsigned char ReadByteViaI2C(unsigned char Channel, unsigned char SDAnum, unsigned char DeviceAddress, const unsigned char RegisterAddress, unsigned char *DataBuffer )
{
	
    unsigned char AcknError;
    unsigned char SDA_number=SDAnum; //for the palm
    unsigned char DeviceAddressHeader;

    AcknError=1; //No error on initialisation

    //Add the write bit to the device address
    DeviceAddressHeader=DeviceAddress<<1 | I2C_WR; //qui ci vuole I2C_WR o RD secondo me RD
   

    //Start the I2C transfer
    InitialiseI2CMaster(Channel);
    StartI2CMaster(Channel);

    //Send device address
    if (!SendByteI2CMaster(Channel,DeviceAddressHeader))
    {
        //Send register address
        if (!SendByteI2CMaster(Channel,RegisterAddress))
        {
                //Send the repeated start
                StartI2CMaster(Channel);
                //Send device address again changing the Rd/Wr bit
                DeviceAddressHeader = DeviceAddress<<1 | I2C_RD;
                if (!SendByteI2CMaster(Channel,DeviceAddressHeader))
                {
                    //Perform block read, but first,we need to know if we must send an ACKN or a NACK

                        ReceiveByteI2CMaster(Channel,NACK);
                        *DataBuffer=ReceivedByte[SDA_number];
                    //Stop transfer
                    StopI2CMaster(Channel);
                } else //No acknowledgement was found therefore send the stop condition
                {
                    StopI2CMaster(Channel);
                    AcknError=0;
                }
        } else //No acknowledgement was found therefore send the stop condition
        {
            StopI2CMaster(Channel);
            AcknError=0;
        }
    } else //No acknowledgement was found therefore send the stop condition
    {
        StopI2CMaster(Channel);
        AcknError=0;
    }
    return(AcknError);
}
//---------------------------------
//ReadViaI2C()
//---------------------------------
//Function that reads from  via the I2C port. It sends first the device
//address including the write bit, then the register address and finally reads data
//back. The function returns "1" if successfull otherwise "0". If an error occurs,
//Then the stop condition is sent.
//--------------------------------------------------------------------------------
unsigned char ReadViaI2C(unsigned char Channel, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToRead, unsigned int *DataBuffer1,unsigned int *DataBuffer2,unsigned int *DataBuffer3,unsigned int *DataBuffer4, const unsigned int OffsetInBuffer)
{
    unsigned char LowByteAddress, HighByteAddress;
    unsigned char LowByteData[4], HighByteData[4];
    unsigned char r, AcknError;
    unsigned char DeviceAddressHeader;

    AcknError=1; //No error on initialisation

    //Add the write bit to the device address
    DeviceAddressHeader=DeviceAddress<<1 | I2C_WR; //qui ci vuole I2C_WR o RD secondo me RD
    //Split the address in two bytes 
    HighByteAddress = (RegisterStartAddress & 0xFF00) >>8;
    LowByteAddress = RegisterStartAddress & 0x00FF;

    //Start the I2C transfer
    InitialiseI2CMaster(Channel);
    StartI2CMaster(Channel);

    //Send device address
    if (!SendByteI2CMaster(Channel,DeviceAddressHeader))
    {
        //Send register address
        if (!SendByteI2CMaster(Channel,HighByteAddress))
        {
            if (!SendByteI2CMaster(Channel,LowByteAddress))
            {
                //Send the repeated start
                StartI2CMaster(Channel);
                //Send device address again changing the Rd/Wr bit
                DeviceAddressHeader = DeviceAddress<<1 | I2C_RD;
                if (!SendByteI2CMaster(Channel,DeviceAddressHeader))
                {
                    //Perform block read, but first,we need to know if we must send an ACKN or a NACK
                    if (NumberOfRegistersToRead==1)
                    {
                        ReceiveByteI2CMaster(Channel,ACK);
                        HighByteData[0]=ReceivedByte[0];
						HighByteData[1]=ReceivedByte[1];
						HighByteData[2]=ReceivedByte[2];
						HighByteData[3]=ReceivedByte[3];
						
                        ReceiveByteI2CMaster(Channel,NACK);
                        LowByteData[0]=ReceivedByte[0];
                        LowByteData[1]=ReceivedByte[1];
                        LowByteData[2]=ReceivedByte[2];
                        LowByteData[3]=ReceivedByte[3];

                        DataBuffer1[OffsetInBuffer]=((HighByteData[0] & 0xFF)<<8) | LowByteData[0];
                       
                        DataBuffer2[OffsetInBuffer]=((HighByteData[1] & 0xFF)<<8) | LowByteData[1];
                       
                        DataBuffer3[OffsetInBuffer]=((HighByteData[2] & 0xFF)<<8) | LowByteData[2];
                        
                        DataBuffer4[OffsetInBuffer]=((HighByteData[3] & 0xFF)<<8) | LowByteData[3];
                        

                    } else
                    {
                        for (r=0;r<(NumberOfRegistersToRead-1);r++)
                        {
                            ReceiveByteI2CMaster(Channel,ACK);
                            HighByteData[0]=ReceivedByte[0];
                            HighByteData[1]=ReceivedByte[1];
                            HighByteData[2]=ReceivedByte[2];
                            HighByteData[3]=ReceivedByte[3];

                            ReceiveByteI2CMaster(Channel,ACK);
                            LowByteData[0]=ReceivedByte[0];
                            LowByteData[1]=ReceivedByte[1];
                            LowByteData[2]=ReceivedByte[2];
                            LowByteData[3]=ReceivedByte[3];

                           DataBuffer1[OffsetInBuffer+r]=((HighByteData[0] & 0xFF)<<8) | LowByteData[0];
                           
                           DataBuffer2[OffsetInBuffer+r]=((HighByteData[1] & 0xFF)<<8) | LowByteData[1];
                        
                           DataBuffer3[OffsetInBuffer+r]=((HighByteData[2] & 0xFF)<<8) | LowByteData[2];
                           
                           DataBuffer4[OffsetInBuffer+r]=((HighByteData[3] & 0xFF)<<8) | LowByteData[3];
                           
                        }
                        //Do the last read sending the NACK
                        ReceiveByteI2CMaster(Channel,ACK);
                        HighByteData[0]=ReceivedByte[0];
                        HighByteData[1]=ReceivedByte[1];
                        HighByteData[2]=ReceivedByte[2];
                        HighByteData[3]=ReceivedByte[3];

                        ReceiveByteI2CMaster(Channel,NACK);
                        LowByteData[0]=ReceivedByte[0];
                        LowByteData[1]=ReceivedByte[1];
                        LowByteData[2]=ReceivedByte[2];
                        LowByteData[3]=ReceivedByte[3];

                        DataBuffer1[OffsetInBuffer+NumberOfRegistersToRead-1]=((HighByteData[0] & 0xFF)<<8) | LowByteData[0];
                       
                        DataBuffer2[OffsetInBuffer+NumberOfRegistersToRead-1]=((HighByteData[1] & 0xFF)<<8) | LowByteData[1];
                        
                        DataBuffer3[OffsetInBuffer+NumberOfRegistersToRead-1]=((HighByteData[2] & 0xFF)<<8) | LowByteData[2];
                        
                        DataBuffer4[OffsetInBuffer+NumberOfRegistersToRead-1]=((HighByteData[3] & 0xFF)<<8) | LowByteData[3];
                        

                    }
                    //Stop transfer
                    StopI2CMaster(Channel);
                } else //No acknowledgement was found therefore send the stop condition
                {
                    StopI2CMaster(Channel);
                    AcknError=0;
                }
            } else //No acknowledgement was found therefore send the stop condition
            {
                StopI2CMaster(Channel);
                AcknError=0;
            }
        } else //No acknowledgement was found therefore send the stop condition
        {
            StopI2CMaster(Channel);
            AcknError=0;
        }
    } else //No acknowledgement was found therefore send the stop condition
    {
        StopI2CMaster(Channel);
        AcknError=0;
    }
    return(AcknError);
}



/********************************************************************************************************/
/*** Low level functions, do not change anything below this line, however check the valid clock level ***/
/********************************************************************************************************/
//ReadViaI2C()
//---------------------------------
//Function that reads from  via the I2C port. It sends first the device
//address including the write bit, then the register address and finally reads data
//back. The function returns "1" if successfull otherwise "0". If an error occurs,
//Then the stop condition is sent.
//--------------------------------------------------------------------------------
unsigned char ReadBurstViaI2C(unsigned char Channel, unsigned char SDAnum, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToRead, unsigned int *DataBuffer)
{
    unsigned char ByteAddress;
    unsigned char LowByteData, HighByteData;
    unsigned char r, AcknError;
    unsigned char SDA_number=SDAnum;
    unsigned char DeviceAddressHeader;

    AcknError=1; //No error on initialisation

    //Add the write bit to the device address
    DeviceAddressHeader=DeviceAddress<<1 | I2C_WR; //qui ci vuole I2C_WR o RD secondo me RD 
   
    ByteAddress = RegisterStartAddress & 0x00FF;

    //Start the I2C transfer
    InitialiseI2CMaster(Channel);
    StartI2CMaster(Channel);

    //Send device address
    if (!SendByteI2CMaster(Channel,DeviceAddressHeader))
    {
        //Send register address

            if (!SendByteI2CMaster(Channel,ByteAddress))
            {
                //Send the repeated start
                StartI2CMaster(Channel);
                //Send device address again changing the Rd/Wr bit
                DeviceAddressHeader = DeviceAddress<<1 | I2C_RD;
                if (!SendByteI2CMaster(Channel,DeviceAddressHeader))
                {
                    //Perform block read, but first,we need to know if we must send an ACKN or a NACK
                    if (NumberOfRegistersToRead==1)
                    {
                        ReceiveByteI2CMaster(Channel,ACK);
                        HighByteData=ReceivedByte[SDA_number];;

						
                        ReceiveByteI2CMaster(Channel,NACK);
                        LowByteData=ReceivedByte[SDA_number];;
                  
                        DataBuffer[0]=((HighByteData & 0xFF)<<8) | LowByteData;


                    } else
                    {
                        for (r=0;r<(NumberOfRegistersToRead-1);r++)
                        {
                            ReceiveByteI2CMaster(Channel,ACK);
                            HighByteData=ReceivedByte[SDA_number];;
                         
                            ReceiveByteI2CMaster(Channel,ACK);
                            LowByteData=ReceivedByte[SDA_number];;

                            DataBuffer[r]=((HighByteData & 0xFF)<<8) | LowByteData;
                            
                        }
                        //Do the last read sending the NACK
                        ReceiveByteI2CMaster(Channel,ACK);
                        HighByteData=ReceivedByte[SDA_number];;
                  
                        ReceiveByteI2CMaster(Channel,NACK);
                        LowByteData=ReceivedByte[SDA_number];;
                       

                        DataBuffer[NumberOfRegistersToRead-1]=((HighByteData & 0xFF)<<8) | LowByteData;

                    }
                    //Stop transfer
                    StopI2CMaster(Channel);
                } else //No acknowledgement was found therefore send the stop condition
                {
                    StopI2CMaster(Channel);
                    AcknError=0;
                }
            } else //No acknowledgement was found therefore send the stop condition
            {
                StopI2CMaster(Channel);
                AcknError=0;
            }

    } else //No acknowledgement was found therefore send the stop condition
    {
        StopI2CMaster(Channel);
        AcknError=0;
    }
    return(AcknError);
}




//---------------------------------
//InitialiseI2CMaster();
//---------------------------------
//Function that configures the I2C port of the ADuC841 in master mode.
//--------------------------------------------------------------------------------






void InitialiseI2CMaster(unsigned char Channel)
{
	switch (Channel)
	{
	case CH0:	
        {
		MCE_0output
	}
	break;
	case CH1:
	{
		MCE_1output;
	}
	break;
	}
/*modificated
    TRISFbits.TRISF6=0; //Write a "0" to CLK so that it becomes an output.
*/
    //I2CCON = 0xA8;		//Master mode
}


//---------------------------------
//StartI2CMaster();
//---------------------------------
//Function that implements the start condition of the I2C protocol. The start
//condition consists in a falling edge on SDA when SCL is high.
//--------------------------------------------------------------------------------
void StartI2CMaster(unsigned char Channel)
{
	switch (Channel)
	{
	case CH0:
	{	
	    DE_0input;
	    Wait(I2Cbit);
	    MCO_0 = 1;    //SCL high
	    Wait(I2Cbit);
	    Wait(I2Cbit);
	    DE_0output    //SDA as output	
	    DO_0off
	    Wait(I2Cbit);
	    MCO_0 = 0;    //SCL low
	    Wait(I2Cbit);	
	}    
	break;
	case CH1:
	{
	    DE_1input;
	    Wait(I2Cbit);
	    MCO_1 = 1;    //SCL high
	    Wait(I2Cbit);
	    Wait(I2Cbit);
	    DE_1output    //SDA as output
	    DO_1off
	    Wait(I2Cbit);
	    MCO_1 = 0;    //SCL low
	    Wait(I2Cbit);
	}
	break;
	}
  
}



void SetSdaReg(i2c_sda_num_t sdaNum, unsigned char input)
{
    switch(sdaNum)
    {
        case sda1:
        {
            DE1=input;
        }break;
        case sda2:
        {
            DE2=input;
        }break;
        case sda3:
        {
            DE3=input;
        }break;
        case sda4:
        {
            DE4=input;
        }break;
    };

}


void SetValReg(i2c_sda_num_t sdaNum, unsigned char val)
{
    switch(sdaNum)
    {
        case sda1:
        {
            DO1=val;
        }break;
        case sda2:
        {
            DO2=val;
        }break;
        case sda3:
        {
            DO3=val;
        }break;
        case sda4:
        {
            DO4=val;
        }break;
    };

}

void StartI2CMaster_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum)
{
	switch (Channel)
	{
	case CH0:
	{	
	    //DE_0input;
        SetSdaReg(sdaNum, 1);
        
	    Wait(I2Cbit);
	    MCO_0 = 1;    //SCL high
	    Wait(I2Cbit);
	  
	 
	    Wait(I2Cbit);
	    //DE_0output    //SDA as output	
        SetSdaReg(sdaNum, 0);

	    //DO_0off
        SetValReg(sdaNum, 0);
	    Wait(I2Cbit);
	    MCO_0 = 0;    //SCL low
	    Wait(I2Cbit);	
	}    
	break;
	case CH1:
	{
		;
	}
	break;
	}
  
}



//---------------------------------
//StopI2CMaster();
//---------------------------------
//Function that implements the stop condition of the I2C protocol. The stop
//condition consists in a rising edge on SDA when SCL is high.
//--------------------------------------------------------------------------------
void StopI2CMaster(unsigned char Channel)
{
	switch (Channel)
	{
	case CH0:	
        {
	    DE_0output  //SDA as output	
	    DO_0off  //SDA low
	    Wait(I2Cbit);
	    MCO_0 = 1;    //SCL high
	    Wait(I2Cbit);//Wait(I2Cbit);
	    DE_0input  //SDA as output	
	    //DO_0on   //SDA goes from low to high when SCL is already high,
	    //Wait(I2Cbit);//Wait(I2Cbit);
	}
	break;
	case CH1:
	{   
	    DE_1output  //SDA as output	
	    DO_1off  //SDA low
	    Wait(I2Cbit);//Wait(I2Cbit);
	    MCO_1 = 1;    //SCL high
	    Wait(I2Cbit);//Wait(I2Cbit);
	    DE_1input   //SDA goes from low to high when SCL is already high,
	    //Wait(I2Cbit);//Wait(I2Cbit);
	}	
	break;
	}
}




void StopI2CMaster_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum)
{
	switch (Channel)
	{
	case CH0:	
    {
	    //DE_0output  //SDA as output	
        SetSdaReg(sdaNum, 0);        
        
	    //DO_0off  //SDA low
        SetValReg(sdaNum, 0);
	    Wait(I2Cbit);
	    MCO_0 = 1;    //SCL high
	    Wait(I2Cbit);//Wait(I2Cbit);
	    //DE_0input  //SDA as output	
        SetSdaReg(sdaNum, 1);
	}
	break;
	case CH1:
	{   
	    ;
	}	
	break;
	}
}


//---------------------------------
//SendByteI2CMaster();
//---------------------------------
//Function that sends a byte to the I2C port and then read the acknowledgement 
//bit. If the acknowledgement is found, then the function returns "1" otherwise, 
//it returns "0".
//--------------------------------------------------------------------------------
unsigned char SendByteI2CMaster(unsigned char Channel,unsigned char ByteToSend)
{

    unsigned char i;
    unsigned char noack=0;
	
	switch (Channel)
	{
	case CH0:
	{
	    DE_0output  //SDAs as output
	    for (i=8; i>0; i--)
	    {
	        MCO_0 = 0;                //Reset SCL		
	        if (ByteToSend >> 7)
	        {
	            DO_0on;
	        } 
		else
	        {
	            DO_0off;
	        }
	    	Wait(0);
	        MCO_0 = 1;                //Set SCL
	        Wait(I2Cbit);//Wait(I2Cbit);
	        MCO_0 = 0;
	        if (i==1)
	        {
		   DE_0input;
		} 
		else
		{                //Reset SCL
	        Wait(I2Cbit);//Wait(I2Cbit);
	        ByteToSend<<=1;         //Rotate data
	        }   
	    }
	    DO_0off
	    DE_0input                //SDA becomes an input
	    MCO_0 = 0;                //Reset SCL	
	    Wait(I2Cbit);//Wait(I2Cbit);
	    MCO_0 = 1;                //Set SCL
	    noack=0;
	    Wait(I2Cbit);//Wait(I2Cbit);
	    MCO_0 = 0;
	    DE_0output   //SDA becomes an output   
	}
	break;
	case CH1:
	{
	    DE_1output  //SDAs as output
	    for (i=8; i>0; i--)
	    {
	        MCO_1 = 0;                //Reset SCL
	        if (ByteToSend >> 7)
	        {
	            DO_1on;
	        }
		else
	        {
	            DO_1off;
	        }
	    	Wait(0);
	        MCO_1 = 1;                //Set SCL
	        Wait(I2Cbit);//Wait(I2Cbit);
	        MCO_1 = 0;
	        if (i==1)
	        {
                    DE_1input;
		}
                else
		{                //Reset SCL
	        Wait(I2Cbit);//Wait(I2Cbit);
	        ByteToSend<<=1;         //Rotate data
	        }
	    }
	    DO_1off
	    DE_1input                //SDA becomes an input
	    MCO_1 = 0;                //Reset SCL
	    Wait(I2Cbit);//Wait(I2Cbit);
	    MCO_1 = 1;                //Set SCL
	    noack=0;
	    Wait(I2Cbit);//Wait(I2Cbit);
	    MCO_1 = 0;
	    DE_1output   //SDA becomes an output
	}
	break;
	}
    return(noack);
}






unsigned char SendByteI2CMaster_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum, unsigned char ByteToSend)
{

    unsigned char i;
    unsigned char noack=0;
	
	switch (Channel)
	{
	case CH0:
	{
	    //DE_0output  //SDAs as output
        SetSdaReg(sdaNum, 0);
        
	    for (i=8; i>0; i--)
	    {
	        MCO_0 = 0;                //Reset SCL		
	        /*@@@@@@@@@@@@
	            DO = ByteToSend >> 7;	//Send data to SDA pin
	        */
	        if (ByteToSend >> 7)
	        {
	            //DO_0on;
                SetValReg(sdaNum, 1);
	        } 
			else
	        {
	            //DO_0off;
                SetValReg(sdaNum, 0);
	        }
	    	Wait(0);
		
	     //   Wait(I2Cbit);//Wait(I2Cbit);
	        MCO_0 = 1;                //Set SCL
	        Wait(I2Cbit);//Wait(I2Cbit);
	        MCO_0 = 0;
	        if (i==1)
	        {
		        //DE_0input;
                SetSdaReg(sdaNum, 1);
		    } 
		    else
		    {                //Reset SCL
	        Wait(I2Cbit);//Wait(I2Cbit);
	        ByteToSend<<=1;         //Rotate data
	        }   
	    }
	    //DO_0off
        SetValReg(sdaNum, 0);
        
	    //DE_0input                //SDA becomes an input
        SetSdaReg(sdaNum, 1);
        
	    MCO_0 = 0;                //Reset SCL
	
	    Wait(I2Cbit);//Wait(I2Cbit);
	    MCO_0 = 1;                //Set SCL

		noack=0;  
	
	     Wait(I2Cbit);//Wait(I2Cbit);
	    MCO_0 = 0;
	    //DE_0output   //SDA becomes an output
        SetSdaReg(sdaNum, 0);
	   
	}
	break;
	case CH1:
	{
        ;
	}
	break;
	}
    return(noack);
}


//---------------------------------
//ReceiveByteI2CMaster();
//---------------------------------
//Function that reads one byte from the I2C port. If we do continuous read, 
//then the acknowledgement must be "0" excepted for the last read sequence which
//it must be "1".
//--------------------------------------------------------------------------------
void ReceiveByteI2CMaster(unsigned char Channel,unsigned char ackn)     // changed bit with unsigned char
{	
    unsigned char i;
	switch (Channel)
	{
	case CH0:
	{
            DE_0input     //SDA becomes an input
	    MCO_0 = 0;    //Reset SCL
	    for (i=8; i>0; i--)
	    {		
	        Wait(I2Cbit);//Wait(I2Cbit);
		ReceivedByte[0] <<= 1;      //Rotate data
	        ReceivedByte[1] <<= 1;      //Rotate data
	        ReceivedByte[2] <<= 1;      //Rotate data
	        ReceivedByte[3] <<= 1;      //Rotate data
	        MCO_0 = 1;                //Set SCL	
	        ReceivedByte[0] |= PORTDbits.RD0;//D1;   //Read SDA -> data 
 	        ReceivedByte[1] |= PORTDbits.RD2;//D2;   //Read SDA -> data   
	        ReceivedByte[2] |= PORTDbits.RD3;//D3;   //Read SDA -> data   
	        ReceivedByte[3] |= PORTDbits.RD1;//D4;   //Read SDA -> data   
	        MCO_0 = 0;                //Reset SCL
	    }
	    DE_0output               //SDA becomes an output
	    if (ackn==1)
	    {
	        DO_0on
	    } 
		else DO_0off
	    Wait(I2Cbit);//Wait(I2Cbit);
	    MCO_0 = 1;    //Set SCL
	    Wait(I2Cbit);//Wait(I2Cbit);
	    MCO_0 = 0;    //Reset SCL
	}
	break;
	case CH1:
	{
            DE_1input     //SDA becomes an input
	    MCO_1 = 0;    //Reset SCL
	    for (i=8; i>0; i--)
	    {
	        Wait(I2Cbit);//Wait(I2Cbit);
	        ReceivedByte[0] <<= 1;      //Rotate data
	        ReceivedByte[1] <<= 1;      //Rotate data
	        ReceivedByte[2] <<= 1;      //Rotate data
	        ReceivedByte[3] <<= 1;      //Rotate data
                MCO_1 = 1;
	        ReceivedByte[0] |= DO5;   //Read SDA -> data    MDI
	        ReceivedByte[1] |= DO5;   //Read SDA -> data    MDI
	        ReceivedByte[2] |= DO5;   //Read SDA -> data    MDI
	        ReceivedByte[3] |= DO5;   //Read SDA -> data    MDI
	        MCO_1 = 0;                //Reset SCL
	    }
	    DE_1output               //SDA becomes an output
	    if (ackn==1)
	    {
	        DO_1on
	    } 
	    else DO_1off
	    Wait(I2Cbit);//Wait(I2Cbit);
	    MCO_1 = 1;    //Set SCL
	    Wait(I2Cbit);//Wait(I2Cbit);
	    MCO_1 = 0;    //Reset SCL
	}
	break;
	}
    
}

