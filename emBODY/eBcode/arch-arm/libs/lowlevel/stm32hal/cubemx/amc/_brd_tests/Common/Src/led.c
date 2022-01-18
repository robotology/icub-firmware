/*******************************************************************************************************************//**
 * @file    led.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 June, 29
 * @brief   LEDs management
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include "led.h"
#include "console.h"

/* Exported functions prototypes -------------------------------------------------------------------------------------*/

/*******************************************************************************************************************//**
 * @bries   Test LEDS from DL3, DL4, DL5, DL8, DL9 and DL10
 * @param   void
 * @return  void
 */
void LedTest(void)
{
    coprintf("General purpose LEDs test started\n"
             "Press SPACE to step forward\n"
             "Press ENTER to terminate...\n");
    /* All LEDs off */
    LedOnOff(LED_NONE, LED_ALL);
    while (1)
    {
        char ch;

        coprintf("-> DL9 Orange\n");
        LedOnOff(LED1, LED6);
        do {if ('\r' == (ch = coRxChar())) goto __exit;} while (' ' != ch);

        coprintf("-> DL10 Orange\n");
        LedOnOff(LED2, LED1);
        do {if ('\r' == (ch = coRxChar())) goto __exit;} while (' ' != ch);

        coprintf("-> DL3 Green\n");
        LedOnOff(LED3, LED2);
        do {if ('\r' == (ch = coRxChar())) goto __exit;} while (' ' != ch);

        coprintf("-> DL4 Green\n");
        LedOnOff(LED4, LED3);
        do {if ('\r' == (ch = coRxChar())) goto __exit;} while (' ' != ch);

        coprintf("-> DL8 Red\n");
        LedOnOff(LED5, LED4);
        do {if ('\r' == (ch = coRxChar())) goto __exit;} while (' ' != ch);

        coprintf("-> DL5 Red\n");
        LedOnOff(LED6, LED5);
        do {if ('\r' == (ch = coRxChar())) goto __exit;} while (' ' != ch);

        coprintf("-> All ON\n");
        LedOnOff(LED1|LED2|LED3|LED4|LED5|LED6, 0);
        do {if ('\r' == (ch = coRxChar())) goto __exit;} while (' ' != ch);

        coprintf("-> All OFF\n");
        LedOnOff(0, LED1|LED2|LED3|LED4|LED5|LED6);
        do {if ('\r' == (ch = coRxChar())) goto __exit;} while (' ' != ch);
    }
__exit:
    /* All LEDs off */
    LedOnOff(LED_NONE, LED_ALL);
    coprintf("LEDs test terminated\n");
}


/* END OF FILE ********************************************************************************************************/
