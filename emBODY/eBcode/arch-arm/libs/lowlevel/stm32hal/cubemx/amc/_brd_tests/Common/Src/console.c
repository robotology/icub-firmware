/*******************************************************************************************************************//**
 * @file    console.c
 * @author  G.Zini
 * @version 1.0
 * @date    2018 November, 9
 * @brief   Console management
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "console.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "utilities.h"
#include "shared.h"
#if defined CORE_CM7
#include "usbd_vcom.h"
#endif


/* Private macros *****************************************************************************************************/

// Maximum waiting time for the TX/RX operations
#define DEFAULT_TX_TIMEOUT  _ms_(100)
#define DEFAULT_RX_TIMEOUT  WAIT_FOREVER

// Maximum printable string
#define CONSOLE_LINE_LENGTH (160)

#ifndef UNUSED
#define UNUSED(x)   (void)(x)
#endif


/* Private variables **************************************************************************************************/

/* Console local buffer */
static char coBuffer[CONSOLE_LINE_LENGTH];


/* Local functions ****************************************************************************************************/

/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Initialize the console device
 * @param   void
 * @return  pdPASS      The console was successfully locked
 *          pdFAIL      It was not possible to lock the console in the time specified
 */
BaseType_t coInit( void )
{
    
#if defined CORE_CM7
    /* Initialize virtual-com */
    return ((VCOM_OK == vcomInit()) && (HAL_OK == SharedInit())) ? pdPASS : pdFAIL ;
#elif defined CORE_CM4
    return (HAL_OK == SharedInit())? pdPASS: pdFAIL;
#endif
}


/*******************************************************************************************************************//**
 * @brief   Transmit a character to the console device without translations
 * @param   ch      Character to be transmitted to the console device
 * @retval  char    Value of the argument 'ch' or '\0' in case of error
 */
char coTxChar( char ch )
{
#if defined CORE_CM7
    /* Receive a charecter from virtual-com */
    return (char)vcomTxChar((uint8_t)ch) ;
#elif defined CORE_CM4
    SharedTxChar((uint8_t)ch, WAIT_FOREVER);
    return ch;
#endif
}

/*******************************************************************************************************************//**
 * @brief   Receive a character from the console device without any translation
 * @param   void
 * @retval  char    Character received from the console or '\0' in case of error
 */
char coRxChar( void )
{
#if defined CORE_CM7
    /* Receive a charecter from virtual-com */
    return (char)vcomRxChar() ;
#elif defined CORE_CM4
    char ch = 0;
    SharedRxChar((uint8_t *)&ch, WAIT_FOREVER);
    return ch;
#endif
}

/*******************************************************************************************************************//**
 * @brief   Verify if one character is pending from the console device. The function does not block the caller
 * @param   void
 * @retval  - true  One or more characters from the console device have been received
 *          - false No characters are pending
 */
bool coRxReady( void )
{
#if defined CORE_CM7
    /* Check the virtual-com receiver fifo level */
    return (0 != vcomRxFifoLevel())? true : false ;
#elif defined CORE_CM4
    return SharedRxReady()? true : false;
#endif
}

/*******************************************************************************************************************//**
 * @brief   Verify the transmitter FIFO is empty. The function does not block the caller
 * @param   void
 * @retval  - true  All characters have been transmitted
 *          - false Some characters are still to be transmitted
 */
bool coTxReady( void )
{
#if defined CORE_CM7
    /* Check the virtual-com transmitter fifo level */
    return (0 == vcomTxFifoLevel())? true : false ;
#elif defined CORE_CM4
    return SharedTxReady()? true : false;;
#endif
}

/*******************************************************************************************************************//**
 * @brief   Transmit a character to the console device. The character '\n' is prefixed with '\r'
 * @param   ch      Character to be transmitted to the console device
 * @retval  char    Value of the argument 'ch'
 */
char coPutChar( char ch )
{
    if ('\n' == ch) coTxChar('\r') ;
    coTxChar( ch ) ;
    return ch ;
}

/*******************************************************************************************************************//**
 * @brief  Waits and receives a single character from the console device. The function filters-out all the control
 *         characters, with the esclusion of CARRIAGE-RETURN and BACK-SPACE. The TAB characters are translated into
 *         SPACES. The function does not echo the received character
 * @param  void
 * @retval char The character received from the console device
 */
char coGetChar( void )
{
    char ch ;
    while (1)
    {
        ch = coRxChar() ;
        // Returns any printable char, carriage-return and back-space
        if ((isprint(ch)) || ('\r' == ch) || ('\b' == ch)) return ch ;
        // Translate TAB into SPACE
        if ('\t' == ch) return (' ') ;
    }
}

