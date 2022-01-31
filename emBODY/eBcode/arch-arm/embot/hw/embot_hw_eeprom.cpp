
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_eeprom.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_hw_bsp_config.h"
#include "embot_hw_eeprom_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"
#include "embot_hw_sys.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif

using namespace std;
using namespace embot::hw;


#if !defined(EMBOT_ENABLE_hw_eeprom)

namespace embot { namespace hw { namespace eeprom {

    bool supported(EEPROM ee)                                                                           
    { return false; }
    bool initialised(EEPROM ee)                                                                         
    { return false; }
    result_t init(EEPROM ee, const Config &config)                                                      
    { return resNOK; }
    const Config & config(EEPROM ee)                                                                    
    { static Config cfg {}; return cfg; }  
    size_t size(EEPROM ee)
    { return 0; }
    result_t read(EEPROM ee, ADR adr, embot::core::Data &destination, embot::core::relTime timeout)
    { return resNOK; }
    result_t write(EEPROM ee, ADR adr, const embot::core::Data &content, embot::core::relTime timeout)
    { return resNOK; }    

}}} // namespace embot { namespace hw { namespace eeprom {

#else

namespace embot { namespace hw { namespace eeprom {
                     
    // initialised mask        
    static std::uint32_t initialisedmask = 0;
    
    struct PrivateData
    {        
        Config config[embot::core::tointegral(EEPROM::maxnumberof)] = {};  
        embot::hw::eeprom::Type type[embot::core::tointegral(EEPROM::maxnumberof)] = {embot::hw::eeprom::Type::none};
        embot::hw::chip::M95512DF *chipM95512DF[embot::core::tointegral(EEPROM::maxnumberof)] = {nullptr};
        PrivateData() = default;
    };
    
    static PrivateData s_privatedata {};    
    
    
    bool supported(EEPROM ee)
    {
        return embot::hw::eeprom::getBSP().supported(ee);
    }
    
    bool initialised(EEPROM ee)
    {
        return embot::core::binary::bit::check(initialisedmask,embot::core::tointegral(ee));
    }
        
    result_t init(EEPROM ee, const Config &cfg)
    {
        if(!supported(ee))
        {
            return resNOK;
        }  
        
        if(initialised(ee))
        {   // dont need to re-init
            return resOK;
        }
                 
        if(!embot::hw::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
        
        const embot::hw::eeprom::BSP &eeprombsp = embot::hw::eeprom::getBSP();
        
        embot::hw::eeprom::Type type = eeprombsp.getPROP(ee)->type;
        
        if(type != embot::hw::eeprom::Type::chipM95512DF)
        {
            return resNOK;
        }
        
        uint8_t index = embot::core::tointegral(ee);
        
        s_privatedata.config[index] = cfg;
        s_privatedata.type[index] = type;
                       
        // bsp specific initialization                    
        eeprombsp.init(ee);  

        // but ... true initialization is in here
        
        if(embot::hw::eeprom::Type::chipM95512DF == type)
        {
            s_privatedata.chipM95512DF[index] = new embot::hw::chip::M95512DF;
            s_privatedata.chipM95512DF[index]->init(eeprombsp.getPROP(ee)->multi.chipM95512DFcfg);
        }
                
        embot::core::binary::bit::set(initialisedmask, index);
                
        return resOK;        
    }
    

    const Config & config(EEPROM ee)
    {
        return s_privatedata.config[embot::core::tointegral(ee)];
    }
    
    size_t size(EEPROM ee)
    {
        if(!initialised(ee))
        {
            return 0;
        } 
        
        size_t s {0};

        if(embot::hw::eeprom::Type::chipM95512DF == s_privatedata.type[embot::core::tointegral(ee)])
        {
            s = embot::hw::chip::M95512DF::storagesize;
        }
        else
        {
            // placeholder for future types
        }
        
        return s;
    }

    result_t read(EEPROM ee, ADR adr, embot::core::Data &destination, embot::core::relTime timeout)
    {
        if(!initialised(ee))
        {
            return resNOK;
        } 
        
        uint8_t index = embot::core::tointegral(ee);
                     
        if(embot::hw::eeprom::Type::chipM95512DF == s_privatedata.type[index])
        {
            s_privatedata.chipM95512DF[index]->read(adr, destination);
        }
        else
        {
            // placeholder for future types
        }

        return resOK;                                   
    }
    

    result_t write(EEPROM ee, ADR adr, const embot::core::Data &content, embot::core::relTime timeout)
    {        
        if(!initialised(ee))
        {
            return resNOK;
        } 
        
        uint8_t index = embot::core::tointegral(ee);
                 
        if(embot::hw::eeprom::Type::chipM95512DF == s_privatedata.type[index])
        {
            s_privatedata.chipM95512DF[index]->write(adr, content);
        }
        else
        {
            // placeholder for future types
        }
        
        return resOK;          
    }

    
}}} // namespace embot { namespace hw { namespace eeprom 

#endif // #if !defined(EMBOT_ENABLE_hw_eeprom)    


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

