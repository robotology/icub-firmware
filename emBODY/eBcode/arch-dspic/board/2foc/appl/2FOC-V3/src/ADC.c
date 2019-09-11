//   
//  ADC related rouines:
//  Measure Current & VDC-Link
//
//  Initialization, control and fault management 
//

#include <p33FJ128MC802.h>
#include <libpic30.h> /*__delay32 */

#include "ADC.h"
#include "Faults.h"

#define ADC_VDCLINK_ALLEGRO_MIN_THRESHOLD 120

SFRAC16 ADCBuffer[4] __attribute__((space(dma),aligned(16)));

void ADCInterruptAndDMAEnable(void)
// Enable DMA interrupt, arm DMA for transfer
{
    //Clear the DMA interrupt flag bit
    IFS0bits.DMA0IF = 0;
    //Set the DMA interrupt enable bit
    IEC0bits.DMA0IE = 1;
    // amplifiers holding
    AD1CON1bits.SAMP = 1;
    // no valid result is present
    AD1CON1bits.DONE = 0;
    // Clear the A/D interrupt flag bit
    IFS0bits.AD1IF = 0;
    // Turn on DMA
    DMA0CONbits.CHEN = 1;
    // Turn on A/D module
    AD1CON1bits.ADON = 1;
}

void ADCInterruptAndDMADisable(void)
// Disable DMA interrupt, disarm DMA
{
    // Turn off DMA
    DMA0CONbits.CHEN = 0;
    // Turn off A/D module
    AD1CON1bits.ADON = 0;
    // reset the DMA interrupt enable bit
    IEC0bits.DMA0IE = 0;
}

SFRAC16 ADCGetVDCLink()
// This quickly returns VDCLink SFRAC value.
{
    return ADCBuffer[2];
}

unsigned int ADCVDCLinkTo100mV(int vdc)
// This return VDC link value in 100mV units.
// It can be used for CAN information message
// or maybe to implement VDClink voltage protection.
// However AdcGetVDCLink() function is faster, so 
// consider using it for voltage protection stuff.
{
    unsigned int tmp;
    //double tmp2;

    //convert to full scale unsigned int
    tmp = vdc;
    tmp -= 32767; //1<<15;

    // convert to 10 bit unsigned int and mult by 10
    //tmp = tmp >> 5;
    //tmp *= 5;
    //tmp2 = tmp;

    // VonAdcPads = VDC * 0.04592 (from input resistor scaling 100K, 4.7K)
    // AdcReadVal = VDC * 0.04592 / 3.3 * 1024 = 14.249115
    // VDC = AdcReadVal / 14.249115
    // TODO: ....e poi, una divisione con un finto float????
    //   forse sarà il caso di farla in FIXED 1.15 !!!
    //tmp2 /= 14.249115;
    //tmp = tmp2 ;

    tmp >>= 3;
    tmp *=  5;
    tmp /= 57; // 4/57 = 1/14.25
  
    return tmp;
}

