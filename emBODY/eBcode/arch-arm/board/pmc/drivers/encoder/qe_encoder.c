#include "qe_encoder.h"


/*******************************************************************************************************************//**
 * @brief   Initializes the QE encoder software driver.
 *          This function assumes that the TIMER peripheral has been already initialized with CubeMX functions.
 * @param   e  the encoder object handle
 * @param   c  pointer to the configuration object for the encoder being initialized
 * @return  true on succes, false otherwise
 */
bool qe_encoder_init(qe_encoder_t *e, qe_encoder_cfg_t *c)
{
    e->cfg = *c;
    HAL_TIM_Encoder_Start(e->cfg.htim, TIM_CHANNEL_ALL);

    return true;
}

/*******************************************************************************************************************//**
 * @brief   Gets the current encoder value for the given encoder
 * @param   e    the encoder object handle
 * @param   val  pointer to a variable to be filled with the encoder value
 * @return  true on succes, false otherwise
 */
bool qe_encoder_get(qe_encoder_t *e, int *val)
{
    *val = __HAL_TIM_GET_COUNTER(e->cfg.htim);
    return true;
}
