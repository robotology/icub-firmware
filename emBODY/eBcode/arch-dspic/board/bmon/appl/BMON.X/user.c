/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */
#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

void InitApp(void)
{
    /* --------------- I/O list //--------------- */
    // 1 input VBATTMON     IN      RC3
    // 1 input IBATTMON     IN      RC2
    // 1 input PGD          IN      RA0
    // 1 output LED0        OUT     RA2
    // 1 output LED1        OUT     RA4
    // 1 output LED2        OUT     RC5
    // 1 output LED3        OUT     RC4


    /* TODO Initialize User Ports/Peripherals/Project here */
    TRISA = 0b00000001;		// 0=output  1=input
    TRISC = 0b00001111;

    ANSELA = 0b00000000;        // A ports (1=analog input, 0=digital I/O)
    ANSELC = 0b00001100;        // C ports (1=analog input, 0=digital I/O)

    /* Initialize ports */
    PORTA = 0b00010100;
    PORTC = 0b00110000;
    LATA  = 0b00010100;
    LATC  = 0b00110000;
    
    /* Setup analog functionality and port direction */

    /* Initialize peripherals */
    T1CON=0b01000001;       /* ------------------ Timer 1 ------------------*/
    // bit 7 e 6 TMR1CS<1:0> Timer1 Clock Source Select bits
    // bit 5 e 4 T1CKPS<1:0> Prescaler
    // bit 3 T1OSCEN            circuito di oscillazione abilitato
    // bit 2 T1SYNC		timer1 in modalità asincrona
    // bit 1 			Unimplemented
    // bit 0 TMR1ON		Enables Timer1
    PIE1bits.TMR1IE = 1;    // Enable overflow interrupt generation on timer 1

    ADCON0=0x00;        /* ------------------ A/D ------------------*/
    // bit 7            Unimplemented
    // bit 6-2          CHS<4:0>: Analog Channel Select bits
    // bit 1            GO/DONE: A/D Conversion Status bit
    // bit 0            ADON: ADC Enable bit
    ADCON0bits.ADON = 1;

    
    ADCON1=0x00;        /* ------------------ A/D ------------------*/
    // bit 7            ADFM: A/D Result Format Select bit
    // bit 6-4          ADCS<2:0>: A/D Conversion Clock Select bits
    // bit 3            Unimplemented
    // bit 2            ADNREF: A/D Negative Voltage Reference Configuration bit
    // bit 1-0          ADPREF<1:0>: A/D Positive Voltage Reference Configuration bits
    ADCON1bits.ADCS=0b110;





    /* Enable interrupts */
    INTCONbits.GIE = 1;     // Global Interrupt Enable bit
    INTCONbits.PEIE = 1;    // Peripheral Interrupt Enable bit
    PIE1bits.ADIE = 1;      // ADIE: A/D Converter (ADC) Interrupt Enable bit
}

