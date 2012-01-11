/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Marco Maggiali
 * email:  marco.maggiali@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/*	@file 	main.c
	@brief 	this file defines mais application.
	@author  marco.maggiali@iit.it
	@date	- 07/11/2011 
*/
//  Firmware for BCS board (Battery current sensing)
//  Author M.Maggiali
//  Rev. 1.0 del 09/11/2011 
//
//  Tested on dsPIC30F4011
//  pic30-gcc V3.30
//  MPLAB IDE ver 8.76
// 
//  Revisiom List
//  Revision 0.0: 
// TODO: CAN
// TODO: Inizializzazione lettura di corrente
//
//
//  Revision 1.0: 
// TODO: CAN but it seems to be useles
// Inizializzazione lettura di corrente e tensione a 48V: DONE 
//

#include<p30f4011.h>
#include <uart.h>

#include "T1.h"
#include "PIN.h"
#include <timer.h>
#include <libpic30.h>
#include "AD.h"
#include <adc10.h>
#include "eeprom.h"
// inizializzazione bit di configurazione (p30f4011.h)
//_FOSC(CSW_FSCM_OFF & EC_PLL8); 
  // Clock switching disabled Fail safe Clock Monitor disabled
  // External clock with PLL x8 (10MHz*8->Fcycle=80/4=20MIPS)
//_FWDT(WDT_OFF);      // WD disabled
//
//_FBORPOR(MCLR_EN & PWRT_64 & PBOR_ON & BORV_27);  // BOR 2.7V POR 64msec
//_FGS(CODE_PROT_OFF); // Code protection disabled



// Enable this macro if you want to plot in DMCI the value of the current and voltage
//#define PLOT_DATA



//------------------------------------------------------------------------
//								Global variables
//------------------------------------------------------------------------
struct s_eeprom _EEDATA(1) ee_data = 
{
  0x0,           // EE_B_EEErased             :1
  0x0,           // EE_B_EnableWD             :1
  0x1,           // EE_B_EnableBOR            :1
  0x05,          // EE_CAN_BoardAddress;      :8
  0x01,          // EE_CAN_MessageDataRate    :8
  0x04,          // EE_CAN_Speed;             :8
  {'B','C','S',' ',' ','B','o','a','r','d',' ',' '},
    64,          // EE_VoltageOffset          :8
     0,          // EE_CurrentOffset          :16
     1,          // EE_Time    in s           :8
  0x0000 // Checksum
};

// Board Configuration image from EEPROM
struct s_eeprom BoardConfig = {0}; 


#ifdef PLOT_DATA

unsigned int PLOT[256];
unsigned int PLOT1[256];
unsigned int PLOT2[256];
unsigned int counter=0;

#endif

