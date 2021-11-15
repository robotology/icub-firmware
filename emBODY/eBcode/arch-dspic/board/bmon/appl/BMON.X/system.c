/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */
#include "system.h"

void ConfigureOscillator(void)
{
   // ECH External Clock High-Power mode 10MHz
    // High power, 4-32 MHz (FOSC = 111)
    OSCCON=0b00000000;      // external oscillator 10MHz
	// bit 7 -> SPLLEN (Software PLL Enable bit)
	// bit 6 -> IRCF3 internal oscillator frequency select bits
	// bit 5 -> IRCF2 
	// bit 4 -> IRCF1
	// bit 3 -> IRCF0
	// bit 2 -> unimplemented
	// bit 1 -> SCS1
	// bit 0 -> SCS0
}


