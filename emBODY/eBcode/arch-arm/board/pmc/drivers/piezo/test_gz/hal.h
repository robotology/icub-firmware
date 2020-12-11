#ifndef __HAL_H__
#define __HAL_H__

#include <stdint.h>

#define USE_HAL_COMP_REGISTER_CALLBACKS 1
#define USE_HAL_SPI_REGISTER_CALLBACKS 1

#define UNUSED(x)
#define LED_ON(x, y)
#define LED_OFF(x, y)
#define LED_CODE(x, y, z)

#define HAL_GPIO_WritePin(x, y, z)

#define HAL_RCC_GetPCLK2Freq() 20000000

typedef int FunctionalState;
#define ENABLE 1
#define DISABLE 0

typedef int HAL_StatusTypeDef;
#define HAL_ERROR -1
#define HAL_OK 0

typedef struct {
	int id;
} COMP_HandleTypeDef;

extern COMP_HandleTypeDef hcomp1;
extern COMP_HandleTypeDef hcomp2;
extern COMP_HandleTypeDef hcomp3;

#define HAL_COMP_Stop(x)
#define HAL_COMP_Start(x)
#define HAL_COMP_TRIGGER_CB_ID 0xcafe
extern void HAL_COMP_RegisterCallback(COMP_HandleTypeDef *hcomp, int a,
				      void(*cb)(COMP_HandleTypeDef *hcomp));

typedef struct {
	int dummy;
} DMA_HandleTypeDef;

#define SPI_MODE_MASTER 132
typedef struct {
	int Mode;
} spi_init_t;

typedef struct {
	int id;
	spi_init_t Init;
	DMA_HandleTypeDef *hdmatx;
} SPI_HandleTypeDef;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;

#define DAC_SYNCEN_GPIO_Port 123
#define DAC_SYNCEN_Pin 456
#define GPIO_PIN_RESET 8
#define GPIO_PIN_SET 10
#define HAL_DMA_XFER_CPLT_CB_ID 0xCDACB
#define HAL_DMA_XFER_HALFCPLT_CB_ID 0xDACB

#define HAL_SPI_TX_COMPLETE_CB_ID 0xCDAC0
#define HAL_SPI_TX_HALF_COMPLETE_CB_ID 0xDAC0

#define HAL_Delay(x);
#define __HAL_DMA_ENABLE_IT(x, y)

void HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma,
			      int CallbackID,
			      void(*cb)(DMA_HandleTypeDef *_hdma));

void HAL_SPI_RegisterCallback(SPI_HandleTypeDef *hspi,
			      int CallbackID,
			      void(*cb)(SPI_HandleTypeDef *_hspi));

void HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi,
			  uint8_t *pData, uint16_t Size);
void HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi);

#define HAL_NVIC_DisableIRQ(x)
#define HAL_SPI_Transmit_IT(...) HAL_OK
#define HAL_SPI_Transmit(...) HAL_OK
#define osDelay(x)

typedef struct {
	int CR1;
} SPI_t;

#define SPI_CR1_BR_Msk 0x7
#define SPI_CR1_BR_Pos 2

extern SPI_t *SPI1;

/*
void HAL_DMA_Start_IT(DMA_HandleTypeDef *h, void *src, void *dst, int size);
void HAL_DMA_Abort(DMA_HandleTypeDef *h);
*/


#endif
