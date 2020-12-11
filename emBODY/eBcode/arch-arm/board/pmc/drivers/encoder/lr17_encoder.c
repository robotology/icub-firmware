#include "lr17_encoder.h"

void(*lr17_encoder_cb)(void *arg) = NULL;
void *lr17_encoder_cb_arg;

volatile int lr17_encoder_val = 0;

uint16_t lr17_encoder_buf;

static void lr17_spi_cb(SPI_HandleTypeDef *spi);

/*******************************************************************************************************************//**
 * @brief   Initializes the LR17 encoder software driver.
 *          This function assumes that the SPI peripheral has been already initialized with CubeMX functions.
 * @return  true on succes, false otherwise
 */
bool lr17_encoder_init()
{
    HAL_SPI_RegisterCallback(&hspi4, HAL_SPI_RX_COMPLETE_CB_ID, lr17_spi_cb);

    return true;
}

/*******************************************************************************************************************//**
 * @brief   Starts an encoder value acquisition. The value could be retrieved later using lr17_encoder_get()
 *          Optionally an user callback will be called when the new data is available
 * @param   cb   an optional function pointer for the acquire-complete callback. Can be NULL
 *          arg  an user argument to be passed to the acquire-complete callback.
 * @return  true on succes, false otherwise
 */
bool lr17_encoder_acquire(void(*cb)(void *arg), void *arg)
{
    lr17_encoder_cb_arg = arg;
    lr17_encoder_cb = cb;

    HAL_SPI_Receive_IT(&hspi4, (void*) &lr17_encoder_buf, 1);
    return true;
}

/*******************************************************************************************************************//**
 * @brief   This function returns the last value read from the encoder.
 * @param   Pointer to a variable to be filled with the encoder value
 * @return  true on succes, false otherwise
 */
bool lr17_encoder_get(int *angle)
{
    *angle = lr17_encoder_val;
    return true;
}

/*******************************************************************************************************************//**
 * @brief   Internal SPI callback for handling the SPI read completion
 * @param   spi the CubeMX SPI handle
 */
static void lr17_spi_cb(SPI_HandleTypeDef *spi)
{
	lr17_encoder_val =
		(lr17_encoder_buf & 0x7fff);
    if (lr17_encoder_cb)
        lr17_encoder_cb(lr17_encoder_cb_arg);
}
