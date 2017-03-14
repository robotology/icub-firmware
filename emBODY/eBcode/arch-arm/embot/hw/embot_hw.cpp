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

#include "embot_hw.h"

#if defined(USE_STM32HAL)
#include "stm32hal.h"
#else
    #include "bsp.h"
    #include "gpio.h"
    #include "can.h"
    #include "stm32l4xx_hal.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>

#include <vector>

using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


//static void s_gpio_init(void)
//{
//    static bool initted = false;
//    if(!initted)
//    {
//        MX_GPIO_Init();
//        initted = true;
//    }
//}

namespace embot { namespace hw { namespace bsp {
    
    static bool initted = false; 

    bool isinitted()
    {
        return initted;
    }

    result_t init()
    {
        if(true == embot::hw::bsp::isinitted())
        {
            return resOK;
        }
        
        // put whatwever is required for ...
        
        bsp_init();
        
        initted = true;
        return resOK;
    }
    

    result_t led_init(LED led)
    {
        if((uint8_t)led > bsp_led_maxnum)
        {
            return resNOK;
        }
        //fake initialization. led (gpio) should be init in bsp_init!
        if(isinitted())
            return resOK;
        else
            return resNOK;
    }
    
    result_t led_init_par(LED led, const void *par)
    {
        if((uint8_t)led > bsp_led_maxnum)
        {
            return resNOK;
        }        
        led_init(led);
        return resOK;
    }
    
    result_t led_on(LED led)
    {
        if((uint8_t)led > bsp_led_maxnum)
        {
            return resNOK;
        }            
        return (bsp_led_on((uint8_t)led));
    }
    
    result_t led_off(LED led)
    {
        if((uint8_t)led > bsp_led_maxnum)
        {
            return resNOK;
        } 
        return (bsp_led_off((uint8_t)led));
    }
    
    result_t led_toggle(LED led)
    {
        if((uint8_t)led > bsp_led_maxnum)
        {
            return resNOK;
        }         
        return (bsp_led_toggle((uint8_t)led));
    }
    

    result_t btn_init(BTN btn)
    {
        if(BTN::zero != btn)
        {
            return resNOK;
        }        
         //fake initialization. led (gpio) should be init in bsp_init!
        if(isinitted())
            return resOK;
        else
            return resNOK;      
    }
    
    bool btn_ispressed(BTN btn)
    {
        //GPIO_PinState b1state = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);    
        //return (GPIO_PIN_RESET == b1state) ? (true) : (false);        
        return false;
        //we need a function in hal bsp.
    }
    
    
}}} // namespace embot { namespace hw { namespace bsp {


    
namespace embot { namespace hw { namespace sys {
    
    
    std::uint32_t clock()
    {
        return SystemCoreClock;
    }
    
    void reset()
    {        
        NVIC_SystemReset();
    }
    
    bool canjump2address(std::uint32_t address)
    {
        return true;
    }
    
    void jump2address(std::uint32_t address)
    {
        volatile std::uint32_t jumpaddr;
        void (*app_fn)(void) = NULL;
        
        if(false == canjump2address(address))
        {
            return;
        }


        // prepare jump address 
        jumpaddr = *(volatile std::uint32_t*) (address + 4);
        // prepare jumping function
        app_fn = (void (*)(void)) jumpaddr;
        // initialize user application's stack pointer 
        __set_MSP(*(volatile uint32_t*) address);
        // jump.
        app_fn(); 
        
        // never in here.
        return;                 
    }
    
    void relocatevectortable(std::uint32_t offset)
    {
        SCB->VTOR = FLASH_BASE | (offset & (uint32_t)0x1FFFFF80);        
    }


    void delay(embot::common::Time t)
    {   
        bsp_delay(t);
    }
    

}}} // namespace embot { namespace hw { namespace sys { 




namespace embot { namespace hw { namespace can {
/**
  * @brief  Enable the specified CAN interrupt.
  * @param  __HANDLE__: CAN handle.
  * @param  __INTERRUPT__: CAN Interrupt.
  * @retval None
  */
#define __HAL_CAN_IS_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->IER) & (__INTERRUPT__)) == (__INTERRUPT__))

