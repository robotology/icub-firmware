#ifndef __HAL_H__
#define __HAL_H__

#include <stdint.h>

typedef struct {
	int dummy;
} DMA_HandleTypeDef;

#define SPI_MODE_MASTER 132
typedef struct {
	int Mode;
} spi_init_t;

typedef struct {
	int dummy;
	spi_init_t Init;
	DMA_HandleTypeDef *hdmatx;
} SPI_HandleTypeDef;

#define DAC_SYNCEN_GPIO_Port 123
#define DAC_SYNCEN_Pin 456
#define GPIO_PIN_RESET 8
#define GPIO_PIN_SET 10
#define HAL_DMA_XFER_CPLT_CB_ID 0xCDACB
#define HAL_DMA_XFER_HALFCPLT_CB_ID 0xDACB

#define HAL_Delay(x);
#define __HAL_DMA_ENABLE_IT(x, y)
void HAL_GPIO_WritePin(int port, int pin, int state);
void HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma,
			      int CallbackID,
			      void(*cb)(DMA_HandleTypeDef *_hdma));

void HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi,
			  uint8_t *pData, uint16_t Size);
void HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi);

/*
void HAL_DMA_Start_IT(DMA_HandleTypeDef *h, void *src, void *dst, int size);
void HAL_DMA_Abort(DMA_HandleTypeDef *h);
*/


#endif
