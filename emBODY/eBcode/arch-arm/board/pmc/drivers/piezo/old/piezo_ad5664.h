#ifndef __PIEZO_H__
#define __PIEZO_H__

#include <stdint.h>
#include "hal.h"
#include "dmaspi.h"

typedef struct  __attribute__((packed)) {
	uint32_t dac[4];
} piezo_ad5664_dma_buf_t;

typedef struct {
	int max_v;
	int dma_elem_num;
	piezo_dmaspi_handle_t *dmaspi;
	const uint16_t *phasetable;
	int phasetable_len;
	int32_t piezo_freq;
} piezo_ad5664_cfg_t;

typedef struct {
	int running;
	piezo_ad5664_dma_buf_t* dma_buffer;
	int v;
	uint32_t phase;
	int shift;
	uint32_t mask;
	piezo_ad5664_cfg_t cfg;
} piezo_ad5664_handle_t;

void piezo_ad5664_start(piezo_ad5664_handle_t *p);
void piezo_ad5664_stop(piezo_ad5664_handle_t *p);
int piezo_ad5664_set_v(piezo_ad5664_handle_t *p, int v);
int piezo_ad5664_init(piezo_ad5664_handle_t *p, piezo_ad5664_cfg_t *cfg);
void piezo_ad5664_free(piezo_ad5664_handle_t *p);
#endif