volatile char flag=0;
unsigned int BitToSend;    // number of bit to be send
//unsigned int stagecomplete;  
unsigned int _board_ID=2;
unsigned int TIMER_VALUE=0x1100;//1s Timer duration
const unsigned int TIME_INTERVAL=1;// 0->1s 1->2s 2->4s ....
unsigned int INTERVAL; //3600>>TIME_INTERVAL; 
/* Received data is stored in array Buf */
unsigned int CURRENT;  //AN0
unsigned int OLD_CURRENT;
int CUR_OFFSET=7; //val-512 is the value read from the AD when no current is passing trought the board.
int cur=0; //the signed value of the current in ADticks-CUR_OFFSET-512;
unsigned long VOLTAGE ; //AN1 
unsigned long OLD_VOLTAGE ; // 
unsigned int REALMAXVOLTAGE=64; //V
unsigned int MAXVOLTAGE=66; //the real max voltage (it depends by the board)
unsigned int DCDC_VOLTAGE;//AN2
long CHARGE; //charge value in %
long CHECKSUM;
const long battery_cap=9216000;
const long battery_cap_div100=92160;
volatile long charge; //20Ah  =>  128 (128 ADticks is 20A) x 3600 (s->h) x 20 (20Ah is the charge capacity of the battery)   
volatile unsigned int sleep=1;
char Buf[20];
char CUR[4]={' ','C',' ','\0'};
char VCC[4]={' ', 'V',' ','\0'};
char DCDC[7]={' ','D','C','D','C',' ','\0'};
char CHRG[5]={' ','C','H',' ','\0'}; 
char CKS[5]={' ','C','K',' ','\0'}; 
char END_LINE[2]={'\n','\0'};
char CUR_CHAR[6];
char VCC_CHAR[6];
char CHARGE_CHAR[8];
char CHECKSUM_CHAR[8];
char * Receiveddata = Buf;
unsigned int  CHAR_RECEIVED=8;
char read[8];
volatile 	 int not_sleeping=0;
// static unsigned int value=0;
//------------------------------------------------------------------------
//								Function prototypes
//------------------------------------------------------------------------

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void);        
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);  
void __attribute__((interrupt, no_auto_psv)) _C1Interrupt(void);  
void __attribute__((__interrupt__)) _U2RXInterrupt(void);
void Wait(int value);
void UIntToString(char * C, unsigned int val, unsigned int N);
void IntToString(char * C, long val, unsigned int N);
unsigned int CharToUInt(char * c,unsigned int N);
long chargeCalculator (int A,int V);
//
//------------------------------------------------------------------------------ 
//								External functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// 								Interrrupt routines
//------------------------------------------------------------------------------
void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    flag=1;
    WriteTimer1(0x0);
   // LED1=~LED1;
    _T1IF = 0; 
      	
            
   // IFS0=IFS0 & 0xFFF7;
}
void __attribute__((__interrupt__)) _T2Interrupt(void)
{
   
    WriteTimer2(0x0);
    _T2IF = 0; 
   // IFS0=IFS0 & 0xFFF7;
}
/* This is UART1 receive ISR */

void __attribute__((interrupt, , no_auto_psv))  _U2TXInterrupt(void)

{  

   IFS1bits.U2TXIF = 0;

}  
void __attribute__((interrupt, , no_auto_psv)) _U2RXInterrupt(void)

