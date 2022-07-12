
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
    result_t startRead(ENCODER e, embot::core::Callback on_completion_userdef)
    { return resNOK; }
    result_t getValue(ENCODER e, POS &pos)
    { return resNOK; }
    result_t read(ENCODER e, POS &pos, embot::core::relTime timeout)
    { return resNOK; }

}}} // namespace embot { namespace hw { namespace encoder {

#else

namespace embot { namespace hw { namespace encoder {
    
    // initialised mask
    static std::uint32_t initialisedmask = 0;
    
    struct privateData
    {
        embot::hw::chip::AS5045 *chip_AS5045 = {nullptr};
        embot::hw::chip::MA730  *chip_MA730 = {nullptr};
        
        embot::hw::chip::AS5045::Data as5045_data {};
        embot::hw::chip::MA730::Data ma730_data {};
        
        Config config = {};
        volatile bool data_is_ready {false};
        embot::core::Callback onCompletion {nullptr, nullptr};


        privateData() = default;
        void start(const embot::core::Callback &cbk) { data_is_ready = false; onCompletion = cbk; }
        void stop() { data_is_ready = true; onCompletion.execute(); onCompletion.clear();}
    };
    
    std::array<privateData, embot::core::tointegral(ENCODER::maxnumberof)> _data_array = {};
    
    void onChipCompletionReading(void *p)
    {
        privateData* d = reinterpret_cast<privateData*>(p);
        
        // set the data ready to be used and call the userdef callback if exists
        d->stop();
    }
    
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
        
        // TODO: safe guard (remove it when AEA3 is completely implemented)
        if(cfg.type != embot::hw::encoder::Type::chipAS5045)
        {
            return resNOK;
        }
        
        uint8_t index = embot::core::tointegral(e);
        
        _data_array[index].config = cfg;
                       
        // bsp specific initialization
        encoderbsp.init(e);

        // but ... true initialization is in here
        
        if(embot::hw::encoder::Type::chipAS5045 == cfg.type)
        {
            _data_array[index].chip_AS5045 = new embot::hw::chip::AS5045;
            _data_array[index].chip_AS5045->init({spiBus, _data_array[index].chip_AS5045->standardspiconfig} );
        }
        else if(embot::hw::encoder::Type::chipMA730 == cfg.type)
        {
            // TODO: placeholder for AEA3
            //_data_array[index].chip_MA730 = new embot::hw::chip::MA730;
            return resNOK;
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
        
        uint8_t index = embot::core::tointegral(e);
        
        const embot::hw::encoder::BSP &encoderbsp = embot::hw::encoder::getBSP();
        embot::hw::encoder::Type type = _data_array[index].config.type;
        
        if(type != embot::hw::encoder::Type::chipAS5045)
        {
            return resNOK;
        }
        
        if(embot::hw::encoder::Type::chipAS5045 == type)
        {
            _data_array[index].chip_AS5045->deinit();
            delete _data_array[index].chip_AS5045;
            _data_array[index].chip_AS5045 = nullptr;
        }
        else if(embot::hw::encoder::Type::chipMA730 == type)
        {
            _data_array[index].chip_MA730->deinit();
            delete _data_array[index].chip_MA730;
            _data_array[index].chip_MA730 = nullptr;
        }
        
        _data_array[index].config = {};
        
        // bsp specific deinitialization
        encoderbsp.deinit(e);
        
        embot::core::binary::bit::clear(initialisedmask, index);
         
        return resOK;
    }
    
    
    result_t startRead(ENCODER e, embot::core::Callback on_completion_userdef)
    {
        if(!initialised(e))
        {
            return resNOK;
        } 
        
        uint8_t index = embot::core::tointegral(e);
        
        if(embot::hw::encoder::Type::chipAS5045 == _data_array[index].config.type)
        {
            _data_array[index].start(on_completion_userdef);
            
            embot::core::Callback cbk { onChipCompletionReading, &_data_array[index] };
            
            if (true == _data_array[index].chip_AS5045->read(_data_array[index].as5045_data, cbk))
            {
                return resOK;
            }
            else
            {
                return resNOK;
            }
        }
        else if(embot::hw::encoder::Type::chipMA730 == _data_array[index].config.type)
        {
            // TODO: AEA3
            return resNOK;
        }
        else
        {
            // invalid encoder
            return resNOK;
        }
    }
    
    result_t getValue(ENCODER e, POS &pos/*, hal_spiencoder_diagnostic_t* diagn*/)
    {   
        if(!initialised(e))
        {
            return resNOK;
        } 
        
        uint8_t index = embot::core::tointegral(e);
        result_t res { resNOK };
        
        if(embot::hw::encoder::Type::chipAS5045 == _data_array[index].config.type)
        {
            // retrieve the current encoder position from data saved internally.
            if(_data_array[index].data_is_ready)
            {
                pos = _data_array[index].as5045_data.position;
                res = resOK;
            }
        }
        
        return res;
    }
    
    // blocking read
    result_t read(ENCODER e, POS &pos, embot::core::relTime timeout)
    {
        if(!initialised(e))
        {
            return resNOK;
        } 
        
        uint8_t index = embot::core::tointegral(e);
                     
        if(embot::hw::encoder::Type::chipAS5045 == _data_array[index].config.type)
        {
            embot::hw::chip::AS5045::Data dd {};
            _data_array[index].chip_AS5045->read(dd, timeout);
                
            if(dd.status.ok)
            {
                pos = dd.position;
                //destination.status = dd.status.ok;
            }
        }
        else if(embot::hw::encoder::Type::chipMA730 == _data_array[index].config.type)
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