int ADCCalibrateOffset(void)
// Calculate mean value of ADC_CAL_N_SAMPLES samples for iA, iB and Vdc and store in
// MeasCurrParm.Offseta, Offsetb 
// DMA AND ADC isr are not enabled here!
// An error in estimation of current offset results in a cogging torque when 
// torque set point is 0; the value can be adjusted looking at the 3 phase ia, ib, and ic
// and centering all values around 0
{
    long ADCOffs1 = 0;
    long ADCOffs2 = 0;
    long ADCVdcZero = 0;
  
    // Put an 'accettable' value in case of error in the calibration process
    MeasCurrParm.Offseta = 0x200;
    MeasCurrParm.Offsetc = 0x200;

    // Turn on ADC module
    AD1CON1bits.ADON = 1;
 	
    int n;
    
    for (n=0; n<ADC_CAL_N_SAMPLES; ++n)
    {
        // samples AN0
        AD1CHS0bits.CH0SA = 0;
        AD1CON1bits.DONE = 0;
        // amplifiers holding
        AD1CON1bits.SAMP = 1;

        // TODO: TRIM ME (eventually:)
        __delay32(100);

        // convert!
        AD1CON1bits.SAMP = 0;

        int timeout = 0;

        // poll for end of conversion
        while(!AD1CON1bits.DONE)
        {
            // Avoids polling lock
            if (ADC_CAL_TIMEOUT == ++timeout)
            {
                // offset correction error signaling
      
                return -1;
            }
        }
        // accumulate values
        ADCOffs1 += (int)ADC1BUF0;

        // Samples AN1
        AD1CHS0bits.CH0SA = 1;
		
        AD1CON1bits.DONE = 0;
        // amplifiers holding
        AD1CON1bits.SAMP = 1;

        // TODO TRIM ME
        __delay32(100);

        // convert!
        AD1CON1bits.SAMP = 0;

        timeout = 0;

        // poll for end of conversion
        while (!AD1CON1bits.DONE)
        {
            // Avoids polling lock
            if (ADC_CAL_TIMEOUT == ++timeout)
            {
                // offset correction error signaling
   
                return -1;
            }
        }
        // accumulate values
        ADCOffs2 += (int)ADC1BUF0;
	
        // Samples AN2 (Vdc)
        AD1CHS0bits.CH0SA = 2;

        AD1CON1bits.DONE = 0;
        // amplifiers holding
        AD1CON1bits.SAMP = 1;

        // TODO TRIM ME */
        __delay32(100);

        // convert!
        AD1CON1bits.SAMP = 0;
        // don't mind about int generated

        timeout = 0;

        // poll for end of conversion
        while (!AD1CON1bits.DONE)
        {
            // Avoids polling lock
            if (ADC_CAL_TIMEOUT == ++timeout)
            {
                // offset correction error signaling
   
                return -1;
            }
        }
        // accumulate values
        ADCVdcZero += (int)ADC1BUF0;

       /* if the measured VDCLINK is lower than this threshold
        * then we cannot be sure the Allegro are properly powered.
        * For this reason the ADC calibration is not reliable.
        */
        if(ADCVDCLinkTo100mV(ADC1BUF0) < ADC_VDCLINK_ALLEGRO_MIN_THRESHOLD) return -2;
    }

    // divide for number of samples in order to get mean
    
    ADCOffs1   += ADCOffs1>=0   ? ADC_CAL_N_SAMPLES/2 : -ADC_CAL_N_SAMPLES/2;
    ADCOffs1   /= ADC_CAL_N_SAMPLES;
    
    ADCOffs2   += ADCOffs2>=0   ? ADC_CAL_N_SAMPLES/2 : -ADC_CAL_N_SAMPLES/2;
    ADCOffs2   /= ADC_CAL_N_SAMPLES;
    
    ADCVdcZero += ADCVdcZero>=0 ? ADC_CAL_N_SAMPLES/2 : -ADC_CAL_N_SAMPLES/2;
    ADCVdcZero /= ADC_CAL_N_SAMPLES;
    
    // associate the calculated offset
    MeasCurrParm.Offseta = ADCOffs1;
    MeasCurrParm.Offsetc = ADCOffs2;

    // Turn off ADC module
    AD1CON1bits.ADON = 0;

    return 0;
}

volatile int storeIa,storeIc;

