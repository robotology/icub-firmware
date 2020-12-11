#ifndef __DMASPI_H__
#define __DMASPI_H__

#include "hal.h"

typedef struct {
	SPI_HandleTypeDef *spi;
	void (*hcb)(void *);
	void (*cb)(void *);
	void *cb_arg;
} piezo_dmaspi_handle_t;

int piezo_dmaspi_init(piezo_dmaspi_handle_t *h, SPI_HandleTypeDef *spi);
void piezo_dmaspi_start_cyclic(piezo_dmaspi_handle_t *h, void *data, int size,
			 void (*hcb)(void *arg), void (*cb)(void *arg),
			 void *arg);
void piezo_dmaspi_stop_cyclic(piezo_dmaspi_handle_t *h);

#endif
