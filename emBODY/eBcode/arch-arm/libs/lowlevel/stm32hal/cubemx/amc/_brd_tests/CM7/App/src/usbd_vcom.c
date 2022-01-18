/*******************************************************************************************************************//**
 * @file    usbd_vcom.c
 * @author  G.Zini
 * @version 1.0
 * @date    2018 November, 5
 * @brief   Virtual COM device driver
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include "console.h"
#include "usbd_vcom.h"
#include "usbd_cdc_if.h"
#include "utilities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os.h"

/* Local Macros *******************************************************************************************************/

#define FREERTOS_PRIORITY(prior)    ((int)(prior)+(int)(tskIDLE_PRIORITY)-(int)(osPriorityIdle))
#define VCOM_TXTASK_PRIORITY        FREERTOS_PRIORITY(osPriorityRealtime)
#define VCOM_TXBUFFER_LENGTH        (64u)
#define VCOM_TXQUEUE_LENGTH         (64u)
#define VCOM_RXQUEUE_LENGTH         (64u)
#define VCOM_TXTASK_STACK           (configMINIMAL_STACK_SIZE)

#define VCOM_MAX_DELAY              (_ms_(5000))



/* Local Typedefs *****************************************************************************************************/
/* Local Variables ****************************************************************************************************/

/* Handle of the Tx task */
TaskHandle_t vcomTxTaskHandle = NULL ;

/* Forward declaration of the Tx task */
static void vcomTxTask( void *arg ) __attribute__((noreturn)) ;

/* Handles tx/rx queues */
QueueHandle_t vcomTxQueue = NULL ;
QueueHandle_t vcomRxQueue = NULL ;

/* Transmit buffer */
static uint8_t vcomTxBuffer[VCOM_TXBUFFER_LENGTH] ;


/* Inline Functions ***************************************************************************************************/
/* Local Functions ****************************************************************************************************/
/* Public Functions ***************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Initialize the VCOM device
 * @param   void
 * @return  Any of the followin codes:
 *          @arg VCOM_OK            Operation terminated without errors
 *          @arg VCOM_BUSY          VCOM device already initialized
 *          @arg VCOM_ALLOC_ERROR   Out of memory
 */
vcomStatus_t vcomInit( void )
{
    if ((NULL == vcomTxQueue) && (NULL == vcomRxQueue))
    {
        if (NULL != (vcomTxQueue = xQueueCreate(VCOM_TXQUEUE_LENGTH, sizeof(uint8_t))))
        {
            if (NULL != (vcomRxQueue = xQueueCreate(VCOM_RXQUEUE_LENGTH, sizeof(uint8_t))))
            {
                if (pdPASS == xTaskCreate(vcomTxTask, "vcomTxTask", VCOM_TXTASK_STACK, NULL,
                                          VCOM_TXTASK_PRIORITY, &vcomTxTaskHandle))
                {
                    return VCOM_OK ;
                }
                vcomTxTaskHandle = NULL ;
                vQueueDelete( vcomRxQueue ) ;
                vcomRxQueue = NULL ;
            }
            vQueueDelete(vcomTxQueue) ;
            vcomTxQueue = NULL ;
        }
        return VCOM_ALLOC_ERROR ;
    }
    return VCOM_BUSY ;
}

/*******************************************************************************************************************//**
 * @brief This function is called by the Communication Device Class driver function CDC_Receive_FS() after a USB packet
 *        has been received from the USB link
 * @param Buf   Pointer to the buffer the packet has been loaded
 * @param Len   Number of item received
 * @return  Any of the followin codes:
 *          @arg USBD_OK    Operation terminated without errors
 *          @arg USBD_BUSY  Receive queue full
 *          @arg USBD_FAIL  VCOM device not initialized yet
 */
USBD_StatusTypeDef CDC_To_App(uint8_t* Buf, uint32_t Len)
{
    BaseType_t Woken = pdFALSE ;
    USBD_StatusTypeDef result = USBD_OK ;
    if (NULL != vcomRxQueue)
    {
        while (Len--)
        {
            if (pdTRUE == xQueueSendToBackFromISR(vcomRxQueue, Buf++, &Woken)) continue ;
            result = USBD_BUSY ;
            break ;
        }
    }
    else result = USBD_FAIL ;
    portYIELD_FROM_ISR( Woken ) ;
    return result ;
}


/*******************************************************************************************************************//**
 * @brief Transmit a single character over the CDC port. The function does not perform any translation
 * @param ch    Character to be transmitted
 * @return      The same argument ch
 */
uint8_t vcomTxChar( uint8_t ch )
{
    xQueueSendToBack(vcomTxQueue, &ch, portMAX_DELAY) ;
    return ch ;
}


/*******************************************************************************************************************//**
 * @brief Receive a single character from the CDC port. The function does not perform any translation
 * @param void
 * @return Received character
 */
