
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ENCODER_H_
#define __EMBOT_HW_ENCODER_H_

#include "embot_hw_types.h"


namespace embot { namespace hw { namespace encoder {
    
    // usually 16 bits are enough to store an SPI encoder value
    using POS = uint16_t;
    
    /**
     * @brief The list of supported encoders
     */
    enum class Type { none, chipAS5045, chipMA730, chipMB049, encoderEMULATED, encoderTEST}; //, spiAS5045 };
    
    /**
     * @brief This struct is used both internally and externally.
     *        It allow the user to init a specific encoder and configure an optional callback function.
     *        The callback function is called when the encoder value is ready to be read.
     */
    struct Config
    {
        embot::hw::encoder::Type type {embot::hw::encoder::Type::none};
        constexpr Config() = default;
    };
    
    /**
     * @brief  This function returns true if the encoder e is supported by the BSP.
     * @param  e The encoder to use.
     * @return true if the encoder is supported, false otherwise.
     */
    bool supported(ENCODER e);
    
    /**
     * @brief  This function returns true if the encoder e is supported by the BSP.
     * @param  e The encoder to use.
     * @return true if the encoder is initialized, false otherwise.
     */
    bool initialised(ENCODER e);
    
    /**
     * @brief This function initialises the encoder `e` with the configuration `cfg`.
     *        it will also initialize the `embot::hw::encoder::bsp` module.
     * @param e the encoder to use.
     * @param cfg the configuration to use for the encoder `e`.
     * @return resOK if the initialisation is successful, resNOK otherwise. 
     */
    result_t init(ENCODER e, const Config &cfg);
    
    /**
     * @brief This funciton deinitialises the encoder e.
     * 
     * @param e the encoder to deinitialise.
     * @return resOK if the deinitialization was successful, resNOK otherwise. 
     */
    result_t deinit(ENCODER e);
    
    /**
     * @brief  This function starts reading of data from the encoder in a non-blocking way.
     *         When reading is finished, then the callback on reception is invoked, which can
     *         retrieve the value with embot::hw::encoder::read().
     * @param  e the encoder to use.
     * @param  on_completion_userdef a callback to execute when the data is ready
     * @return resNOK on error else resOK.
     */
     result_t startRead(ENCODER e, embot::core::Callback on_completion_userdef = {});
    
    /**
     * @brief This function allow to get the last value read from the encoder `e`.
     *        If the value is not yet ready when this function is called, then `pos` will contain the value 1.
     * 
     * @param e the encoder to use.
     * @param pos reference to the variable where to store the value.
     * @return resOK is the value is ready, resNOK otherwise.
     */
     result_t getValue(ENCODER e, POS &pos);
    
    /**
     * @brief BLOCKING READ.
     *        It reads starting from address adr a total of destination.capacity bytes and waits until a timeout.
     *        The timeout must always be specified. for example 3*embot::core::time1millisec, 500*embot::core::time1microsec, etc. 
     *        operation is OK only if read() returns resOK. In this case destination.pointer contains the data; 
     *        operation fails if read() returns resNOKtimeout (the timeout has expired) or resNOK (the operation was not even started)
     * 
     * @param e the encoder to use.
     * @param pos the reference where to store the data.
     * @param timeout the maximum time to wait.
     * @return resOK if the operation is successful, resNOK otherwise.
     */
     result_t read(ENCODER e, POS &pos, embot::core::relTime timeout);
    
    
}}} // namespace embot { namespace hw { namespace encoder



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


