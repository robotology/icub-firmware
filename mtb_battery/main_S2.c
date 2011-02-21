
#include<p30f4011.h>
#include <uart.h>
#include "serial_interface.h"
#include "AD7147RegMap.h"
#include "I2C.h"
#include "T1.h"
#include "LED.h"
#include "options.h"
#include <timer.h>
#include <libpic30.h>
#include <adc10.h>
#include <stdio.h>


// inizializzazione bit di configurazione (p30f4013.h)
_FOSC(CSW_FSCM_OFF & EC_PLL8); 
  // Clock switching disabled Fail safe Clock Monitor disabled
  // External clock with PLL x8 (10MHz*8->Fcycle=80/4=20MIPS)
_FWDT(WDT_OFF);      // WD disabled
//
_FBORPOR(MCLR_EN & PWRT_64 & PBOR_ON & BORV_27);  // BOR 2.7V POR 64msec
_FGS(CODE_PROT_OFF); // Code protection disabled
//------------------------------------------------------------------------
//								Global variables
//------------------------------------------------------------------------
char flag=0;
unsigned int PLOT[256];


unsigned int BitToSend;    // number of bit to be send
//unsigned int stagecomplete;  

unsigned int _board_ID=2;
unsigned char board_MODE=1;//EIGHT_BITS
char _additional_info [32]={'B','a','t','t','e','r','y',' ','S','e','n','s','o','r'};
unsigned int TIMER_VALUE=0xFFFF; // Timer duration

char buff[200];

#define CHAR_RECEIVED 3
char read[CHAR_RECEIVED];
// static unsigned int value=0;
//------------------------------------------------------------------------
//								Function prototypes
//------------------------------------------------------------------------

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void);        
void __attribute__((interrupt, no_auto_psv)) _C1Interrupt(void);  
void __attribute__((__interrupt__)) _U1RXInterrupt(void);

//------------------------------------------------------------------------------
// 							Interrupt routine: timer1
//------------------------------------------------------------------------------
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    flag=flag+1;
    WriteTimer1(0x0);
    _T1IF = 0; 
   // IFS0=IFS0 & 0xFFF7;
}

//------------------------------------------------------------------------------
// 							Interrupt routine: UART RX
//------------------------------------------------------------------------------
void __attribute__((interrupt, , no_auto_psv)) _U1RXInterrupt(void)

{
  int i=0;
   IFS0bits.U1RXIF = 0;

   /* Read the receive buffer till atleast one or more character can be read */
   while( DataRdyUART1() && i<CHAR_RECEIVED)
   {	
        read[i] = ReadUART1();
	  	i=i+1;
   }
  	if (read[0]=='A')
  	{
  
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
    unsigned char i,l;
    unsigned int counter,m;

    //=========================== Serial Interface ===========================
	unsigned int baudvalue;   
	unsigned int U1MODEvalue;
	unsigned int U1STAvalue; 
	CloseUART1();
	// Configure uart1 receive and transmit interrupt 
	    ConfigIntUART1(UART_RX_INT_EN & UART_RX_INT_PR2 & 
	                   UART_TX_INT_DIS & UART_TX_INT_PR6);
	// Configure UART1 module to transmit 8 bit data with one stopbit. Also Enable loopback mode  
    baudvalue = 10; 
    /*U1MODEvalue = UART_EN & UART_IDLE_CON & 
                  UART_DIS_WAKE & UART_DIS_LOOPBACK  &
                  UART_DIS_ABAUD & UART_NO_PAR_8BIT  &
              //    UART_IrDA_DISABLE &
                  UART_1STOPBIT;*/

    U1MODEvalue = UART_EN & UART_IDLE_CON & 
                  UART_DIS_WAKE & UART_EN_LOOPBACK  &
                  UART_EN_ABAUD & UART_NO_PAR_8BIT  &
                  UART_1STOPBIT;

    /*U1STAvalue  = UART_INT_TX_BUF_EMPTY  &  
    //              UART_SYNC_BREAK_DISABLED &
                  UART_TX_ENABLE & UART_INT_RX_CHAR &
                  UART_ADR_DETECT_DIS &
                  UART_RX_OVERRUN_CLEAR;*/

    U1STAvalue  = UART_INT_TX_BUF_EMPTY  &  
                  UART_TX_PIN_NORMAL &
                  UART_TX_ENABLE & UART_INT_RX_3_4_FUL &
                  UART_ADR_DETECT_DIS &
                  UART_RX_OVERRUN_CLEAR;

    OpenUART1(U1MODEvalue, U1STAvalue, baudvalue);

    unsigned int result;
	unsigned int mean;
    unsigned int Channel, PinConfig, Scanselect;
    unsigned int Adcon3_reg, Adcon2_reg, Adcon1_reg;
    ADCON1bits.ADON = 0;
    Channel = ADC_CH0_POS_SAMPLEA_AN4 & ADC_CH0_NEG_SAMPLEA_NVREF;
    SetChanADC10(Channel);
    ConfigIntADC10(ADC_INT_DISABLE);
    PinConfig  = ENABLE_AN4_ANA;
    Scanselect =SKIP_SCAN_AN1 & SKIP_SCAN_AN2 & SKIP_SCAN_AN5 &
                 SKIP_SCAN_AN9 & SKIP_SCAN_AN10 &
                 SKIP_SCAN_AN14 & SKIP_SCAN_AN15;
    Adcon3_reg = ADC_SAMPLE_TIME_10 &
                 ADC_CONV_CLK_SYSTEM &
                 ADC_CONV_CLK_13Tcy;
    Adcon2_reg = ADC_VREF_AVDD_AVSS  &//ADC_VREF_AVDD_AVSS & 
                 ADC_SCAN_OFF &
                 ADC_ALT_BUF_OFF &
                 ADC_ALT_INPUT_OFF &
                 ADC_CONVERT_CH0&
                 ADC_SAMPLES_PER_INT_1;
    Adcon1_reg = ADC_MODULE_ON &
                 ADC_IDLE_CONTINUE &
                 ADC_FORMAT_INTG &
                 ADC_CLK_MANUAL &
                 ADC_SAMPLE_SIMULTANEOUS &
                 ADC_AUTO_SAMPLING_OFF; 
    OpenADC10(Adcon1_reg, Adcon2_reg, Adcon3_reg,PinConfig, Scanselect);             
    ADCON1bits.SAMP = 1;

	// Timer initialization    
    T1_Init(TIMER_VALUE);

    i=0;
    counter=0;
    for (;;)
    {
        if (flag==10)
        {
            flag=0;

	      	/// ADC conversion
	      	i=0;
	      	mean=0;
	      	result=0;
			while(i<15)
	        {
				ConvertADC10();		
				i=i+1;
		        while(BusyADC10());
		        result =ReadADC10(0);
	            ADCON1bits.SAMP = 1;
				mean=result+mean;
		        for(l=0;l<10;l++);	
			}
		
			sprintf(buff,"count: %d voltage: %d \n", counter, (mean>>4));
		    counter++;
			putsUART1 ((unsigned int *) buff);	

        }//if (flag==10)
    }//for(;;)
}//main

//------------------------------------------------------------------------
//					This is just a simple delay routine.
//------------------------------------------------------------------------
void Wait(unsigned int value)    
{
	Nop();
    //  while (value>0) value--;  
}//Wait();