uint8_t vcomRxChar( void )
{
    uint8_t ch ;
    xQueueReceive(vcomRxQueue, &ch, portMAX_DELAY) ;
    return ch ;
}


/*******************************************************************************************************************//**
 * @brief Check the status of the Tx queue
 * @param void
 * @return Number of characters waiting o be transmitted
 */
unsigned vcomTxFifoLevel( void )
{
    return (unsigned)uxQueueMessagesWaiting(vcomTxQueue) ;
}


/*******************************************************************************************************************//**
 * @brief Check the status of the Rx queue
 * @param void
 * @return Number of characters received and still not read
 */
unsigned vcomRxFifoLevel( void )
{
    return (unsigned)uxQueueMessagesWaiting(vcomRxQueue) ;
}

/*******************************************************************************************************************//**
 * @brief   Transmit a character to the VCOM device. The character '\n' is prefixed with '\r'
 * @param   ch      Character to be transmitted to the console device
 * @retval  char    Value of the argument 'ch'
 */
char vcomPutChar( char ch )
{
    if ('\n' == ch) vcomTxChar('\r') ;
    vcomTxChar( ch ) ;
    return ch ;
}

/*******************************************************************************************************************//**
 * @brief  Waits and receives a single character from the VCOM device. The function filters-out all the control
 *         characters, with the esclusion of CARRIAGE-RETURN and BACK-SPACE. The TAB characters are translated into
 *         SPACES. The function does not echo the received character
 * @param  void
 * @retval char The character received from the console device
 */
char vcomGetChar( void )
{
    char ch ;
    while (1)
    {
        ch = vcomRxChar() ;
        // Returns any printable char, carriage-return and back-space
        if ((isprint(ch)) || ('\r' == ch) || ('\b' == ch)) return ch ;
        // Translate TAB into SPACE
        if ('\t' == ch) return (' ') ;
    }
}

/******************************************************************************************************************//**
 * @brief  Prints an ANSI-OEM (ASCII) string to the VCOM device. The function translates the '\n' character into the
 *         '\r' '\n' sequence.
 * @param  str      Poiter to a zero-terminated string. If the pointer is NULL, the function doesn't perform any action
 * @retval char *   Value of the argument 'pCh'
 */
char *vcomPutString( const char *str )
{
    if (NULL != str)
    {
        char ch ;
        const char *pc ;
        for (pc = str ; ('\0' != (ch = *(pc++))) ; ) vcomPutChar( ch ) ;
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
char *vcomEditString( char *str, size_t size )
{
    return EditString((void (*)(char))vcomTxChar, (char (*)(void))vcomRxChar, str, size) ;
}

/*******************************************************************************************************************//**
 * @brief  Print a formatted string to the console device
 * @param  *fmt     Format string. The format is like the standard printf format, with the following limitations:
 *                  - Only the %d, %u, %x, %X, %f, %s, %S formats are accepted. The h and l flags are not implemented
 * @param  ...      Variable argument list
 * @retval int      Number of arguments accepted and printed
 */
int vcomPrintf( const char *fmt, ...)
{
    int ret ;
    va_list ap ;
    va_start(ap, fmt) ;
    ret = tiny_vprintf((void (*)(char))vcomPutChar, fmt, ap) ;
    va_end(ap) ;
    return ret ;
}

/*******************************************************************************************************************//**
 * @brief USB CDC test card connection test program
 * @param  void
 * @return void
 */
void vcomTest( void )
{
    coprintf("USB VCOM device test\n") ;
    if (VCOM_ALLOC_ERROR != vcomInit())
    {
        coprintf("Press ESC to stop\n") ;
        do
        {
            vcomPrintf("The quick brown fox jumps over the lazy dog. ") ;
        } while (!coRxReady() || ('\x1B' != coRxChar()));
    }
    else coprintf("vcomInit() FAILED\n") ;
    coprintf("USB test terminated\n") ;
}


/* Local tasks ********************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  Management of trasmission to the CDC port
 * @param  arg      Not used
 * @return void     Never terminates
 */
static void vcomTxTask(void *arg)
{
    UNUSED(arg) ;
    unsigned i ;
    while (true)
    {
        xQueueReceive(vcomTxQueue, &(vcomTxBuffer[i = 0]), portMAX_DELAY) ;
        for ( ; (++i < VCOM_TXBUFFER_LENGTH) && (pdPASS == xQueueReceive(vcomTxQueue, &(vcomTxBuffer[i]), 0)) ; ) ;
        vTaskPrioritySet(NULL, VCOM_TXTASK_PRIORITY - 1) ;
        while (USBD_BUSY == CDC_Transmit_FS(vcomTxBuffer, i)) vTaskDelay(1) ;
        vTaskPrioritySet(NULL, VCOM_TXTASK_PRIORITY) ;
    }
}

/* END OF FILE ********************************************************************************************************/
