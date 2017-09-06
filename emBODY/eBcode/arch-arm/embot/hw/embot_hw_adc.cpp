/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

#include "embot_hw.h"
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

//stm32hal_adc_configureCallback


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#if     !defined(HAL_ADC_MODULE_ENABLED)



// in here we manage the case of no can module being present in stm32hal

namespace embot { namespace hw { namespace adc {

    bool supported(Port p)                                                                  { return false; }
    bool initialised(Port p)                                                                { return false; }
    result_t init(Port p, const Config &config)                                             { return resNOK; }
    result_t configure(Port p, void *destination, std::uint8_t numberofitems)               { return resNOK; }
    result_t configure(Port p, Acquisition acquisition)                                     { return resNOK; }
    result_t configure(Port p, const embot::common::Callback &oncompletion)                 { return resNOK; }
    result_t start(Port p)                                                                  { return resNOK; }
    bool isrunning(Port p)                                                                  { return false; }
    result_t get(Port p, void *items)                                                       { return resNOK; }
    result_t stop(Port p)                                                                   { return resNOK; }
    
}}} // namespace embot { namespace hw { namespace adc {

#elif   defined(HAL_ADC_MODULE_ENABLED)


// there are two parts: 
// 1. the cpp driver under namespace embot::hw::adc
// 2. the functions which stm library required to be redefined: IRQ handlers and callbacks 

namespace embot { namespace hw { namespace adc {
        
    struct bspmap_t
    {
        std::uint32_t       mask;
    };
    
    // const support maps
    #if     defined(STM32HAL_BOARD_STRAIN2)    
    
    static const bspmap_t bspmap = 
    {
        0x00000001
    };
   
    #else
        static const bspmap_t bspmap = 
        {
            0x00000000
        };
    #endif
      
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t port2index(Port port)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(port);
    }
        
    bool supported(Port p)
    {
        if((Port::none == p) || (Port::maxnumberof == p))
        {
            return false;
        }
        return embot::common::bit::check(bspmap.mask, port2index(p));
    }
    
    bool initialised(Port p)
    {
        if(Port::none == p)
        {
            return false;
        }
        return embot::common::bit::check(initialisedmask, port2index(p));
    }    

    
    // this is the case of only one adc instance. in case of Port::maxnumberof > 1 we must have arrays of them
    static Config s_config;   
    static volatile bool s_adc_isrunning = false; // the volatile keyword makes this variable and funtion isrunning() good for polling   
   
    
    // called by the IRQ handler at the completion of ADC conversion
    void callbackOnCompletion(ADC_HandleTypeDef* hadc)
    {
        if(hadc != (&hadc1))
        {
            return;
        }
        
        if(false == initialised(Port::one))
        {
            return;
        }
        
        if(Acquisition::single == s_config.acquisition)
        {
            stop(Port::one);
        }
        else
        {
            start(Port::one);
        }
        
        if(nullptr != s_config.oncompletion.callback)
        {
            s_config.oncompletion.callback(s_config.oncompletion.arg);
        }
    }    

   
    result_t init(Port p, const Config &config)
    {
        if(false == supported(p))
        {
            return resNOK;
        }
        
        if(true == initialised(p))
        {
            return resOK;
        }
        
        s_config = config;
                        
        // init peripherals: adc1 and dma1
        MX_ADC1_Init();
        // dma is globally initted by stm32hal_bsp_init() because it hold all dma peripherals
        //MX_DMA_Init(); 
        

        // init internal variables        
        s_adc_isrunning = false;
        
        embot::common::bit::set(initialisedmask, port2index(p));

        return resOK;
    }
    
    bool isrunning(Port p)
    { 
        if(false == initialised(p))
        {
            return false;
        }
        
        return s_adc_isrunning;
    }
 
    
    result_t configure(Port p, Acquisition acquisition)
    {
        if(false == initialised(p))
        {
            return resNOK;
        }
        
        if(true == s_adc_isrunning)
        {
            stop(p);
        }

        s_config.acquisition = acquisition;

        return resOK;         
    }
 
    
    result_t configure(Port p, const embot::common::Callback &oncompletion)
    {        
        if(false == initialised(p))
        {
            return resNOK;
        }
        
        if(true == s_adc_isrunning)
        {
            stop(p);
        }

        s_config.oncompletion = oncompletion;

        return resOK;                        
    }
 
    
    result_t configure(Port p, void *destination, std::uint8_t numberofitems)
    {
        if(false == initialised(p))
        {
            return resNOK;
        }

        if((nullptr == destination) || (0 == numberofitems))
        {
            return resNOK;
        } 

        if(true == s_adc_isrunning)
        {
            stop(p);
        }        

        s_config.destination = destination;
        s_config.numberofitems = numberofitems;
        
        
        return resOK;
    }
    

    result_t start(Port p)
    {
        if(false == initialised(p))
        {
            return resNOK;
        }

        if((nullptr == s_config.destination) || (0 == s_config.numberofitems))
        {
            return resNOK;
        }
        
        s_adc_isrunning = true;
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)s_config.destination, s_config.numberofitems);
        
        return resOK;           
    }
    
    
    result_t stop(Port p)
    {
        if(false == initialised(p))
        {
            return resNOK;
        } 

        HAL_ADC_Stop_DMA(&hadc1);  

        s_adc_isrunning = false;        

        return resOK;           
    }    
       

    result_t get(Port p, void *items)
    {
        if(false == initialised(p))
        {
            return resNOK;
        } 
        
        std::memmove(items, s_config.destination, static_cast<std::uint8_t>(s_config.itemsize)*s_config.numberofitems);
        
        return resOK;        
    }
    
    
}}} // namespace embot { namespace hw { namespace adc {


// - stm32hal.lib needs some handlers being compiled in here: IRQ handlers and callbacks.


void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}

// this IRQHandler is not used if we have DMA ....
//void ADC1_IRQHandler(void)
//{
//    HAL_ADC_IRQHandler(&hadc1);
//}

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

