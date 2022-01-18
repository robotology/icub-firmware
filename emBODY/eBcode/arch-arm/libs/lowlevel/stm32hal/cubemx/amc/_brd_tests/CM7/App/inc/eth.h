/*******************************************************************************************************************//**
 * @file    eth.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 June, 9
 * @brief   Ethernet management functions
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion ******************************************************************************/
#ifndef __ETH_H
#define __ETH_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "main.h"


/* Exported macro ----------------------------------------------------------------------------------------------------*/

/* Exported typedefs -------------------------------------------------------------------------------------------------*/

/* Ethernet frame */
typedef struct
{
    uint8_t dst[6];
    uint8_t src[6];
    uint8_t buf[0];
} EthFrame_t;


/* Exported functions prototypes -------------------------------------------------------------------------------------*/
extern HAL_StatusTypeDef EthInit(void);
extern uint64_t EthHashAddress(uint32_t seed);
extern void EthTest(void);

#ifdef __cplusplus
  } /* extern "C" */
#endif

#endif /* __ETH_H */
/* END OF FILE ********************************************************************************************************/
