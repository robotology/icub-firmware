/*******************************************************************************************************************//**
 * @file    phy.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 June, 9
 * @brief   Ethernet management functions
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion ******************************************************************************/
#ifndef __PHY_H
#define __PHY_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "main.h"


/* Exported macro ----------------------------------------------------------------------------------------------------*/

/* Exported typedefs -------------------------------------------------------------------------------------------------*/

/* Type for argument pTable in function PhyConf() */
typedef struct
{
    uint32_t    size;
    uint32_t    addr;
    uint32_t    mask;
    uint32_t    value;
} PhyConf_t;


/* Exported functions prototypes -------------------------------------------------------------------------------------*/
extern HAL_StatusTypeDef PhyInit(void);
extern void PhyDeInit(void);

extern uint64_t PhyHashAddress(void);
extern HAL_StatusTypeDef PhyRead(uint32_t uReg, uint8_t *pData, size_t uSize);
extern HAL_StatusTypeDef PhyWrite(uint32_t uReg, const uint8_t *pData, size_t uSize);
extern HAL_StatusTypeDef PhyConf(const PhyConf_t *pTable, unsigned length);
extern HAL_StatusTypeDef PhyRead_8bit(uint32_t addr, uint8_t *pData);
extern HAL_StatusTypeDef PhyRead_16bit(uint32_t addr, uint16_t *pData);
extern HAL_StatusTypeDef PhyRead_32bit(uint32_t addr, uint32_t *pData);
extern HAL_StatusTypeDef PhyRead_48bit(uint32_t addr, uint64_t *pData);
extern HAL_StatusTypeDef PhyWrite_8bit(uint32_t addr, uint8_t value);
extern HAL_StatusTypeDef PhyWrite_16bit(uint32_t addr, uint16_t value);
extern HAL_StatusTypeDef PhyWrite_32bit(uint32_t addr, uint32_t value);
extern HAL_StatusTypeDef PhyWrite_48bit(uint32_t addr, uint64_t value);
extern HAL_StatusTypeDef PhyRead_MIIM(uint32_t Port, uint32_t Reg, uint16_t *pValue);
extern HAL_StatusTypeDef PhyWrite_MIIM(uint32_t Port, uint32_t Reg, uint16_t Value);


#ifdef __cplusplus
  } /* extern "C" */
#endif

#endif /* __PHY_H */
/* END OF FILE ********************************************************************************************************/
