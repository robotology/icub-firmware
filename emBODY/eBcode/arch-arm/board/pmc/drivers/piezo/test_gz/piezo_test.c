#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "../piezo.h"
#include "../../utils.h"
#include "../tables/generated/delta_8192_table.c"
#include "../tables/generated/delta_1024_table.c"
#include "../tables/generated/rhomb_8192_table.c"

int dbg_count = 0;

#define DAC_CMD_LOADLATCH 0x2
#define DAC_CMD_LOAD 0x0

SPI_HandleTypeDef hspi1 = {.id = 0};
SPI_HandleTypeDef hspi2 = {.id = 1};
SPI_HandleTypeDef hspi3 = {.id = 2};

COMP_HandleTypeDef hcomp1 = {.id = 0};
COMP_HandleTypeDef hcomp2 = {.id = 1};
COMP_HandleTypeDef hcomp3 = {.id = 2};

SPI_t _SPI1 = {.CR1 = 2};
SPI_t *SPI1 = &_SPI1;

pthread_t dma_thread;
volatile struct  {
	int size;
	void *src;
	void *arg;
	int idx;
} dma_data[3];

volatile int dma_stop = 0;

void(*dma_cb)(DMA_HandleTypeDef *_hdma) = NULL;
void(*dma_h_cb)(DMA_HandleTypeDef *_hdma) = NULL;

void(*spi_cb[3])(SPI_HandleTypeDef *_hdma) = {[0 ... 2] = NULL};
void(*spi_h_cb[3])(SPI_HandleTypeDef *_hdma) = {[0 ... 2] = NULL};

void(*comp_cb[3])(COMP_HandleTypeDef *hcomp) = {[0 ... 2] = NULL};

#define DAC_DECODE_VAL(x) ((((x) & 0xFF) << 8) | (((x) & 0xFF000000) >> 24))
#define DAC_DECODE_ADR(x) (((x) & 0x700) >> 8)
#define DAC_LDAC(x) (((x) >> 11) & 0x7)

void *dma_worker(void *arg)
{
	char fname[128];
	uint32_t out;
	int ldac = 0;
	int dac[3] = {0, 0, 0};
	int err;
	int id;
	FILE *f[3];

	for (id = 0; id < 3; id++) {
		sprintf(fname, "test_out_%d.csv", id);
		f[id] = fopen(fname, "w");
	}

	while (!dma_stop) {
		for (id = 2; id >= 0; id--) {
			err = 0;
			out = ((volatile uint32_t*)dma_data[id].src)[dma_data[id].idx++];
			/*
			 * dma_data.size is in 16-bit words
			 * the DMA buffer is accessed in 32-bit words
			 */
			if (dma_data[id].idx == (dma_data[id].size / 4)) {
				if (dma_h_cb)
					dma_h_cb(dma_data[id].arg);
				if (spi_h_cb[id])
					spi_h_cb[id](dma_data[id].arg);
				//printf("xfer h\n");
			}

			if (dma_data[id].idx == (dma_data[id].size / 2)) {
				dma_data[id].idx = 0;
				if (dma_cb)
					dma_cb(dma_data[id].arg);
				if (spi_cb[id]) {
					spi_cb[id](dma_data[id].arg);
					dbg_count++;
				}
				//printf("xfer c\n");
			}
			usleep(50);

			if (dac[id] != DAC_DECODE_ADR(out)) {
				printf("DAC idx SEQ error; motor %d\n", id);
				err = 1;
			}

			if ((dac[id] == 3) && (DAC_LDAC(out) != DAC_CMD_LOADLATCH)) {
				printf("missing LOADLATCH error; motor %d\n", id);
				err = 1;
			}

			if ((dac[id] != 3) && (DAC_LDAC(out) != DAC_CMD_LOAD)) {
				printf("missing LOAD error; motor %d\n", id);
				err = 1;
			}

//		if (!err)
//			printf("OK\n");

			fprintf(f[id], "%u", DAC_DECODE_VAL(out));
			if (dac[id] == 3)
				fprintf(f[id], "\n");
			else
				fprintf(f[id], ", ");
//			printf("ID:%d - 0x%08x - %d %d %s\n", id, out,
//			       DAC_DECODE_ADR(out), DAC_DECODE_VAL(out),
//			       DAC_LDAC(out) ? "LDAC" : "");

			dac[id] = (dac[id] + 1) % 4;
		}
	}
	for (id = 0; id < 3; id++)
		fclose(f[id]);
}

void HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma,
			      int CallbackID,
			      void(*cb)(DMA_HandleTypeDef *_hdma))
{
	dma_data[0].arg = hdma;
	if (CallbackID == HAL_DMA_XFER_CPLT_CB_ID)
		dma_cb = cb;
	else
		dma_h_cb = cb;
}

void HAL_SPI_RegisterCallback(SPI_HandleTypeDef *hspi,
			      int CallbackID,
			      void(*cb)(SPI_HandleTypeDef *_hspi))
{
	dma_data[hspi->id].arg = hspi;
	if (CallbackID == HAL_SPI_TX_COMPLETE_CB_ID)
		spi_cb[hspi->id] = cb;
	else
		spi_h_cb[hspi->id] = cb;
}

void HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi,
			  uint8_t *pData, uint16_t Size)
{
	dma_data[hspi->id].size = Size;
	dma_data[hspi->id].src = pData;

	if (hspi->id == 0)
		pthread_create(&dma_thread, NULL, &dma_worker, NULL);
}

void HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi)
{

	dma_stop = 1;

	pthread_join(dma_thread, NULL);

}

#define piezo_test_check_state(m, e, t) _piezo_test_check_state(m, e, t, __LINE__)
int _piezo_test_check_state(int m, piezoMotorState_t expected, int timeout_ms, int line)
{
	piezoMotorState_t state;

	timeout_ms /= 10;

	do  {
		piezoGetState(m, &state);
		if (timeout_ms-- == 0)
			break;
		usleep(10000);
	} while (state != expected);

	if (state != expected) {
		printf("wrong state on motor %d (got %d, expected %d) - line %d\n",
		       m, state, expected, line);
		return -1;
	}

	return 0;
}

#define piezo_set_state_and_check(m, mode) _piezo_set_state_and_check(m, mode, __LINE__)
int _piezo_set_state_and_check(int m, piezoMode_t mode, int line)
{
	piezoMotorState_t s;
	switch (mode) {
	case PIEZO_NORMAL:
		s = STATE_NORMAL;
		break;

	case PIEZO_BRAKE:
	case PIEZO_FREEWHEELING:
		s = STATE_STEADY;
		break;
	default:
		printf("unknown state??\n");
		return -1;
	}

	piezoSetMode(m, mode);
	return _piezo_test_check_state(m, s, 2000, line);
}

extern void HAL_COMP_RegisterCallback(COMP_HandleTypeDef *hcomp, int a, void(*cb)(COMP_HandleTypeDef *hcomp))
{
	comp_cb[hcomp->id] = cb;
}

void piezo_test_emulate_overcurrent(int m)
{
	if (comp_cb[m])
		comp_cb[m](NULL);
}

int main()
{

	piezoMotorCfg_t cfg1, cfg2, cfg3;
	int i;

	//static const uint16_t phaseTable[] =
	//{
	    /* Include CSV file (edit and substitute "semicolon" with "comma") */
	//    #include "PhaseTable.csv"
	//};

	cfg1.phaseTable = delta_1024;
	cfg2.phaseTable = delta_8192;
	cfg3.phaseTable = rhomb_8192;

	cfg1.phaseTableLen = 1024;
	cfg2.phaseTableLen = 8192;
	cfg3.phaseTableLen = 8192;

	for (i = 0; i < 3; i++) {
		piezo_test_check_state(i, STATE_NOT_INIT, 0);
	}

	piezoInit(&cfg1, &cfg2, &cfg3);
	piezoSetStepFrequency(0, 5);
	piezoSetStepFrequency(1, 5);
	piezoSetStepFrequency(2, 57);
	sleep(2);
	piezoSetStepFrequency(0, 200);
	piezoSetStepFrequency(1, 100);
	piezoSetStepFrequency(2, 57);
	sleep(1);
	piezo_set_state_and_check(0, PIEZO_BRAKE);
	sleep(1);
	piezo_set_state_and_check(1, PIEZO_BRAKE);
	sleep(1);
	piezo_set_state_and_check(0, PIEZO_NORMAL);
	for (i = 0; i < 50; i++) {
		usleep(20000);
		piezoSetStepFrequency(0, 200 - i*2);
	}

	piezo_set_state_and_check(0, PIEZO_FREEWHEELING);
	piezo_set_state_and_check(1, PIEZO_FREEWHEELING);
	sleep(1);
	piezo_set_state_and_check(0, PIEZO_NORMAL);
	piezoSetStepFrequency(0, 100);
	sleep(1);
  	piezoSetStepFrequency(0, -100);
	piezo_set_state_and_check(2, PIEZO_BRAKE);
	sleep(1);

	/* check for OC from run, freewheel and brake states */
	for (i = 0; i < 3; i++) {
		piezo_test_emulate_overcurrent(i);
		piezo_test_check_state(i, STATE_OVERCURRENT, 500);
	}

	/* try to change mode; it must fail and stay in overcurrent */
	piezoSetMode(1, PIEZO_FREEWHEELING);
	usleep(500000);
	piezo_test_check_state(0, STATE_OVERCURRENT, 0);
	piezoSetMode(0, PIEZO_NORMAL);
	usleep(500000);
	piezo_test_check_state(0, STATE_OVERCURRENT, 0);
	sleep(1);

	/* test overcurrent recovery */
	piezoOvercurrentClear(0);
	piezo_test_check_state(0, STATE_NORMAL, 2000);
	piezoOvercurrentClear(1);
	piezo_test_check_state(1, STATE_STEADY, 2000);
	sleep(1);
	HAL_SPI_DMAStop(&hspi1);
//	printf("count %d\n", dbg_count);
}
