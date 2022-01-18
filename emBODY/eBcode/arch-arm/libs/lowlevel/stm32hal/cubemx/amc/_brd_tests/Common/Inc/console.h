/*******************************************************************************************************************//**
 * @file    console.c
 * @author  G.Zini
 * @version 1.0
 * @date    2018 November, 9
 * @brief   Console management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __CONSOLE_H
#define __CONSOLE_H

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------------------------------------------------------------------*/
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "FreeRTOS.h"


/* Exported macro ----------------------------------------------------------------------------------------------------*/
#define coFlush()   {do{}while(!coTxReady());}

#ifndef WAIT_FOREVER
#define WAIT_FOREVER (portMAX_DELAY)
#endif

#ifndef CONSOLE_TIMEOUT
#define CONSOLE_TIMEOUT (_sec_(1))
#endif

#define dbg_puts(str) coLockedPutString(WAIT_FOREVER,str)
#define dbg_printf(...) coLockedPrintf(WAIT_FOREVER,__VA_ARGS__)


/* Exported functions prototypes -------------------------------------------------------------------------------------*/
extern BaseType_t coInit( void ) ;
extern char coTxChar( char ch ) ;
extern char coRxChar( void ) ;
extern bool coRxReady( void ) ;
extern bool coTxReady( void ) ;
extern char coPutChar( char ch ) ;
extern char coGetChar( void ) ;
extern char *coPutString( const char *str ) ;
extern char *coEditString( char *str, size_t size ) ;
extern int coprintf( const char *fmt, ...) ;
extern bool coAskUnsigned(const char *ask, uint32_t *pNumber) ;
extern bool coAskSigned(const char *ask, int32_t *pNumber) ;
extern bool coAskHex(const char *ask, uint32_t *pNumber) ;
extern BaseType_t coLock( TickType_t xTicksToWait ) ;
extern void coUnLock( void ) ;
extern BaseType_t coLockedPutString(TickType_t xTicksToWait, const char *str) ;
extern BaseType_t coLockedPrintf(TickType_t xTicksToWait, const char *fmt, ...) ;
extern char *coLockedEditString( TickType_t xTicksToWait, char *str, size_t size ) ;


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */
#endif /* __CONSOLE_H */
/* END OF FILE ********************************************************************************************************/
