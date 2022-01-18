/*******************************************************************************************************************//**
 * @file    usbd_vcom.h
 * @author  G.Zini
 * @version 1.0
 * @date    2018 November, 5
 * @brief   Virtual COM device driver
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __USBD_VCOM_H
#define __USBD_VCOM_H

#ifdef __cplusplus
    extern "C" {
#endif

/* Includes ***********************************************************************************************************/
#include "usbd_def.h"


/* Public Macros ******************************************************************************************************/

typedef enum
{
    VCOM_OK = 0,
    VCOM_BUSY,
    VCOM_ALLOC_ERROR
} vcomStatus_t ;

/* Public Functions ***************************************************************************************************/

extern vcomStatus_t vcomInit( void ) ;

extern uint8_t vcomTxChar( uint8_t ch ) ;
extern uint8_t vcomRxChar( void ) ;
extern unsigned vcomTxFifoLevel( void ) ;
extern unsigned vcomRxFifoLevel( void ) ;
extern char vcomPutChar( char ch ) ;
extern char vcomGetChar( void ) ;
extern char *vcomPutString( const char *str ) ;
extern char *vcomEditString( char *str, size_t size ) ;
extern int vcomPrintf( const char *fmt, ...) ;

extern void vcomTest( void ) ;


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */
#endif /* __USBD_VCOM_H */

/* END OF FILE ********************************************************************************************************/