    const bool s_supported[static_cast<unsigned int>(PortNumber::value)] = {true, false};
        
    static bool s_initted[static_cast<unsigned int>(PortNumber::value)] = {false, false};
    
    
    static Config s_config;    
    static std::vector<Frame> *s_Qtx;
    static std::vector<Frame> *s_rxQ;
    
    
    //allocate memory for HAL
    static CanTxMsgTypeDef        TxMessage;
    static CanRxMsgTypeDef        RxMessage;
    
    uint8_t port2index(Port p)
    {
        return static_cast<unsigned int>(p);
    }

    bool embot::hw::can::supported(Port p)
    {
        return s_supported[port2index(p)];
    }
    
    bool embot::hw::can::initted(Port p)
    {
        return s_initted[port2index(p)];
    }

    
    result_t embot::hw::can::init(Port p, const Config &config)
    {
        if(false == supported(p))
        {
            return resNOK;
        }
        
        if(true == initted(p))
        {
            return resOK;
        }
        
        s_config = config;
        
        //configure txframe like a data frame with standard ID. (we never use ext or remote frames)
        TxMessage.ExtId = 0;
        TxMessage.IDE = CAN_ID_STD;
        TxMessage.RTR = CAN_RTR_DATA;
        
        //gives memeory to HAL
        hcan1.pTxMsg = &TxMessage;
        hcan1.pRxMsg = &RxMessage;
    
        
        //init peripheral
        MX_CAN1_Init();
        
        // do whatever else is required .... for instance... init the buffers.
        
        s_Qtx = new std::vector<Frame>;
        s_rxQ = new std::vector<Frame>;
        s_Qtx->reserve(config.txcapacity);
        s_rxQ->reserve(config.rxcapacity);
        
        
        
        /*##-2- Configure the CAN Filter ###########################################*/
        CAN_FilterConfTypeDef sFilterConfig;
        sFilterConfig.FilterNumber = 0;
        sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
        sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
        sFilterConfig.FilterIdHigh = 0x0000;
        sFilterConfig.FilterIdLow = 0x0000;
        sFilterConfig.FilterMaskIdHigh = 0x0000;
        sFilterConfig.FilterMaskIdLow = 0x0000;
        sFilterConfig.FilterFIFOAssignment = 0;
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.BankNumber = 14;

        #warning vale ho tolto il filtro
//        if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
//        {
//        /* Filter configuration Error */
//           return resNOK;
//        }
        
//        /////////test
//        volatile int size = 100;
//        size = s_Qtx->size();
//        Frame f;
//        f.id = 2;
//        f.size =
//        f.data[0] = 't';
//        f.data[1] = 'e';
//        f.data[2] = 's';
//        f.data[3] = 't';
//        f.data[4] = 0;

//        s_Qtx->push_back(f);
//        size = s_Qtx->size();
//        f.data[4] = 1;
//        s_Qtx->push_back(f);
//        
//        vector<Frame>::iterator b = s_Qtx->begin();
//        s_Qtx->erase(b);
//        
//        size = s_Qtx->size();
//        
//        b = s_rxQ->begin();
//        s_rxQ->erase(b);
//        
//        size = s_Qtx->size();
//        
//        size=size;
        
        return resOK;
    }

    result_t embot::hw::can::enable(Port p)
    {
        if(true == initted(p))
        {
            return resNOK;
        }  

        //enable rx interrupt
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_TME); //disable interrupt because driver sents frames only on user's request by func hal_can_transmit
        //__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
        HAL_StatusTypeDef res = HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);
        if(res != HAL_OK)
            return resNOK;
        