{
  int i=0;
  char MENU[14]={'S','e','t','t','i','n','g',' ','B','C','S','\n','\0'};
  char Am[20]=" current offset   ";
  char Vm[20]=" value read at 48V";
  char Tm[20]=" time interval    ";
  char setted [8]={'S','E','T','T','E','D','\n','\0'};
  char finish[31]=" finished and saved to eeprom"  ;
  char pressA[19]="press A to set the";
  char pressM[20]="press M for MENU  ";
  char pressV[19]="press V to set the";
  char pressT[21]="press Txx to set the";
  char Tmax  [18]=":xx from 0 to 15";
  char pressS[28]="press S to finish and SAVE";
  char error[23]="error reading character";
  
  IFS1bits.U2RXIF = 0;
	
		Am[18]='\n';
		Vm[18]='\n';
		Am[19]='\0';
		Vm[19]='\0';
		Tm[19]='\0';
	    pressA[18]='\0';
	    pressV[18]='\0';
	    pressS[26]='\n';
	    pressS[27]='\0';
	    pressM[18]='\n';
	    pressM[19]='\0';
	    pressT[20]='\0';


   /* Read the receive buffer till atleast one or more character can be
      read */ 
      read[0]='0';
      i=getsUART2(CHAR_RECEIVED,read,800);
      if (sleep==1)
      {
   		sleep=0;
   	    not_sleeping=0;

   		OLD_CURRENT=CURRENT; //the CURRENT read before exiting the sleep mode
   		OLD_VOLTAGE=(VOLTAGE*MAXVOLTAGE)/REALMAXVOLTAGE;
   		putsUART2((unsigned int *) MENU);
   		putsUART2((unsigned int *) pressM);	
   		putsUART2((unsigned int *) pressA);	
   		putsUART2((unsigned int *) Am);	
   		putsUART2((unsigned int *) pressV);
   	    putsUART2((unsigned int *) Vm);
   	    putsUART2((unsigned int *) pressT);
   	    putsUART2((unsigned int *) Tm);
        Tmax[16]='\n';
	    Tmax[17]='\0';
   	    putsUART2((unsigned int *) Tmax);
   	    
   		putsUART2((unsigned int *) pressS);
   		while (BusyUART2());
 	  }

  	if (read[0]=='A')
  	{		
		Am[18]=0x20;
		CUR_OFFSET=(OLD_CURRENT+CUR_OFFSET)-512;//CharToUInt(read,4);  	
		putsUART2((unsigned int *) Am);
		putsUART2((unsigned int *) setted);
		putsUART2((unsigned int *) pressM);	
		putsUART2((unsigned int *) pressS);
		while (BusyUART2());
  	}	
  	if (read[0]==0x00)
  	{
  	 putsUART2((unsigned int *) error);
  	 while (BusyUART2());
  	}	
  	if (read[0]=='V')
  	{
		Vm[18]=0x20;
		REALMAXVOLTAGE=(49104/OLD_VOLTAGE);// (744*48)*66/48  	
		putsUART2((unsigned int *) Vm);
		putsUART2((unsigned int *) setted);
		putsUART2((unsigned int *) pressM);		
		putsUART2((unsigned int *) pressS);
		while (BusyUART2());
  	}
    if (read[0]=='T')
  	{		
		Tm[18]=0x20;		  
		
		INTERVAL=CharToUInt(read,2);
		TIMER_VALUE=0x1100*INTERVAL; 
		CloseTimer1();
		T1_Init(TIMER_VALUE); 	
		putsUART2((unsigned int *) Tm);
		putsUART2((unsigned int *) setted);
		putsUART2((unsigned int *) pressM);	
		putsUART2((unsigned int *) pressS);
		while (BusyUART2());
  	}	
  	if (read[0]=='M') //
  	{
  	   	putsUART2((unsigned int *) MENU);
  	   	putsUART2((unsigned int *) pressM);
   		putsUART2((unsigned int *) pressA);	
   		putsUART2((unsigned int *) Am);	
   		putsUART2((unsigned int *) pressV);
   	    putsUART2((unsigned int *) Vm);
   	    putsUART2((unsigned int *) pressT);
   	    putsUART2((unsigned int *) Tm);
   	    putsUART2((unsigned int *) Tmax);
   		putsUART2((unsigned int *) pressS);
	}
	if (read[0]=='S') //go in sleep mode until another setting is sent
  	{
	  	sleep=1;
	  	finish[30]='\0';
	  	putsUART2((unsigned int *) finish);
	  	BoardConfig.EE_CurrentOffset=CUR_OFFSET;
	  	BoardConfig.EE_Time=INTERVAL;
	  	BoardConfig.EE_VoltageOffset=REALMAXVOLTAGE;
	  	SaveEepromBoardConfig();
	  	while (BusyUART2());
	}

}
//------------------------------------------------------------------------
//									MAIN
//------------------------------------------------------------------------
int main(void)
{
    //------------------------------------------------------------------------
    //									Variables   
    //------------------------------------------------------------------------
    unsigned char i;
    unsigned int m;
	 int TIMEOUT=0;

    //=================================== === Serial Interface =================================
unsigned int baudvalue;   
unsigned int U2MODEvalue;
unsigned int U2STAvalue; 
CloseUART2();
/* Configure uart2 receive and transmit interrupt */
    ConfigIntUART2(UART_RX_INT_EN & UART_RX_INT_PR6 & 
                   UART_TX_INT_EN & UART_TX_INT_PR2);
/* Configure UART2 module to transmit 8 bit data with one stopbit. Also Enable loopback mode  */
     baudvalue =15; //45;//019.2 23;//=38.4 15= 57.6 //
     U2MODEvalue = UART_EN & UART_IDLE_CON & 
                  UART_EN_WAKE & UART_DIS_LOOPBACK  &
                  UART_DIS_ABAUD & UART_NO_PAR_8BIT  &
                  UART_1STOPBIT;
    U2STAvalue  = UART_INT_TX_BUF_EMPTY &  
                  UART_TX_PIN_NORMAL &
                  UART_TX_ENABLE & UART_INT_RX_CHAR &
                  UART_ADR_DETECT_DIS &
                  UART_RX_OVERRUN_CLEAR;
    OpenUART2(U2MODEvalue, U2STAvalue, baudvalue);

    unsigned int result;
	unsigned int mean;
	
	AD_Init();             
    ADCON1bits.SAMP = 0;

/* Load transmit buffer and transmit the same till null character is encountered */
  //   putsUART1 ((unsigned int *) _additional_info);
	
    
 

	//------------------------------------------------------------------------
    //							Variable init
    //------------------------------------------------------------------------
	charge=battery_cap;
	INTERVAL=3600>>TIME_INTERVAL;
	RecoverConfigurationFromEEprom();
	REALMAXVOLTAGE= BoardConfig.EE_VoltageOffset;
	CUR_OFFSET= BoardConfig.EE_CurrentOffset;
	TIMER_VALUE=BoardConfig.EE_Time*0x1100;
	
   //------------------------------------------------------------------------
    //								Peripheral init
    //------------------------------------------------------------------------
    T1_Init(TIMER_VALUE); //enable RTC
  //  T2_Init(TIMER_VALUE);
    //Read Silicon versions to check communication, It should read 0xE622
	PINs_Init();	
	
    i=0;
    m=32;

#ifdef PLOT_DATA
    counter=0;
#endif

    LED1=0;
    for (;;)
    {
	  
	   if (sleep) 
	   {
		   Sleep();
	   }
       if (flag==1)
        {
	       	LED1=1;
            flag=0;
	      	/// ADC conversion
	      	i=0;
	      	mean=0;
	      	result=0;
		  //      LED1=1;		    
	        SetChanADC10(ADC_CH0_POS_SAMPLEA_AN0 & ADC_CH0_NEG_SAMPLEA_NVREF);
	        ADCON1bits.DONE = 0;
			ADCON1bits.SAMP = 1;	
		    Wait(10);
			ConvertADC10();
	        while(BusyADC10());
	        CURRENT=ReadADC10(0)-CUR_OFFSET;
	        SetChanADC10(ADC_CH0_POS_SAMPLEA_AN1 & ADC_CH0_NEG_SAMPLEA_NVREF);
			ADCON1bits.DONE = 0;
			ADCON1bits.SAMP = 1;
			Wait(20);
			ConvertADC10();	
	        while(BusyADC10());
			VOLTAGE=ReadADC10(0);
			VOLTAGE=(VOLTAGE*REALMAXVOLTAGE)/MAXVOLTAGE;

			cur=CURRENT;		
			cur=cur-512;
			cur=(cur>>1)<<1;  //to remove the noise coming from the electronics 
		    
	 		charge=chargeCalculator(cur,VOLTAGE);
	 		
	 		if ((battery_cap-charge)>0) 
	 			CHARGE= ((battery_cap-charge)*100/battery_cap_div100); 
	 		else 
	 			CHARGE=0;
	 		
	 		CHECKSUM=CURRENT+VOLTAGE+CHARGE;
	 			 				
			IntToString(CHARGE_CHAR,CHARGE,5);	
	    	UIntToString(CUR_CHAR,CURRENT,4);
			UIntToString(VCC_CHAR,VOLTAGE,4);
			IntToString(CHECKSUM_CHAR,CHECKSUM,5);
if (sleep)
{
			putsUART2((unsigned int *) CUR);
	    	putsUART2((unsigned int *) CUR_CHAR); 
	    	putsUART2((unsigned int *) VCC); 
	    	putsUART2((unsigned int *) VCC_CHAR); 
	    	putsUART2((unsigned int *) CHRG); 
	    	putsUART2((unsigned int *) CHARGE_CHAR);	    	
	    	putsUART2((unsigned int *) CKS); 
	    	putsUART2((unsigned int *) CHECKSUM_CHAR);
	    	putsUART2((unsigned int *) END_LINE); //must be left	
	    	while (BusyUART2() && TIMEOUT<2000)
	    	{
		    	TIMEOUT++;
	    	}
	    	TIMEOUT=0;
}	    	
	if (not_sleeping<=10)
		   	{
			  not_sleeping++; 	
		   	}
		   	else
		   	{ 
		      Nop();
			  not_sleeping=0;
			  Nop();
			  sleep=1;
		   	}	
			LED1=0;
#ifdef PLOT_DATA

			PLOT[counter]=CURRENT;
			PLOT1[counter]=VOLTAGE;
//			PLOT2[counter]=DCDC_VOLTAGE;
		    counter++;
			if (counter ==254) 
			{
				counter=0;
			}
#endif
        }//if (flag==1)    
    }//for(;;)
}//main


