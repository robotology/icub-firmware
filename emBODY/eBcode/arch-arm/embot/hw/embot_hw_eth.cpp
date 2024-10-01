
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_eth.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_eth_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"
//#include "embot_hw_gpio.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace embot::hw;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(HAL_ETH_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_eth)

namespace embot { namespace hw { namespace eth {

#if 0
    bool supported(EtH b)
    { return false; }
    bool initialised(EtH b)
    { return false; }
    result_t init(EtH b, const Config &config)
    { return resNOK; }
    result_t deinit(EtH b)
    { return resNOK; }
     
    bool isbusy(embot::hw::EtH b, embot::core::relTime timeout, embot::core::relTime &remaining) 
    { return false; }  
    // blocking      
    result_t read(embot::hw::EtH b, embot::core::Data &destination, embot::core::relTime timeout) 
    { return resNOK; } 
    result_t write(embot::hw::EtH b, const embot::core::Data &source, embot::core::relTime timeout) 
    { return resNOK; }  
    // non blocking
    result_t read(embot::hw::EtH b, embot::core::Data &destination, const embot::core::Callback &oncompletion)
    { return resNOK; }
    result_t write(embot::hw::EtH b, const embot::core::Data &source, const embot::core::Callback &oncompletion)
    { return resNOK; }  

#endif

    bool supported(embot::hw::EtH b)
    { return false; }
    bool initialised(embot::hw::EtH b)
    { return false; }
    result_t init(embot::hw::EtH b)
    { return resNOK; }
    
    ipal_result_t ipal_init(ipal_hal_eth_cfg_t *cfg)
    { return ipal_res_NOK_generic; }
    ipal_result_t ipal_enable(void)
    { return ipal_res_NOK_generic; }
    ipal_result_t ipal_disable(void)
    { return ipal_res_NOK_generic; }
    ipal_result_t ipal_sendframe(ipal_hal_eth_frame_t *frame)
    { return ipal_res_NOK_generic; }
    uint16_t ipal_get_frame_size(void)
    { return 0; }
    void ipal_get_frame(uint16_t length, uint8_t* frame)
    { }
    
    uint64_t ipal_get_mac(void)
    { return 0; }    
    
    bool islinkup(embot::hw::PHY phy) 
    { return false; }

}}} // namespace embot { namespace hw { namespace eth {

#else

namespace embot { namespace hw { namespace eth {
                 
    // initialised mask       
    std::uint32_t initialisedmask = 0;
    
    // the public functions
    
    bool supported(embot::hw::EtH p)
    {
        return embot::hw::eth::bsp::getBSP().supported(p);
    }
    
    bool initialised(embot::hw::EtH p)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(p));
    }    
     
    result_t init(EtH b)
    {
        if(false == supported(b))
        {
            return resNOK;
        }
        
        if(true == initialised(b))
        {
            return resOK;
        }  

        if(!embot::hw::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
                              
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(b));
                
        return resOK;
    }

    
//    result_t deinit(EtH b)
//    {       
//        if(false == initialised(b))
//        {
//            return resOK;
//        }  
//        
//        std::uint8_t index = embot::core::tointegral(b);
//        
//        embot::core::binary::bit::clear(initialisedmask, embot::core::tointegral(b));
//                
//        return resOK;
//    }        

        

    ipal_result_t init(ipal_hal_eth_cfg_t *cfg)
    {
        if(resOK == init(embot::hw::EtH::one))
        {        
            embot::hw::eth::bsp::getBSP().init(cfg);
            return ipal_res_OK;
        }
        return ipal_res_NOK_generic;    
    }


    ipal_result_t enable(void) 
    { 
        embot::hw::eth::bsp::getBSP().enable();	
        return ipal_res_OK;
    }


    ipal_result_t disable(void)
    { 
        embot::hw::eth::bsp::getBSP().disable();	
        return ipal_res_OK;
    }


    ipal_result_t sendframe(ipal_hal_eth_frame_t *frame)
    {
        embot::hw::eth::bsp::getBSP().sendframe(frame);    
        return ipal_res_OK; 
    }


    uint16_t get_frame_size(void)
    { 
        return embot::hw::eth::bsp::getBSP().get_frame_size(); 
    }


    void get_frame(uint16_t length, uint8_t* frame)
    { 
        embot::hw::eth::bsp::getBSP().get_frame(length, frame); 
    }


    uint64_t get_mac(void)
    { 
        return embot::hw::eth::bsp::getBSP().get_mac(); 
    }
        
    bool islinkup(embot::hw::PHY phy)
    {
        return embot::hw::eth::bsp::getBSP().islinkup(phy);
    }  

    uint64_t getnumberoferrors(embot::hw::PHY phy, embot::hw::eth::ERR e)
    {
        return embot::hw::eth::bsp::getBSP().errors(phy, e);
    }    
           
}}} // namespace embot { namespace hw { namespace eth {

 


#endif //defined(HAL_ETH_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

