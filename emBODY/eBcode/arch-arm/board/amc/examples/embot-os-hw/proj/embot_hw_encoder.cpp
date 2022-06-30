
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_encoder.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_hw_bsp_config.h"
#include "embot_hw_encoder_bsp.h"

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


#if !defined(EMBOT_ENABLE_hw_encoder)

namespace embot { namespace hw { namespace encoder {

    bool supported(ENCODER e)
    { return false; }
    bool initialised(ENCODER e)
    { return false; }
    result_t init(ENCODER e, const Config &config)
    { return resNOK; }
    result_t deinit(ENCODER e)
    { return resNOK; }
    result_t start(ENCODER e)
    { return resNOK; }
    const Config & config(ENCODER e)
    { static Config cfg {}; return cfg; }
    const Data & data(ENCODER e)
    { static Data data {}; return data; }
    size_t size(ENCODER e)
    { return 0; }
    result_t read(ENCODER e, embot::hw::encoder::Data &destination, embot::core::relTime timeout)
    { return resNOK; }

}}} // namespace embot { namespace hw { namespace encoder {

#else

namespace embot { namespace hw { namespace encoder {
                     
    // initialised mask
    static std::uint32_t initialisedmask = 0;
    
    struct PrivateData
    {        
        Config config[embot::core::tointegral(ENCODER::maxnumberof)] = {};  
        embot::hw::chip::AS5045 *chipAS5045[embot::core::tointegral(ENCODER::maxnumberof)] = {nullptr};
        embot::hw::chip::MA730  *chipMA730[embot::core::tointegral(ENCODER::maxnumberof)] = {nullptr};
        PrivateData() = default;
    };
    
    static PrivateData s_privatedata {};
    
    
    bool supported(ENCODER e)
    {
        return embot::hw::encoder::getBSP().supported(e);
    }
    
    bool initialised(ENCODER e)
    {
        return embot::core::binary::bit::check(initialisedmask,embot::core::tointegral(e));
    }
    
    result_t init(ENCODER e, const Config &cfg)
    {
        if(!supported(e))
        {
            return resNOK;
        }
        
        if(initialised(e))
        {   // don't need to re-init
            return resOK;
        }
        
        if(!embot::hw::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
        
        const embot::hw::encoder::BSP &encoderbsp = embot::hw::encoder::getBSP();
        
        embot::hw::SPI spiBus = encoderbsp.getPROP(e)->spiBus;
        
        if(cfg.type != embot::hw::encoder::Type::chipAS5045)
        {
            return resNOK;
        }
        
        uint8_t index = embot::core::tointegral(e);
        
        s_privatedata.config[index] = cfg;
                       
        // bsp specific initialization
        encoderbsp.init(e);

        // but ... true initialization is in here
        
        if(embot::hw::encoder::Type::chipAS5045 == cfg.type)
        {
            s_privatedata.chipAS5045[index] = new embot::hw::chip::AS5045;
            s_privatedata.chipAS5045[index]->init({spiBus, s_privatedata.chipAS5045[index]->standardspiconfig} );
        }
        else if(embot::hw::encoder::Type::chipMA730 == cfg.type)
        {
            // placeholder for AEA3
            s_privatedata.chipMA730[index] = new embot::hw::chip::MA730;
        }
        
        embot::core::binary::bit::set(initialisedmask, index);
        
        return resOK;
    }
    
    result_t deinit(ENCODER e)
    {
        if(false == initialised(e))
        {
            return resOK;
        }
        
        const embot::hw::encoder::BSP &encoderbsp = embot::hw::encoder::getBSP();
        embot::hw::encoder::Type type = encoderbsp.getPROP(e)->type;
        
        if(type != embot::hw::encoder::Type::chipAS5045)
        {
            return resNOK;
        }
        
        uint8_t index = embot::core::tointegral(e);
        
        if(embot::hw::encoder::Type::chipAS5045 == type)
        {
            s_privatedata.chipAS5045[index]->deinit();
            delete s_privatedata.chipAS5045[index];
            s_privatedata.chipAS5045[index] = nullptr;
        }
        else if(embot::hw::encoder::Type::chipMA730 == type)
        {
            s_privatedata.chipMA730[index]->deinit();
            delete s_privatedata.chipMA730[index];
            s_privatedata.chipMA730[index] = nullptr;
        }
        
        s_privatedata.config[index] = {};
            
        // bsp specific deinitialization
        encoderbsp.deinit(e);
                
        embot::core::binary::bit::clear(initialisedmask, index);
                
        return resOK;
    }
    
    const Config & config(ENCODER e)
    {
        return s_privatedata.config[embot::core::tointegral(e)];
    }
    
    result_t start(ENCODER e)
    {
        if(!initialised(e))
        {
            return resNOK;
        } 
        
        uint8_t index = embot::core::tointegral(e);
                     
        if(embot::hw::encoder::Type::chipAS5045 == s_privatedata.config[index].type)
        {
            // TODO: AEA2
            return resOK;
        }
        else if(embot::hw::encoder::Type::chipMA730 == s_privatedata.config[index].type)
        {
            // TODO: AEA3
            return resOK;
        }
        else
        {
            // invalid encoder
            return resNOK;
        }
        
    }

    result_t read(ENCODER e, embot::hw::encoder::Data &destination, embot::core::relTime timeout)
    {
        if(!initialised(e))
        {
            return resNOK;
        } 
        
        uint8_t index = embot::core::tointegral(e);
                     
        if(embot::hw::encoder::Type::chipAS5045 == s_privatedata.config[index].type)
        {
            // TODO: temporary save the data read here
            embot::hw::chip::AS5045::Data dd {};
            s_privatedata.chipAS5045[index]->read(dd, timeout);
            destination.position = dd.position;
        }
        else if(embot::hw::encoder::Type::chipMA730 == s_privatedata.config[index].type)
        {
            // placeholder for AEA3
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

