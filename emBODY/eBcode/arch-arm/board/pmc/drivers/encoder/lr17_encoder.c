#include "lr17_encoder.h"

// this is required to see EMBOT_ENABLE_hw_lr17_emulatedMODE
#include "embot_hw_bsp_pmc_config.h"

#if defined(EMBOT_ENABLE_hw_lr17_emulatedMODE)
    #warning EMBOT_ENABLE_hw_lr17_emulatedMODE is defined, the hw driver of the lr17 will give fake values
#endif

void(*lr17_encoder_cb)(void *arg) = NULL;
void *lr17_encoder_cb_arg;

volatile int lr17_encoder_val = 0;
volatile bool lr17_isvalid = false;

uint16_t lr17_encoder_buf;

static void lr17_spi_cb(SPI_HandleTypeDef *spi);

#if defined(EMBOT_ENABLE_hw_lr17_emulatedMODE)

    // this sensor maps 15 bits into 360 deg
    // hence 1 deg -> 91, 5 deg -> 455, 85 deg -> 7736, 90 -> 8192
    // we emulate a chainsaw waveform in range [5, 85] deg
    #define LR17_MIN_VAL 91
    #define LR17_MAX_VAL 7736
    #define LR17_STEP 20
    
    static int lr17_emulated_val = LR17_MIN_VAL;
    
    void lr17_emulated_init(void)
    {
        lr17_emulated_val = LR17_MIN_VAL;
    }
    
    int lr17_emulated_get(void)
    {
        // i return a simple waveform
        lr17_emulated_val += LR17_STEP;
        if(lr17_emulated_val >= LR17_MAX_VAL)
        {
            lr17_emulated_val = LR17_MIN_VAL;
        }
        return lr17_emulated_val;
    }
    
#endif

/*******************************************************************************************************************//**
 * @brief   Initializes the LR17 encoder software driver.
 *          This function assumes that the SPI peripheral has been already initialized with CubeMX functions.
 * @return  true on succes, false otherwise
 */
bool lr17_encoder_init()
{
#if defined(EMBOT_ENABLE_hw_lr17_emulatedMODE)
    lr17_emulated_init();
#else    
    HAL_SPI_RegisterCallback(&hspi4, HAL_SPI_RX_COMPLETE_CB_ID, lr17_spi_cb);
    lr17_isvalid = false;
    lr17_encoder_val = 0;
#endif    
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
#if defined(EMBOT_ENABLE_hw_lr17_emulatedMODE)
    if(NULL != cb)
    {
        cb(arg);
    }
#else    
    lr17_encoder_cb_arg = arg;
    lr17_encoder_cb = cb;
    HAL_SPI_Receive_IT(&hspi4, (void*) &lr17_encoder_buf, 1);
#endif    
    return true;
}

/*******************************************************************************************************************//**
 * @brief   This function returns the last value read from the encoder.
 * @param   Pointer to a variable to be filled with the encoder value
 * @return  true on succes, false otherwise
 */
bool lr17_encoder_get(int *angle)
{
#if defined(EMBOT_ENABLE_hw_lr17_emulatedMODE)
    *angle = lr17_emulated_get();
    return true;
#else    
    *angle = lr17_encoder_val;
    return lr17_isvalid;
#endif    
}

/*******************************************************************************************************************//**
 * @brief   Internal SPI callback for handling the SPI read completion
 * @param   spi the CubeMX SPI handle
 */
static void lr17_spi_cb(SPI_HandleTypeDef *spi)
{
    lr17_isvalid = (0xffff == lr17_encoder_buf) ? false : true;
    
	lr17_encoder_val =
		(lr17_encoder_buf & 0x7fff);
    if (lr17_encoder_cb)
        lr17_encoder_cb(lr17_encoder_cb_arg);
}



void SPI4_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi4);
}


