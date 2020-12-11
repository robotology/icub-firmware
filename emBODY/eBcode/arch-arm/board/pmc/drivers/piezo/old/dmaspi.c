#include <stdlib.h>
#include <errno.h>
#include "dmaspi.h"

struct _piezo_dmaspi_table_t{
	struct _piezo_dmaspi_table_t *next;
	piezo_dmaspi_handle_t *data;
};

typedef struct _piezo_dmaspi_table_t piezo_dmaspi_table_t;

static piezo_dmaspi_table_t *piezo_dmaspi_global_table = NULL;

static int dmaspi_register(piezo_dmaspi_handle_t *h)
{
	piezo_dmaspi_table_t **tmp;

	for (tmp = &piezo_dmaspi_global_table; *tmp; tmp = &((*tmp)->next));

	*tmp = calloc(1, sizeof(piezo_dmaspi_handle_t));
	if (!*tmp)
		return -ENOMEM;

	(*tmp)->next = NULL;
	(*tmp)->data = h;

	return 0;
}

static piezo_dmaspi_handle_t *dmaspi_from_dma(DMA_HandleTypeDef *dma)
{
	piezo_dmaspi_table_t *tmp;

	for (tmp = piezo_dmaspi_global_table; tmp; tmp = tmp->next)
		if (tmp->data->spi->hdmatx == dma)
			return tmp->data;

	return NULL;
}

void piezo_dmaspi_cb(DMA_HandleTypeDef *hdma)
{
	piezo_dmaspi_handle_t *dmaspi;

	dmaspi = dmaspi_from_dma(hdma);
	dmaspi->cb(dmaspi->cb_arg);
}

void piezo_dmaspi_hcb(DMA_HandleTypeDef *hdma)
{
	piezo_dmaspi_handle_t *dmaspi;

	dmaspi = dmaspi_from_dma(hdma);
	dmaspi->hcb(dmaspi->cb_arg);
}

int piezo_dmaspi_init(piezo_dmaspi_handle_t *h, SPI_HandleTypeDef *spi)
{
	piezo_dmaspi_handle_t *tmp = dmaspi_from_dma(spi->hdmatx);
	if (tmp)
		return -EINVAL;

	h->spi = spi;

	return dmaspi_register(h);
}

static void piezo_dmaspi_ssel_quirk(void)
{
	HAL_GPIO_WritePin(DAC_SYNCEN_GPIO_Port,
			  DAC_SYNCEN_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(DAC_SYNCEN_GPIO_Port,
			  DAC_SYNCEN_Pin, GPIO_PIN_SET);
}

void piezo_dmaspi_start_cyclic(piezo_dmaspi_handle_t *h, void *data, int size,
			 void (*hcb)(void *arg), void (*cb)(void *arg),
			 void *arg)
{
	h->cb = cb;
	h->hcb = hcb;
	h->cb_arg = arg;

	HAL_DMA_RegisterCallback(h->spi->hdmatx,
				 HAL_DMA_XFER_CPLT_CB_ID, piezo_dmaspi_cb);
	HAL_DMA_RegisterCallback(h->spi->hdmatx,
				 HAL_DMA_XFER_HALFCPLT_CB_ID, piezo_dmaspi_hcb);
	__HAL_DMA_ENABLE_IT(h->spi->hdmatx, DMA_IT_TC | DMA_IT_HT);

	if (h->spi->Init.Mode == SPI_MODE_MASTER)
		piezo_dmaspi_ssel_quirk();

	HAL_SPI_Transmit_DMA(h->spi, data, size);
}

void piezo_dmaspi_stop_cyclic(piezo_dmaspi_handle_t *h)
{
	/*HAL_DMA_Abort(p->dma_handle); */
	HAL_SPI_DMAStop(h->spi);
}
