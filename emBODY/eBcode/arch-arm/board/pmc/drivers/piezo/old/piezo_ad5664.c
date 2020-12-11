#include <string.h>
#include <malloc.h>
#include <errno.h>
#include "piezo_ad5664.h"
#include "gpl_utils.h"

#define DAC_CMD_LOAD 0x0
#define DAC_CMD_LOADLATCH 0x2

/* Macro: set a 32 bits word to be transmitted as two 16 bits half-word */
#define PIEZO_SETDACVALUE(cmd,dac,val)			\
	((((uint32_t)(cmd) << 11) & 0x00003800UL) |	\
	 (((uint32_t)(dac) << 8) & 0x00000700UL) |	\
	 (((uint32_t)(val) >> 8) & 0x000000FFUL) |	\
	 (((uint32_t)(val) << 24) & 0xFF000000UL))

int piezo_ad5664_init(piezo_ad5664_handle_t *p, piezo_ad5664_cfg_t *cfg)
{
	int _shift;

	memset(p, 0, sizeof(*p));
	p->cfg = *cfg;
	_shift = fls(p->cfg.phasetable_len) - 1;
	p->shift = 32 - _shift;
	p->mask = (1 << _shift) - 1;
	p->dma_buffer = (piezo_ad5664_dma_buf_t*)calloc(p->cfg.dma_elem_num,
						 sizeof(piezo_ad5664_dma_buf_t));
	if (!p->dma_buffer)
		return -ENOMEM;

	return 0;
}

void piezo_ad5664_free(piezo_ad5664_handle_t *p)
{
	free(p->dma_buffer);
}

static inline void piezo_ad5664_dma_update(piezo_ad5664_handle_t *p, int half)
{
	int i, j;
	uint16_t val;
	int idx[4];
	piezo_ad5664_dma_buf_t *buf;
	uint32_t cmd;
	int vel = ACCESS_ONCE(p->v);
	int len = p->cfg.dma_elem_num;
	/*
	 * on half DMA IRQ    :  DMA works on 2nd part, FW on 1st part.
	 * on complete DMA IRQ:  FW works on 1st part, DMA on 2nd part.
	 */
	buf = p->dma_buffer + (half ? 0 : len);

	/* for each entry in (each of 4) buffer.. */
	for (i = 0; i < len; i++) {
		p->phase += vel;
		/* GZ magic... */
		idx[0] = (p->phase >> p->shift) & p->mask;
		idx[1] = (~p->phase >> p->shift) & p->mask;
		idx[2] = (( p->phase + 0x80000000UL) >>
			  p->shift) & p->mask;
		idx[3] = ((~p->phase + 0x80000000UL) >>
			  p->shift) & p->mask;

		for (j = 0; j < 4; j++) {
			val = p->cfg.phasetable[idx[j]];
			cmd = (j == 3) ? DAC_CMD_LOADLATCH : DAC_CMD_LOAD;
			ACCESS_ONCE(buf[i].dac[j]) = PIEZO_SETDACVALUE(cmd, j, val);
		}
	}
}

static void piezo_ad5664_dma_complete_cb(void *arg)
{
	piezo_ad5664_handle_t *p = arg;
	piezo_ad5664_dma_update(p, 0);
}

static void piezo_ad5664_dma_half_cb(void *arg)
{
	piezo_ad5664_handle_t *p = arg;
	piezo_ad5664_dma_update(p, 1);
}

static void piezo_ad5664_release(piezo_ad5664_handle_t *p)
{
	piezo_ad5664_stop(p);
	free(p->dma_buffer);
}

void piezo_ad5664_start(piezo_ad5664_handle_t *p)
{
	if (p->running)
		return;

	p->running = 1;

	piezo_ad5664_dma_update(p, 0);
	piezo_ad5664_dma_update(p, 1);

	/* xfer N 16-bit words, hence we divide len by 2 */
	piezo_dmaspi_start_cyclic(p->cfg.dmaspi,
				  p->dma_buffer, p->cfg.dma_elem_num *
				  sizeof(piezo_ad5664_dma_buf_t) / 2,
				  piezo_ad5664_dma_half_cb,
				  piezo_ad5664_dma_complete_cb, p);
}

void piezo_ad5664_stop(piezo_ad5664_handle_t *p)
{
	if (!p->running)
		return;

	piezo_dmaspi_stop_cyclic(p->cfg.dmaspi);
}

int piezo_ad5664_set_v(piezo_ad5664_handle_t *p, int v)
{
	if ((v > p->cfg.max_v) || (v < -p->cfg.max_v))
		return -EINVAL;

	ACCESS_ONCE(p->v) = v * p->cfg.piezo_freq;

	return 0;
}

#if 0
uint32_t piezo_ad5664_interpolate(uint32_t a, uint32_t b, int idx, int max)
{
	double ret;

	ret = (double)a / (double)max * (double)idx;
	ret += (double)b / (double)max * (double)(max - idx);
	return (uint32_t)ret;
}
#endif
