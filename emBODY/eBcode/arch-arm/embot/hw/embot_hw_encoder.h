
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ENCODER_H_
#define __EMBOT_HW_ENCODER_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_encoder_bsp.h"



namespace embot { namespace hw { namespace encoder {
    
    using POS = uint16_t;
    
    struct Data
    {
        POS position {0};
        bool status {false};
        Data() = default;
        bool isvalid() const { return status; }
    };
    
    struct Config
    {
        // ... max size of eeprom etc. but maybe some of it can go into the embot::hw::eeprom::PROP
        uint32_t tbd {0};
        embot::hw::encoder::Type type {embot::hw::encoder::Type::none};
        constexpr Config() = default;
    };
    

    bool supported(ENCODER e);
    bool initialised(ENCODER e);
    result_t init(ENCODER e, const Config &cfg);
    result_t deinit(ENCODER e);
    result_t start(ENCODER e);
    
    const Config & config(ENCODER e);
    
    // BLOCKING READ.
    // it reads starting from address adr a total of destination.capacity bytes and waits until a timeout. 
    // operation is OK only if read() returns resOK. In this case destination.pointer contains the data; 
    // operation fails if read() returns resNOKtimeout (the timeout has expired) or resNOK (the operation was not even started)
    result_t read(ENCODER e, embot::hw::encoder::Data &destination, embot::core::relTime timeout);
    
    // NON BLOCKING READ.
    // it reads starting from address adr a total of destination.capacity bytes and waits until a timeout. 
    // operation is OK only if read() returns resOK. In this case destination.pointer contains the data; 
    // operation fails if read() returns resNOKtimeout (the timeout has expired) or resNOK (the operation was not even started)
    
    // TODO:
    // result_t read(ENCODER e, embot::hw::encoder::Data &destination);
    
    
}}} // namespace embot { namespace hw { namespace encoder



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