        return resOK;           
    }
    
    
    result_t embot::hw::can::disable(Port p)
    {
        if(true == initted(p))
        {
            return resNOK;
        }  


        // do whatever is needed
        
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_FMP0);
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_TME);
        
        return resOK;           
    }    

    result_t embot::hw::can::put(Port p, const Frame &frame)
    {
        if(true == initted(p))
        {
            return resNOK;
        }  
        uint8_t tx_is_enabled = __HAL_CAN_IS_ENABLE_IT(&hcan1, CAN_IT_TME);
        
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_TME);
        if(s_Qtx->size() == s_config.txcapacity)
        {
            s_Qtx->erase(s_Qtx->begin());
        }
        // do whatever is needed
        
        s_Qtx->push_back(frame);
        if(tx_is_enabled)
            __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_TME);
        
        return resOK;           
    }   
    
    

    std::uint8_t embot::hw::can::outputqueuesize(Port p)
    {
        if(true == initted(p))
        {
            return 0;
        }  
        
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_TME);
        uint8_t size = s_Qtx->size();
        __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_TME);
        return size;
    }
    
    std::uint8_t embot::hw::can::inputqueuesize(Port p)
    {
        if(true == initted(p))
        {
            return 0;
        }  
        uint32_t size=0;

        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_FMP0);
        size = s_rxQ->size();
        __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
        return size;
    }

    static result_t s_transmit(Port p)
    {
        volatile int size = s_Qtx->size();
        if(0 == s_Qtx->size())
        {
            __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_TME);
            if(nullptr != s_config.txqueueempty.callback)
            {
                s_config.txqueueempty.callback(s_config.txqueueempty.arg);
            }
            return resOK;
        }
        
        Frame& frame = s_Qtx->front();
        
        //1) copy frame to hal memory
        hcan1.pTxMsg->StdId = frame.id & 0x7FF;
        hcan1.pTxMsg->DLC = frame.size;
        memcpy(hcan1.pTxMsg->Data, frame.data, frame.size);

        
        vector<Frame>::iterator b = s_Qtx->begin();
        s_Qtx->erase(b);
        
        size = s_Qtx->size();
        size=size;
        //2) transmit frame
        
        //RIMOSSO PER DEBUG
        HAL_StatusTypeDef res = HAL_CAN_Transmit_IT(&hcan1);
        //HAL_StatusTypeDef res = HAL_OK;
        //the only possible return values are HAL_BUSY or HAL_OK
        if(res == HAL_BUSY)
        {
            uint8_t testonly =0;
            //this means that can is transmitting a frame, so when it finish it advertises me by IRQ handler;
            //i should never be here.
            testonly=testonly;
        }
       
        if(nullptr != s_config.ontxframe.callback)
        {
            s_config.ontxframe.callback(s_config.ontxframe.arg);
        }
            
        
        return resOK;
    }
    
    result_t embot::hw::can::transmit(Port p)
    {
        if(true == initted(p))
        {
            return resNOK;
        } 

         return (embot::hw::can::s_transmit(p));
    }
    
   

    result_t embot::hw::can::get(Port p, Frame &frame, std::uint8_t &remaining)
    {
        if(true == initted(p))
        {
            return resNOK;
        } 
        
        frame = s_rxQ->front();
        
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_FMP0);
        s_rxQ->erase(s_rxQ->begin());
        remaining = s_rxQ->size();
        __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
        
        return resOK;
        
    }

    void embot::hw::can::rxHandler(Port p)
    {
        if(true == initted(p))
        {
            return;
        }
        
        Frame rxframe;
        
        rxframe.id = hcan1.pRxMsg->StdId;
        rxframe.size = hcan1.pRxMsg->DLC;
        memcpy(rxframe.data, hcan1.pRxMsg->Data, rxframe.size);
        
        
        if(s_rxQ->size() == s_config.rxcapacity)
        {
            //remove the oldest frame
             s_rxQ->erase(s_rxQ->begin());
        }
        s_rxQ->push_back(rxframe);
        
        if(nullptr != s_config.onrxframe.callback)
        {
            s_config.onrxframe.callback(s_config.onrxframe.arg);
        }
        
    }
    
}}} // namespace embot { namespace hw { namespace can {
    

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
    static int pippo = 0;
    pippo = pippo;
    embot::hw::can::s_transmit(embot::hw::can::Port::one);

}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    embot::hw::can::rxHandler(embot::hw::can::Port::one);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    static uint32_t errors;
    
    errors++;
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

