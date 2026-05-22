
#ifndef __ETHRAM_H_
#define __ETHRAM_H_

#ifdef __cplusplus
extern "C" {
#endif
 
#include "stm32hal.h"

extern ETH_HandleTypeDef heth;
extern ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT];
extern ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT];
extern uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE];

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard
