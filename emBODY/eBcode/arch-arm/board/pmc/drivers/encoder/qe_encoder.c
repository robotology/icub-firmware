#include "qe_encoder.h"

// to see EMBOT_ENABLE_hw_qe_emulatedMODE
#include "embot_hw_bsp_pmc_config.h"

#if defined(EMBOT_ENABLE_hw_qe_emulatedMODE)
    #warning EMBOT_ENABLE_hw_qe_emulatedMODE is defined, the hw driver of the qe will give fake values 
#endif

#if defined(EMBOT_ENABLE_hw_qe_emulatedMODE)

    // the sensors measure 1.25 micro-m/step. they have an excursion in steps inside [-5240, +5240]
    #define QE_MIN_VAL -5240
    #define QE_MAX_VAL +5240
    #define QE_RANGE (2*5240)
    #define QE_STEP 20
    
    // i support up to two encoders. 
    // the first starts from the minimum value, the second from its mid range
    static int qe_emulated_val[2] = {QE_MIN_VAL, QE_MIN_VAL + QE_RANGE/2};
    
    void qe_emulated_init(void)
    {
        // nothing for now.
    }
    
    int qe_emulated_get(TIM_HandleTypeDef *htim)
    {
        // we know that the first encoder uses htim2. the second uses htim5
        uint8_t index = (htim == &htim2) ? 0 : 1;

        qe_emulated_val[index] += QE_STEP;
        if(qe_emulated_val[index]>QE_MAX_VAL) 
        {
            qe_emulated_val[index] = QE_MIN_VAL;
        }
        return qe_emulated_val[index];
    }
    
#endif

/*******************************************************************************************************************//**
 * @brief   Initializes the QE encoder software driver.
 *          This function assumes that the TIMER peripheral has been already initialized with CubeMX functions.
 * @param   e  the encoder object handle
 * @param   c  pointer to the configuration object for the encoder being initialized
 * @return  true on succes, false otherwise
 */
bool qe_encoder_init(qe_encoder_t *e, qe_encoder_cfg_t *c)
{   
#if defined(EMBOT_ENABLE_hw_qe_emulatedMODE)
    e->cfg = *c;
    qe_emulated_init();
#else    
    e->cfg = *c;
    HAL_TIM_Encoder_Start(e->cfg.htim, TIM_CHANNEL_ALL);
#endif
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
#if defined(EMBOT_ENABLE_hw_qe_emulatedMODE)
    *val = qe_emulated_get(e->cfg.htim);    
#else    
    *val = __HAL_TIM_GET_COUNTER(e->cfg.htim);
#endif    
    return true;
}