int itm_puts(const char* str) 
{    

    if(NULL == str)
    {
        return(0);
    }

    uint32_t ch;
    int num = 0;
    while('\0' != (ch = *str))
    {
        ITM_SendChar(ch);
        str++;
        num++;
    }
     
    ITM_SendChar('\n');
    return(++num);    
}

/******************************************************************************************************************//**
 * @brief  Prints an ANSI-OEM (ASCII) string to the console device. The function translates the '\n' character into the
 *         '\r' '\n' sequence.
 * @param  str      Poiter to a zero-terminated string. If the pointer is NULL, the function doesn't perform any action
 * @retval char *   Value of the argument 'pCh'
 */
char *coPutString( const char *str )
{
    if (NULL != str)
    {
        itm_puts(str);
//        char ch ;
//        const char *pc ;
//        for (pc = str ; ('\0' != (ch = *(pc++))) ; ) coPutChar( ch ) ;
    }
    return (char *)str ;
}

/*******************************************************************************************************************//**
 * @brief   Editing of a string through the console device
 * @param   *str        Pointer to the destination buffer of the string. The value NULL is not accepted
 * @param   size        Size of the destination buffer. It must be greater than or equal to 1. The destination buffer
 *                      should store the string and the termination character, so this argument must be set to one
 *                      character more than the maximum string length that the function should handle. If size is
 *                      exactly equal to 1 then the function does not accept characters other than 'new line' and
 *                      generates an empty string
 * @retval  char *      Value of the argument 'str' or NULL in case of errors
 */
char *coEditString( char *str, size_t size )
{
    return EditString((void (*)(char))coTxChar, (char (*)(void))coRxChar, str, size) ;
}

/*******************************************************************************************************************//**
 * @brief  Print a formatted string to the console device
 * @param  *fmt     Format string. See function vsprintf for info
 * @param  ...      Variable argument list
 * @retval int      Number of arguments accepted and printed
 */
int coprintf( const char *fmt, ...)
{
    int ret ;
    va_list ap ;
    va_start(ap, fmt) ;
//    ret = tiny_vprintf((void (*)(char))coPutChar, fmt, ap) ;
    ret = vsprintf(coBuffer, fmt, ap);
    va_end(ap) ;
    coPutString(coBuffer);
    return ret ;
}


/*******************************************************************************************************************//**
 * @brief Ask the operator to enter an unsigned decimal number
 * @param ask       Optional character string representing the prompt to the operator
 * @param pNumber   Pointer to the destination variable
 * @return          true if a valid unsigned decimal number was entered
 */
bool coAskUnsigned(const char *ask, uint32_t *pNumber)
{
    char buf[16] ;
    char *cur ;
    uint32_t Num ;
    if (NULL != ask) coprintf("%s", ask) ;
    if ((NULL != coEditString(cur = buf, sizeof(buf))) &&
        atoul((const char **)&cur, &Num) &&
        ('\0' == skipblank((const char **)&cur)))
    {
        if (NULL != pNumber) *pNumber = Num ;
        return true ;
    }
    return false ;
}

/*******************************************************************************************************************//**
 * @brief Ask the operator to enter a signed decimal number
 * @param ask       Optional character string representing the prompt to the operator
 * @param pNumber   Pointer to the destination variable
 * @return          true if a valid signed decimal number was entered
 */
bool coAskSigned(const char *ask, int32_t *pNumber)
{
    char buf[16] ;
    char *cur ;
    int32_t Num ;
    if (NULL != ask) coprintf("%s", ask) ;
    if ((NULL != coEditString(cur = buf, sizeof(buf))) &&
        atosl((const char **)&cur, &Num) &&
        ('\0' == skipblank((const char **)&cur)))
    {
        if (NULL != pNumber) *pNumber = Num ;
        return true ;
    }
    return false ;
}

/*******************************************************************************************************************//**
 * @brief Ask the operator to enter an hexadecimal number
 * @param ask       Optional character string representing the prompt to the operator
 * @param pNumber   Pointer to the destination variable
 * @return          true if a valid hexadecimal number was entered
 */
bool coAskHex(const char *ask, uint32_t *pNumber)
{
    char buf[16] ;
    char *cur ;
    uint32_t Num ;
    if (NULL != ask) coprintf("%s", ask) ;
    if ((NULL != coEditString(cur = buf, sizeof(buf))) &&
        xtoul((const char **)&cur, &Num) &&
        ('\0' == skipblank((const char **)&cur)))
    {
        if (NULL != pNumber) *pNumber = Num ;
        return true ;
    }
    return false ;
}

/* END OF FILE ********************************************************************************************************/
