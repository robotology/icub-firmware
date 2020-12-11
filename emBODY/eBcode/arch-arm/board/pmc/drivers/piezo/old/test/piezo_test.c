#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "../piezo_ad5664.h"
#include "../gpl_utils.h"
#include "test_tables.h"

#define DAC_CMD_LOADLATCH 0x2
#define DAC_CMD_LOAD 0x0

pthread_t dma_thread;
struct  {
	int size;
	void *src;
	int idx;
	void *arg;
} dma_data;

int dma_stop = 0;

#define __ACCESS_ONCE(x) ({		 \
	typeof(x) __var = (typeof(x)) 0; \
	(volatile typeof(x) *)&(x); })
#define ACCESS_ONCE(x) (*__ACCESS_ONCE(x))
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

void(*dma_cb)(DMA_HandleTypeDef *_hdma);
void(*dma_h_cb)(DMA_HandleTypeDef *_hdma);

void HAL_GPIO_WritePin(int port, int pin, int state)
{
}

#define DAC_DECODE_VAL(x) ((((x) & 0xFF) << 8) | (((x) & 0xFF000000) >> 24))
#define DAC_DECODE_ADR(x) (((x) & 0x700) >> 8)
#define DAC_LDAC(x) (((x) >> 11) & 0x7)

void *dma_worker(void *arg)
{
	uint32_t out;
	int ldac = 0;
	int dac = 0;
	FILE *f = fopen("test_out.csv", "w");

	while(!ACCESS_ONCE(dma_stop)) {
		out = ACCESS_ONCE(((uint32_t*)dma_data.src)[dma_data.idx++]);
		/*
		 * dma_data.size is in 16-bit words
		 * the DMA buffer is accessed in 32-bit words
		 */
		if ((dma_data.idx / 2) == (dma_data.size / 2))
			dma_h_cb(dma_data.arg);
		if ((dma_data.idx / 2) == dma_data.size) {
			dma_data.idx = 0;
			dma_cb(dma_data.arg);
		}
		usleep(5000);

		if (dac != DAC_DECODE_ADR(out))
			printf("DAC idx SEQ error\n");

		if ((dac == 3) && (DAC_LDAC(out) != DAC_CMD_LOADLATCH))
			printf("missing LOADLATCH error\n");

		if ((dac != 3) && (DAC_LDAC(out) != DAC_CMD_LOAD))
			printf("missing LOAD error\n");

		dac = (dac + 1) % 4;

		fprintf(f, "%u", DAC_DECODE_VAL(out));
		if (dac == 3)
			fprintf(f, "\n");
		else
			fprintf(f, ", ");
//		printf("0x%08x - %d %d %s\n", out,
//		       DAC_DECODE_ADR(out), DAC_DECODE_VAL(out),
//		       DAC_LDAC(out) ? "LDAC" : "");

	}
	fclose(f);
}

void HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma,
			      int CallbackID,
			      void(*cb)(DMA_HandleTypeDef *_hdma))
{
	dma_data.arg = hdma;
	if (CallbackID == HAL_DMA_XFER_CPLT_CB_ID)
		dma_cb = cb;
	else
		dma_h_cb = cb;
}

void HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi,
			  uint8_t *pData, uint16_t Size)
{
	dma_data.size = Size;
	dma_data.src = pData;
	dma_data.idx = 0;
	pthread_create(&dma_thread, NULL, &dma_worker, NULL);
}

void HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi)
{
	ACCESS_ONCE(dma_stop) = 1;
	pthread_join(dma_thread, NULL);
}

piezo_dmaspi_handle_t dmaspi;
DMA_HandleTypeDef dummy_dma;
SPI_HandleTypeDef dummy_spi = {
	.Init.Mode = SPI_MODE_MASTER,
	.hdmatx = &dummy_dma
};

int main()
{
	piezo_ad5664_handle_t p;
	piezo_ad5664_cfg_t cfg;

	piezo_dmaspi_init(&dmaspi, &dummy_spi);
	cfg.piezo_freq = 1;
	cfg.max_v = 0x7FFFFFFF;
	/* must be pow of 2 */
	cfg.dma_elem_num = 512;
	cfg.dmaspi = &dmaspi;
//	cfg.phasetable = TESTTABLE_SIN128;
//	cfg.phasetable_len = ARRAY_SIZE(TESTTABLE_SIN128);

//	cfg.phasetable = TESTTABLE_SIN8_128;
//	cfg.phasetable_len = ARRAY_SIZE(TESTTABLE_SIN128);

//	cfg.phasetable = TESTTABLE_CONST8;
//	cfg.phasetable_len = ARRAY_SIZE(TESTTABLE_CONST8);

//	cfg.phasetable = TESTTABLE_RAMP8;
//	cfg.phasetable_len = ARRAY_SIZE(TESTTABLE_RAMP8);

	cfg.phasetable = TESTTABLE_GZ_CSV;
	cfg.phasetable_len = ARRAY_SIZE(TESTTABLE_GZ_CSV);


	piezo_ad5664_init(&p, &cfg);
	piezo_ad5664_set_v(&p, 20000000);
	piezo_ad5664_start(&p);
	sleep(5);
	piezo_ad5664_stop(&p);
}
