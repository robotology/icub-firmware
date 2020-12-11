#include "piezo_ad5664.h"
#include "dmaspi.h"
#include "pmc_piezo.h"
#include "gpl_utils.h"
#include "hal.h"
#include "spi.h"

piezo_ad5664_handle_t pmc_piezo_h[3];
piezo_dmaspi_handle_t dmaspi[3];
/* magic gift from the HW guy.. */
const uint16_t PMC_PIEZO_TABLE[8193] =
{
	#include "PhaseTable.csv"
};

int pmc_piezo_init()
{
	int i;
	piezo_ad5664_cfg_t cfg;
	SPI_HandleTypeDef spi[]= {hspi1, hspi2, hspi3};
	int32_t piezo_freq = (2LL * 4LL * 36LL * 4294967296LL)/
		(int32_t)HAL_RCC_GetPCLK2Freq();

	cfg.phasetable = PMC_PIEZO_TABLE;
	cfg.phasetable_len = ARRAY_SIZE(PMC_PIEZO_TABLE);
	cfg.piezo_freq = piezo_freq;
	/* magic from GZ */
	cfg.max_v = 3000 ;
	/* must be pow of 2 */
	cfg.dma_elem_num = 512;

	for (i = 0; i < 3; i++) {
		piezo_dmaspi_init(&dmaspi[i], &spi[i]);
		cfg.dmaspi = &dmaspi[i];
		piezo_ad5664_init(&pmc_piezo_h[i], &cfg);
	}

	return 0;
}
