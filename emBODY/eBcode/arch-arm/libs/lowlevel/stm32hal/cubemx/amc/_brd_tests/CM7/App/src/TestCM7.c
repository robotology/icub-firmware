/*******************************************************************************************************************//**
 * @file    TestCM7.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 June, 4
 * @brief   Main Cortex-M7 Test Loop
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "console.h"
#include "version.h"
#include "eth.h"
#include "eeprom.h"
#include "led.h"
#include "adc.h"
#include "can.h"
#include "hwsemphr.h"
#include "shared.h"


/*******************************************************************************************************************//**
  * @brief  Function implementing the TestCM7_Task thread.
  * @param  argument: Not used
  * @retval None
  */
__attribute__((noreturn)) void TestCM7(void const * argument)
{
    UNUSED(argument);
    /* Init code for USB_DEVICE */
    MX_USB_DEVICE_Init();
    /* Init code for ADC3 */
    AdcInit();
    /* Init code for the console device */
    coInit();
    /* Wait for the link established */
    coGetChar();

__info:
    /* Give some INFO */
    coprintf("AMC Test software ver. %u.%u\n", __SW_VERSION__, __SW_REVISION__);
    coprintf("Compliled %s, %s\n", __DATE__, __TIME__);
    coprintf("Hardware option %u\n", __HW_VERSION__);
    coprintf("CPUID %08X (Cortex M7)\n", SCB->CPUID);
    coprintf("Device category %04X, revision %04X\n", __DEVICE_ID__, __DEVICE_REVISION__);
    coprintf("Flash size %u Bytes (%s %u Sectors x %u Bytes)\n", 
              FLASH_SIZE, FLASH_BANK_TYPE, FLASH_SECTOR_TOTAL, FLASH_SECTOR_SIZE);
    coprintf("Device Unique ID %08X.%08X.%08X\n", UID[0], UID[1], UID[2]);

    while (1)
    {
        char ch;

        /* Main Menu */
        coprintf("\n"
                 "Main menu:\n"
                 "0 - Link to CM4\n"
                 "1 - Test LED\n"
                 "2 - Test ADC\n"
                 "3 - Test EEPROM\n"
                 "4 - Test CAN\n"
                 "5 - Test Ethernet\n");
        while (1)
        {
            coprintf("\nCM7 > ");
            ch = coRxChar();
            if (isprint(ch)) 
            {
                coprintf("%c\n", ch);
                switch (ch)
                {
                    case '0':
                        coprintf("Passing control to CM4 core...\n\n");
                        HAL_Delay(1000);
                        SharedTest();
                        goto __info;
                    case '1':
                        LedTest();
                        break;
                    case '2':
                        AdcTest();
                        break;
                    case '3':
                        EeTest();
                        break;
                    case '4':
                        CanTest();
                        break;
                    case '5':
                        EthTest();
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