int ADCCalibrateGain(void)
// Calculate mean value of ADC_CAL_N_SAMPLES samples for iA, iB and Vdc and store in
// MeasCurrParm.Offseta, Offsetb 
// DMA AND ADC isr are not enabled here!
// An error in estimation of current offset results in a cogging torque when 
// torque set point is 0; the value can be adjusted looking at the 3 phase ia, ib, and ic
// and centering all values around 0
{
    long Ia = 0;
    long Ic = 0;
    
    // Turn on ADC module
    AD1CON1bits.ADON = 1;
 	
    int n;
    
    for (n=0; n<ADC_CAL_N_SAMPLES; ++n)
    {
        // samples AN0
        AD1CHS0bits.CH0SA = 0;
        AD1CON1bits.DONE = 0;
        // amplifiers holding
        AD1CON1bits.SAMP = 1;

        // TODO: TRIM ME (eventually:)
        __delay32(100);

        // convert!
        AD1CON1bits.SAMP = 0;

        int timeout = 0;

        // poll for end of conversion
        while(!AD1CON1bits.DONE)
        {
            // Avoids polling lock
            if (ADC_CAL_TIMEOUT == ++timeout)
            {
                // offset correction error signaling
      
                return -1;
            }
        }
        // accumulate values
        Ia += (int)ADC1BUF0;

        // Samples AN1
        AD1CHS0bits.CH0SA = 1;
		
        AD1CON1bits.DONE = 0;
        // amplifiers holding
        AD1CON1bits.SAMP = 1;

        // TODO TRIM ME
        __delay32(100);

        // convert!
        AD1CON1bits.SAMP = 0;

        timeout = 0;

        // poll for end of conversion
        while (!AD1CON1bits.DONE)
        {
            // Avoids polling lock
            if (ADC_CAL_TIMEOUT == ++timeout)
            {
                // offset correction error signaling
   
                return -1;
            }
        }
        // accumulate values
        Ic += (int)ADC1BUF0;
    }

    // divide for number of samples in order to get mean

    Ia += Ia>=0 ? ADC_CAL_N_SAMPLES/2 : -ADC_CAL_N_SAMPLES/2; 
    Ia = MeasCurrParm.Offseta - Ia/ADC_CAL_N_SAMPLES;
    
    Ic += Ic>=0 ? ADC_CAL_N_SAMPLES/2 : -ADC_CAL_N_SAMPLES/2;
    Ic = MeasCurrParm.Offsetc - Ic/ADC_CAL_N_SAMPLES;
    
    storeIa = Ia;
    storeIc = Ic;
    
    if (Ia < 0) Ia = -Ia;
    if (Ic < 0) Ic = -Ic;
    
    int Il = Ia<Ic ? Ia : Ic;
    int Ih = Ia>Ic ? Ia : Ic;

    int Kh = 0x4001;
    int Kl; 
    long diff_old = 0x7FFFFFFFUL;
    
    while (1)
    {
        Kl = __builtin_divsd(0x10000000L,--Kh);
        
        long diff = __builtin_mulss(Kh,Ih) - __builtin_mulss(Kl,Il);
        
        if (diff < 0) diff = -diff;
        
        if (diff <= diff_old)
        {
            diff_old = diff;
        }
        else
        {
            Kl = __builtin_divsd(0x10000000UL,++Kh);
            
            break;
        }
    }    
    
    MeasCurrParm.qKa = Ia<Ic ? Kl : Kh; 
    MeasCurrParm.qKc = Ic<Ia ? Kl : Kh;
    
    // Turn off ADC module
    AD1CON1bits.ADON = 0;

    return 0;
}

void ADCConfigPWMandDMAMode()
// Confure ADC regisaters for PWM sync and DMA transfer
{
    // zero configuration
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
  
    // 10 bits
    AD1CON1bits.AD12B = 0;
  
    // A/D Conversion Clock (TAD). must be >= 76ns.
    // 40Mhz clk = 25 ns (TCY).
    // TAD = TCY * (ADCS+1)
    // TAD = 25*5 = 125 ns
    // Conversion time = TAD*12 = 1.5us
    // time to conv 4 ch = 1.5*4 = 6 us
    // TODO: il valore di conversione ha influenza sul ritardo con cui parte il calcolo della SVG
    // 1.5 us + tempo di calcolo deve essere < di TPWM/2
    // AM portato a 4. Empiricamente si vede meno rumore su DCLink.
    // Su lettura di corrente sembra migliorare leggermente
    AD1CON3bits.ADCS = 4;   // AD1CON3bits.ADCS = 2;

    // Samples CH0, CH1, CH2, CH3 simultaneously (when CHPS = 1x)
    AD1CON2bits.CHPS = 2;

    // Samples multiple channels simultaneous
    AD1CON1bits.SIMSAM = 1;

    // Sampling begins immediately after last conversion completes.
    // SAMP bit is auto set
    AD1CON1bits.ASAM = 1;

    // PWM triggered conversion
    AD1CON1bits.SSRC = 3;

    // Signed fractional (DOUT = sddd dddd dd00 0000)
    AD1CON1bits.FORM = 3;

    // Interrupt after every conversion!
    AD1CON2bits.SMPI = 0;
   
    // One sample per cycle on each ch
    AD1CON4 = 0;
    
    // Do not scan!
    AD1CON2bits.CSCNA = 0;

    // ADCHS: ADC Input Channel Select Register
    AD1CHS0 = 0;
	
    // CH0 is unused. connect to a dummy AN
    AD1CHS0bits.CH0SA = 3;
    // CH1 positive input is AN0, CH2 positive input is AN1, CH3 positive input is AN2
    AD1CHS123bits.CH123SA = 0;

    // DMA configurations for ADC
    DMA0CON = 0;
    // Configure DMA for Peripheral indirect mode
    DMA0CONbits.AMODE = 2;

    // Continuous, Ping-Pong modes disabled
    DMA0CONbits.MODE = 0;

    // trasfer block unit is a WORD
    DMA0CONbits.SIZE = 0;

    // set DMA source register
    DMA0PAD=(int)&ADC1BUF0;
    // number of words to transfer
    // do 4 transfers
    DMA0CNT = 3;
    // attach DMA0 transfer to ADC
    DMA0REQ = 13;

    // set address of dma buffer
    DMA0STA = __builtin_dmaoffset(ADCBuffer);
}