// ------------------------------------------------------------------------
//					This is just a simple delay routine.
//------------------------------------------------------------------------
void Wait(int value)    
{
	Nop();
    while (value>0)
   	    {
	   	     Nop();
        	 value--;
        }	 
}//Wait();

//* it converts an integer to a string c, where val is the value to be converted and N is the number of digit
void UIntToString(char * c, unsigned int val, unsigned int N)
{
	unsigned int i=0;
	unsigned int data;
	for (i = 0; i < N; i++)
	{
		data = val % 10;
		val /= 10;
		c[N-1-i] = data + 48;
	}
	c[N+1]='\0';
}	

// c must be N+3 large 
void IntToString(char * c, long val, unsigned int N) 
{
	unsigned int i=0;
	unsigned int data;
	
	if (val>0)
	{
		c[0] = '+';
	}
	else
	{
		c[0]= '-';
	}
	for (i = 0; i < N; i++) 
	{
		data = val % 10;
		val /= 10;
		c[N-i] = data + 48;
	}
	c[N+2]='\0';
}	

//
//
// where:
// A is the current read from the AD after subtracting the Vref and the Offset
// V is the voltage read from the AD 
// it returns the charge calculated    
long chargeCalculator (int A,int V)
{
   const long p1 = -36864;
   const long p2 = 30505000;
   const long p3 = -102010;
   const long p4 = 80293000;
   const long p5 = -31933;
   const long p6 =  30835000;
   long Charge=0;
   int OFFSET=0;
 
  if (A<0) A=0; //during the charging phase I'll consider current=0;
   
  OFFSET=((64-A)*24)/100; //the curve is calculated for A=10A =>64tics

  // the curve is approximated with three lines 
  	
  if (V>=765+OFFSET) 
  Charge = p1*(V-OFFSET) + p2; 
 
  if ((V<765+OFFSET)&& (V>=706+OFFSET))  
  Charge = p3*(V-OFFSET) + p4; 
 
  if (V<(706+OFFSET))
  Charge = p5*(V-OFFSET) + p6; 

  return Charge;
}
 
