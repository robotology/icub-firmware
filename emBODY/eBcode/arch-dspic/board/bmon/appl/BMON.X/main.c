 /****************************************************
 *  BMON board - iCub Facility - 2013                 *
 *  Main program                                     *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
// ----------------------------------------------------------------------------
// Firmware Version
// ----------------------------------------------------------------------------
char Firmware_vers = 1;
char Revision_vers = 1;
char Build_number  = 0;


/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <pic16f1824.h>
#include <xc.h>                 /* XC8 General Include File */

#include <stdint.h>             /* For uint8_t definition */
#include <stdbool.h>            /* For true/false definition */

#include "system.h"             /* System functions/params */
#include "user.h"               /* User functions/params */
#include "configuration_bits.h" /* Configuration bits */
#include "i2c.h"                /* i2c */
//#include <plib/i2c.h>

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

#define LED0 RA2        // DL1 red
#define LED1 RA4        // DL2 green
#define LED2 RC5        // DL3 green
#define LED3 RC4        // DL4 green

//#define VBATT RC3       // input battery voltage
//#define IBATT RC2       // input battery current

#define BUTTON RA0      // push button

#define ON  1
#define OFF 0


bit button_ok=0;

char timer_100us=0;
char timer_1ms=0;
char timer_10ms=0;
char timer_100ms=0;
char timer_1s=0;
char timeout=0;

char cont=0;

unsigned char prova=0;
char j=0;

char adc_channel=7;
volatile unsigned char I2C_Array[6] = {0};

char toggle=0;
char timer_blink=0;
char blink_ds=100;

char timer_debounce=3;
char timer_fail=0;

unsigned int adc_buffer;
//int VTH[7]={0x7D00, 0x84D0, 0x8CA0, 0x9470, 0x9C40, 0xA410, 0xABE0};   // threshold in mV

unsigned int VTH[7]={32000, 34000, 36000, 38000, 40000, 42000, 44000};   // threshold in mV

//int VTH[7]={0x0253, 0x0279, 0x0293, 0x02C3, 0x02E8, 0x030D, 0x0333};
//int VTH[14]={
//    0x0284, 0x02AD, 0x02D5, 0x02FE, 0x0320, 0x0348, 0x0370, // I2C power (3.08V)
//    0x0258, 0x027D, 0x02A2, 0x02C7, 0x02EC, 0x0311, 0x0336, // push button (3.3V)
//};
unsigned int vtol=100;  // voltage tolerance for hysteresis
unsigned int vhyst=0;    // voltage hysteresis

float vscale=0;    //
float vbatt_hres=0;
unsigned int vbatt=0;
unsigned int charge=0;
float iscale=0;
float ibatt_hres=0;
signed int ibatt=0;

// VTH = {32V, 34V, 36V, 38V, 40V, 42V, 44V}

//int VTH0=0x0279;        /* Voltage threshold 0 -> 34V */
//int VTH1=0x0293;        /* Voltage threshold 1 -> 36V */
//int VTH2=0x02C3;        /* Voltage threshold 2 -> 38V */
//int VTH3=0x02E8;        /* Voltage threshold 2 -> 40V */
//int VTH4=0x030D;        /* Voltage threshold 2 -> 42V */

int read_adc(char input);       /* read the ADC */
void measureBattery(void);      /* measure battery voltage and current*/
void readButton(void);          /* read the test button */
void displayCharge(void);       /* display status charge with leds */
void lightupLed(char lightup);  /* lightup/blink led  */
void blink();                   /* blink timer */

// -----------------------------------------------------------------------------
// Main Program
// -----------------------------------------------------------------------------
void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();
    //I2cInitMaster();
    I2cInitSlave(0x5A);
    
    while(1)
    {
        if (timer_100us >= 1){      /* ------------- 100us cycle ------------ */
            timer_100us=0;
            timer_1ms++;
        }

        if (timer_1ms >= 10){       /* ------------- 1ms cycle -------------- */
            timer_1ms=0;
            timer_10ms++;
        }

        if (timer_10ms >= 10){      /* ------------- 10ms cycle ------------- */
            timer_10ms=0;
            timer_100ms++;

            blink();
            measureBattery();
            displayCharge();
        }

        if (timer_100ms >= 10){     /* ------------- 100ms cycle ------------ */
            timer_100ms=0;
            timer_1s++;

            readButton();
            
            if(timeout++ > 10){      // If I2C communication fails -> reset
                if(SCL_PIN==1 && SDA_PIN==0){
                    #asm
                    reset
                    #endasm
                }
            }
            
        }

        if (timer_1s >= 10){     /* ---------------- 1s cycle --------------- */
            timer_1s=0;
            cont++;
        }
    }
}

// -----------------------------------------------------------------------------
// Set and read ADC channel
// -----------------------------------------------------------------------------
int read_adc(char channel){
    if      (channel==7)   {ADCON0bits.CHS=0b00111;}   // AN7
    else if (channel==6)   {ADCON0bits.CHS=0b00110;}   // AN6
    ADCON0bits.GO_nDONE=1;      // Start conversion
}

