
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_EEPROM_H_
#define __EMBOT_HW_EEPROM_H_

#include "embot_core.h"
#include "embot_hw_types.h"



namespace embot { namespace hw { namespace eeprom {
    
    using ADR = uint16_t;
        
    struct Config
    {   
        // ... max size of eeprom etc. but maybe some of it can go into the embot::hw::eeprom::PROP
        uint32_t tbd {0};               
        constexpr Config() = default;      
    };  
    

    bool supported(EEPROM ee);    
    bool initialised(EEPROM ee);        
    result_t init(EEPROM ee, const Config &cfg);
    
    const Config & config(EEPROM ee);    
    
    
    // BLOCKING READ. 
    // it reads starting from address adr a total of destination.capacity bytes and waits until a timeout. 
    // operation is OK only if read() returns resOK. In this case destination.pointer contains the data; 
    // operation fails if read() returns resNOKtimeout (the timeout has expired) or resNOK (the operation was not even started)
    result_t read(EEPROM ee, ADR adr, embot::core::Data &destination, embot::core::relTime timeout);
    
    // BLOCKING WRITE to a register. 
    // it writes starting from address adr a total of content.size bytes pointed by content.pointer and waits until a timeout.
    // operation is OK only if write() returns resOK. In this case the bytes pointed by content.pointer are guaranteed to be written to register.
    // operation fails if write() returns resNOKtimeout (the timeout has expired) or resNOK (the operation was not even started).
    result_t write(EEPROM ee, ADR adr, const embot::core::Data &content, embot::core::relTime timeout);
       
}}} // namespace embot { namespace hw { namespace eeprom 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