// where read is the char* to be converted in unsigned int and N is the number of digits. 
// The char* has in c[0] a letter and should have N digits.
unsigned int CharToUInt(char * c,unsigned int N)
{
	unsigned int i=0;
	unsigned int data;
	unsigned int m[5]={1,10,100,1000,10000};  
	
	
	for (i = 0; i < N; i++) 
	{
		data = data + ((c[i+1]-48)*m[N-1-i]); //usually c[0] is used for the command then we start from c[1] 	
	}
	return data;
}	 
//calculation of the CHARGE of the battery
//
//   charge=charge-(cur*TIME_INTERVAL)  
//
//   where:
//   
//   TIME_INTERVAL is the time between 2 acquisitions in s 
//	   
//   cur=(CURRENT-512-CURRENT_OFFSET);  //current in ADtick removing the Vref compensated by the CURRENT_OFFSET     
//
//	 charge is the value of the charge in ADtick*s  

/*							  // but 	
 			charge=charge-(cur*INTERVAL);  
 			if (charge<0)
 			{
	 		 	//if the charge capacity is less then 0 then there is something wrong in the integration
	 		    CHARGE= (charge*100/battery_cap_div100); 
	 		}
	 		else
	 		{
	 	    	CHARGE= (charge*100/battery_cap_div100); 
	 		}
*/	 