// -----------------------------------------------------------------------------
// Read the push button state with debounce filtering
// -----------------------------------------------------------------------------
void readButton(void){
    if(BUTTON==1){
        if(timer_fail < timer_debounce)	{timer_fail++;}
    }
    else{
	if(timer_fail > 0)	{timer_fail=0;}
    }
    if(timer_fail == timer_debounce){
        button_ok=1;
        vscale=53.5;
        iscale=42.8;
    }
    else{
        button_ok=0;
        vscale=50;
        iscale=40;
    }
}


// -----------------------------------------------------------------------------
// use adc to measure battery voltage and current
// -----------------------------------------------------------------------------
void measureBattery(void){
    if(!PIR1bits.ADIF)  {
        switch(adc_channel){
            case 7:                 // read battery voltage
                //I2C_Array[2]=ADRESH;
                //I2C_Array[3]=ADRESL>>6;
                //ibatt=((int)adc_buffer);
                ibatt_hres=(float)(adc_buffer-0x0200)*iscale;
                ibatt=(signed int)ibatt_hres;
                I2C_Array[2]=(ibatt>>8) & 0xFF;
                I2C_Array[3]=ibatt & 0xFF;
                read_adc(adc_channel);    // read battery voltage
                adc_channel=6;
            break;

            case 6:                 // read battery current
                //I2C_Array[0]=ADRESH;
                //I2C_Array[1]=ADRESL>>6;
                vbatt_hres=(float)adc_buffer*vscale;
                vbatt=(unsigned int)vbatt_hres;
                I2C_Array[0]=(vbatt>>8) & 0xFF;
                I2C_Array[1]=vbatt & 0xFF;

                if(vbatt > VTH[5])      {charge=100;}
                else if(vbatt < VTH[0]) {charge=0;}
                else                    {charge=(unsigned int)(100*(float)((float)(vbatt-VTH[0])/(float)(VTH[5]-VTH[0])));}
                I2C_Array[4]=(charge>>8) & 0xFF;
                I2C_Array[5]=charge & 0xFF;


                read_adc(adc_channel);        // read battery current
                adc_channel=7;
            break;
        }

    }
}



// -----------------------------------------------------------------------------
// Light up leds to display battery voltage
// -----------------------------------------------------------------------------
void displayCharge(void){
    int adc_buffer_last=adc_buffer;

    if(vbatt < (VTH[0]+vhyst)){
        if((VTH[0]-vbatt) > vtol)   {vhyst=0;}
        else                        {vhyst=vtol;}
        blink_ds=6;
        lightupLed(0b00010001);     // light up LED0
    }
    else if(vbatt < (VTH[1]+vhyst)){
        if((VTH[1]-vbatt) > vtol)   {vhyst=0;}
        else                        {vhyst=vtol;}
        blink_ds=15;
        lightupLed(0b00010001);     // light up LED0
    }
    else if(vbatt < (VTH[2]+vhyst)){
        if((VTH[2]-vbatt) > vtol)   {vhyst=0;}
        else                        {vhyst=vtol;}
        lightupLed(0b00000001);     // light up LED0
    }
    else if(vbatt < (VTH[3]+vhyst)){
        if((VTH[3]-vbatt) > vtol)   {vhyst=0;}
        else                        {vhyst=vtol;}
        lightupLed(0b00000011);     // light up LED1
    }
    else if(vbatt < (VTH[4]+vhyst)){
        if((VTH[4]-vbatt) > vtol)   {vhyst=0;}
        else                        {vhyst=vtol;}
        lightupLed(0b00000111);     // light up LED2
    }
    else if(vbatt < (VTH[5]+vhyst)){
        if((VTH[5]-vbatt) > vtol)   {vhyst=0;}
        else                        {vhyst=vtol;}
        lightupLed(0b00001111);     // light up LED3
    }
    else if(vbatt < (VTH[6]+vhyst)){
        if((VTH[6]-vbatt) > vtol)   {vhyst=0;}
        else                        {vhyst=vtol;}
        blink_ds=15;
        lightupLed(0b00001111);     // light up LED3
    }
    else{
        vhyst=0;
        blink_ds=6;
        lightupLed(0b00011111);     // light up LED3
    }
}



// -----------------------------------------------------------------------------
// Turn on/off and blink leds
// -----------------------------------------------------------------------------
void lightupLed(char lightup){
    // lightup<4> blink, lightup<3:0> light up
    char blink = toggle & (lightup>>4 & 0x01);
    LED0 = (!(lightup    & 0x01)) || blink;
    LED1 = (!(lightup>>1 & 0x01)) || blink;
    LED2 = (!(lightup>>2 & 0x01)) || blink;
    LED3 = (!(lightup>>3 & 0x01)) || blink;
}

// -----------------------------------------------------------------------------
// Blinking timer function
// -----------------------------------------------------------------------------
void blink(){
    timer_blink++;
    if(timer_blink >= blink_ds){
        toggle = toggle^1;
        timer_blink=0;
    }
}