void ADCConfigureRegistersForCalibration()
// Configure ADC registers for calibration without PWM sync and DMA
{
    // zero configuration
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
    
    // A/D Conversion Clock Select bits = 5 * Tcy
    AD1CON3bits.ADCS = 4;

    // sample only from ch0
    AD1CON2bits.CHPS = 0;

    // Samples multiple channels individually in sequence
    AD1CON1bits.SIMSAM = 0;

    // Sampling begins immediately after last conversion completes.
    // SAMP bit is auto set
    AD1CON1bits.ASAM = 1;

    // sampling in manual mode
    AD1CON1bits.SSRC = 0;

    // Signed fractional (DOUT = sddd dddd dd00 0000)
    AD1CON1bits.FORM = 3;

    // Interrupt after every conversion!
    AD1CON2bits.SMPI = 0;
   
    // One sample per cycle on each ch
    AD1CON4 = 0;
    
    // Do not scan for calibration !
    AD1CON2bits.CSCNA = 0;

    // Channel 0 negative input is VREF-
    AD1CHS0 = 0;
}

void ADCDoOffsetCalibration(void)
// ADC Initialization:
// Performs offset calibration for ISENSES and initial base value acquisition for vdclink.
// Then setup ADC for for sequential sampling on  CH0=AN0, CH1=AN1, CH2=AN2. 
// without PWM sync and DMA, value and stored in MeasCurrParm.Offseta,b signed fractional form.
{
    int i;
    int ret;
    int led;
    // Configure ADC registers for calibration without PWM sync and DMA
    ADCConfigureRegistersForCalibration();

    // Execute calibration and store values
  
    for (i=0; i<10000; ++i) // delay  1s
    {
        __delay32(4000);   //delay 100us
    }

    led = LED_status.RedBlinkRate;

    while (1)
    {
        ret = ADCCalibrateOffset();

        if (ret == -1)
        {
           /* AD is not working properly.
            * loop forever if fatal error
            * condition
            */
            SysError.ADCCalFailure = 1;
            LED_status.RedBlinkRate=BLINKRATE_STILL;

            while(1);

        }
        else if (ret == -2)
        {
           /* VDCLink is too low.
            * enlight red led, then
            * wait forever for VDClink to
            * raise
            */
            SysError.ADCCalFailure = 1;
	 	
            LED_status.RedBlinkRate=BLINKRATE_STILL;
            // break;
		
            continue;
        }
		 
     	LED_status.RedBlinkRate = led;
 	 
        return;
    }
}

void ADCDoGainCalibration(void)
{
    int i;
    int ret;
    int led;
    // Configure ADC registers for calibration without PWM sync and DMA

    // Execute calibration and store values
  
    for (i=0; i<10000; ++i) // delay  1s
    {
        __delay32(4000);   //delay 100us
    }

    led = LED_status.RedBlinkRate;

    while (1)
    {
        ret = ADCCalibrateGain();

        if (ret == -1)
        {
           /* AD is not working properly.
            * loop forever if fatal error
            * condition
            */
            SysError.ADCCalFailure = 1;
            LED_status.RedBlinkRate=BLINKRATE_STILL;

            while(1);

        }
        else if (ret == -2)
        {
           /* VDCLink is too low.
            * enlight red led, then
            * wait forever for VDClink to
            * raise
            */
            SysError.ADCCalFailure = 1;
	 	
            LED_status.RedBlinkRate=BLINKRATE_STILL;
            // break;
		
            continue;
        }
		 
     	LED_status.RedBlinkRate = led;
 	 
        return;
    }
}
