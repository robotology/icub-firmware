/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_adc.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

using namespace std;

#include "embot_binary.h"
#include "embot_hw_bsp.h"
#include "embot_hw_bsp_config.h"
#include "stm32hal.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(HAL_ADC_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_adc)

// in here we manage the case of no can module being present in stm32hal

namespace embot { namespace hw { namespace adc {

    bool supported(ADC p)                                                                  { return false; }
    bool initialised(ADC p)                                                                { return false; }
    result_t init(ADC p, const Config &config)                                             { return resNOK; }
    result_t configure(ADC p, void *destination, std::uint8_t numberofitems)               { return resNOK; }
    result_t configure(ADC p, Acquisition acquisition)                                     { return resNOK; }
    result_t configure(ADC p, const embot::common::Callback &oncompletion)                 { return resNOK; }
    result_t start(ADC p)                                                                  { return resNOK; }
    bool isrunning(ADC p)                                                                  { return false; }
    result_t get(ADC p, void *items)                                                       { return resNOK; }
    result_t stop(ADC p)                                                                   { return resNOK; }
    
}}} // namespace embot { namespace hw { namespace adc {

#elif   defined(HAL_ADC_MODULE_ENABLED)


using namespace embot::hw;

// there are two parts: 
// 1. the cpp driver under namespace embot::hw::adc
// 2. the functions which stm library required to be redefined: IRQ handlers and callbacks 

namespace embot { namespace hw { namespace adc {
        
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    

    bool supported(ADC p)
    {
        return embot::hw::bsp::adc::getBSP().supported(p);
    }
    
    bool initialised(ADC p)
    {
        return embot::binary::bit::check(initialisedmask, embot::common::tointegral(p));
    }    

    
    // this is the case of only one adc instance. in case of ADC::maxnumberof > 1 we must have arrays of them
    struct ADCdata
    {
        Config config;
        ADC_HandleTypeDef *handle;
        volatile bool adc_isrunning; // the volatile keyword makes this variable and funtion isrunning() good for polling 
    };
    
    ADCdata adcdata;
    
    
    // called by the IRQ handler at the completion of ADC conversion
    void callbackOnCompletion(ADC_HandleTypeDef* hadc)
    {
        if(hadc != adcdata.handle)
        {
            return;
        }
        
        if(false == initialised(ADC::one))
        {
            return;
        }
        
        if(Acquisition::single == adcdata.config.acquisition)
        {
            stop(ADC::one);
        }
        else
        {
            start(ADC::one);
        }
        
        adcdata.config.oncompletion.execute();
    }    

   
    result_t init(ADC p, const Config &config)
    {
        if(false == supported(p))
        {
            return resNOK;
        }
        
        if(true == initialised(p))
        {
            return resOK;
        }
        
        // init peripherals: adc1 and dma1
        embot::hw::bsp::adc::getBSP().init(p);     
        
        adcdata.config = config;
        adcdata.adc_isrunning = false;
        adcdata.handle = embot::hw::bsp::adc::getBSP().getPROP(p)->handle;
                                
        embot::binary::bit::set(initialisedmask, embot::common::tointegral(p));

        return resOK;
    }
    
    bool isrunning(ADC p)
    { 
        if(false == initialised(p))
        {
            return false;
        }
        
        return adcdata.adc_isrunning;
    }
 
    
    result_t configure(ADC p, Acquisition acquisition)
    {
        if(false == initialised(p))
        {
            return resNOK;
        }
        
        if(true == adcdata.adc_isrunning)
        {
            stop(p);
        }

        adcdata.config.acquisition = acquisition;

        return resOK;         
    }
 
    
    result_t configure(ADC p, const embot::common::Callback &oncompletion)
    {        
        if(false == initialised(p))
        {
            return resNOK;
        }
        
        if(true == adcdata.adc_isrunning)
        {
            stop(p);
        }

        adcdata.config.oncompletion = oncompletion;

        return resOK;                        
    }
 
    
    result_t configure(ADC p, void *destination, std::uint8_t numberofitems)
    {
        if(false == initialised(p))
        {
            return resNOK;
        }

        if((nullptr == destination) || (0 == numberofitems))
        {
            return resNOK;
        } 

        if(true == adcdata.adc_isrunning)
        {
            stop(p);
        }        

        adcdata.config.destination = destination;
        adcdata.config.numberofitems = numberofitems;
        
        
        return resOK;
    }
    

    result_t start(ADC p)
    {
        if(false == initialised(p))
        {
            return resNOK;
        }

        if((nullptr == adcdata.config.destination) || (0 == adcdata.config.numberofitems))
        {
            return resNOK;
        }
        
        if(true == adcdata.adc_isrunning)
        {
            stop(p);
        }
        
        adcdata.adc_isrunning = true;
        HAL_ADC_Start_DMA(adcdata.handle, (uint32_t*)adcdata.config.destination, adcdata.config.numberofitems*static_cast<uint8_t>(adcdata.config.itemsize));
        
        return resOK;           
    }
    
    
    result_t stop(ADC p)
    {
        if(false == initialised(p))
        {
            return resNOK;
        } 
        
        if(true == adcdata.adc_isrunning)
        {
            HAL_ADC_Stop_DMA(adcdata.handle);  
            adcdata.adc_isrunning = false;  
        }

        adcdata.adc_isrunning = false;        

        return resOK;           
    }    
       

    result_t get(ADC p, void *items)
    {
        if(false == initialised(p))
        {
            return resNOK;
        } 
        
        std::memmove(items, adcdata.config.destination, static_cast<std::uint8_t>(adcdata.config.itemsize)*adcdata.config.numberofitems);
        
        return resOK;        
    }
    
    
}}} // namespace embot { namespace hw { namespace adc {


// - stm32hal.lib needs some handlers being compiled in here: IRQ handlers and callbacks.

// IRQ handlers are in hw bsp file


// this function must be re-defined. it is weakly defined in the stm32hal.lib and is called by the HAL_DMA_IRQHandler().
// originally, the funtion HAL_ADC_Start_DMA() sets hdma->XferCpltCallback() equal to ADC_DMAConvCplt() which in its
// inside calls HAL_ADC_ConvCpltCallback().
// when DMA transfer is completed, the interrupt handler DMA1_Channel1_IRQHandler() calls HAL_DMA_IRQHandler() which
// calls hdma->XferCpltCallback(hdma) and then it is executed the function HAL_ADC_ConvCpltCallback() and its content. 
// it is long chain but it is stmicroelectornics which decided that.
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    embot::hw::adc::callbackOnCompletion(hadc);
}

#endif //defined(HAL_ADC_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

