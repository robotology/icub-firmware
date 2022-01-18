/*******************************************************************************************************************//**
 * @file    TestCM4.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 June, 4
 * @brief   Main Cortex-M7 Test Loop
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include "main.h"
#include "cmsis_os.h"
#include "shared.h"
#include "version.h"
#include "console.h"
#include "enc.h"
#include "hall.h"
#include "pwm.h"


/*******************************************************************************************************************//**
  * @brief  Function implementing the TestCM7_Task thread.
  * @param  argument: Not used
  * @retval None
  */
__attribute__((noreturn)) void TestCM4(void const * argument)
{
    /*Switch VAUX on */
    HAL_GPIO_WritePin(PWR_VAUXEN_GPIO_Port, PWR_VAUXEN_Pin, GPIO_PIN_SET);
    
    /* Init code for the console device */
    coInit();

__wait:
    /* Wait for the link established */
    coGetChar();

__info:
    /* Give some INFO */
    coprintf("AMC Test software ver. %u.%u\n", __SW_VERSION__, __SW_REVISION__);
    coprintf("Compliled %s, %s\n", __DATE__, __TIME__);
    coprintf("Hardware option %u\n", __HW_VERSION__);
    coprintf("CPUID %08X (Cortex M4)\n", SCB->CPUID);
    coprintf("Device category %04X, revision %04X\n", __DEVICE_ID__, __DEVICE_REVISION__);

    while (1)
    {
        char ch;

        /* Main Menu */
        coprintf("\n"
                 "Main menu:\n"
                 "0 - Link to CM7\n"
                 "1 - Test Displacement Encoder\n"
                 "2 - Test Hall sensors\n"
                 "3 - Test PWM outputs\n"
                 "4 - Test \n"
                 "5 - Test \n");
        while (1)
        {
            coprintf("\nCM4 > ");
            ch = coRxChar();
            if (isprint(ch)) 
            {
                coprintf("%c\n", ch);
                switch (ch)
                {
                    case '0':
                        coprintf("Passing control to CM7 core...\n\n");
                        HAL_Delay(1000);
                        coTxChar('\0');
                        goto __wait;
                    case '1':
                        EncTest();
                        break;
                    case '2':
                        HallTest();
                        break;
                    case '3':
                        PwmTest();
                        break;
                    case '4':
                        coprintf("*** TO BE DONE ***\n");
                        break;
                    case '5':
                        coprintf("*** TO BE DONE ***\n");
                        break;
                    case '?':
                        goto __info;
                    default:
                        coprintf("?\n");
                        break;
                }
                break;
            }
            else if ('\x1B' == ch) break;
            else if ('\n' != ch) coprintf("?");
        }
        coprintf("\n");
    }
}



/* END OF FILE ********************************************************************************************************/
