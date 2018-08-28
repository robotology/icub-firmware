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

#include "embot_hw_can.h"
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_binary.h"

#include <cstring>
#include <vector>

using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#if     !defined(HAL_CAN_MODULE_ENABLED)

// in here we manage the case of no can module being present in stm32hal

namespace embot { namespace hw { namespace can {

    bool supported(Port p) { return false; }
    
    bool initialised(Port p) { return false; }
    
    result_t init(Port p, const Config &config) { return resNOK; }
    
    result_t enable(Port p)  { return resNOK; }
    
    result_t disable(Port p)  { return resNOK; }
    
    result_t put(Port p, const Frame &frame)  { return resNOK; }
    
    std::uint8_t outputqueuesize(Port p)  { return 0; }
    
    result_t transmit(Port p)  { return resNOK; }
    
    std::uint8_t inputqueuesize(Port p)  { return 0; }
    
    result_t get(Port p, Frame &frame, std::uint8_t &remaining)  { return resNOK; }    
    
}}} // namespace embot { namespace hw { namespace can {

#elif   defined(HAL_CAN_MODULE_ENABLED)



namespace embot { namespace hw { namespace can {
    
    
    struct bspmap_t
    {
        std::uint32_t       mask;
    };
    
    // const support maps
    #if     defined(STM32HAL_BOARD_NUCLEO64) 

    #define STM32HAL_HAS_CAN1 
    
    // yes ... if STM32HAL_BOARD_NUCLEO64 is defined we are never in here because HAL_CAN_MODULE_ENABLED is not defined ... but it is just a reminder
    static const bspmap_t bspmap = 
    {
        0x00000000
    };
   

    #elif   defined(STM32HAL_BOARD_MTB4)
    
    #define STM32HAL_HAS_CAN1 
    
    static const bspmap_t bspmap = 
    {
        0x00000001
    };

    #elif   defined(STM32HAL_BOARD_STRAIN2)
    
    #define STM32HAL_HAS_CAN1 
    
    static const bspmap_t bspmap = 
    {
        0x00000001
    };

    #elif   defined(STM32HAL_BOARD_RFE)
    
    #define STM32HAL_HAS_CAN1 
    #define STM32HAL_HAS_CAN_API_Vxxx
    
    static const bspmap_t bspmap = 
    {
        0x00000001
    };
    
    #else
        #error embot::hw::can::bspmask must be filled    
    #endif
    
    
    
    #if defined(STM32HAL_HAS_CAN_API_Vxxx)
        #warning TODO: write the can-handler for the new APIs of STM32
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
        return embot::binary::bit::check(bspmap.mask, port2index(p));
    }
    
    bool initialised(Port p)
    {
        if(Port::none == p)
        {
            return false;
        }
        return embot::binary::bit::check(initialisedmask, port2index(p));
    }    
    
    
/**
  * @brief  Enable the specified CAN interrupt.
  * @param  __HANDLE__: CAN handle.
  * @param  __INTERRUPT__: CAN Interrupt.
  * @retval None
  */
#define __HAL_CAN_IS_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->IER) & (__INTERRUPT__)) == (__INTERRUPT__))

//    const bool s_supported[static_cast<unsigned int>(PortNumber::value)] = {true, false};
        
//    static bool s_initted[static_cast<unsigned int>(PortNumber::value)] = {false, false};
    
    
    static Config s_config;    
    static std::vector<Frame> *s_Qtx;
    static std::vector<Frame> *s_rxQ;

#if defined(STM32HAL_HAS_CAN_API_Vxxx)

#else    
    //allocate memory for HAL
    static CanTxMsgTypeDef        TxMessage;
    static CanRxMsgTypeDef        RxMessage;
#endif

//#if 0 
//// not used anymore: removed to avoid compiler warnings
//    static void s_transmit_noirq(CAN_HandleTypeDef *hcan)
//    {
//        std::uint32_t size = s_Qtx->size();
//        
//        if(0 == size)
//        {
//            if(nullptr != s_config.txqueueempty.callback)
//            {
//                s_config.txqueueempty.callback(s_config.txqueueempty.arg);
//            }
//            return; // resOK;
//        }
//        
//        for(std::uint32_t i=0; i<size; i++)
//        {
//            const Frame& frame = s_Qtx->at(i);
//            hcan->pTxMsg->StdId = frame.id & 0x7FF;
//            hcan->pTxMsg->DLC = frame.size;
//            std::memmove(hcan->pTxMsg->Data, frame.data, sizeof(hcan->pTxMsg->Data));
//            HAL_StatusTypeDef res = HAL_CAN_Transmit(hcan, 1);
//            if((HAL_OK == res) && (nullptr != s_config.ontxframe.callback))
//            {
//                s_config.ontxframe.callback(s_config.ontxframe.arg);
//            }            
//        }
//        
//        s_Qtx->clear();
//                    
//        return; // resOK;
//    }    
//#endif
    
    static void s_transmit(CAN_HandleTypeDef *hcan)
    {
        if(0 == s_Qtx->size())
        {
            if(nullptr != s_config.txqueueempty.callback)
            {
                s_config.txqueueempty.callback(s_config.txqueueempty.arg);
            }
            return; // resOK;
        }
        
        Frame& frame = s_Qtx->front();
        
        HAL_StatusTypeDef res = HAL_ERROR;

#if defined(STM32HAL_HAS_CAN_API_Vxxx)
    
#else         
        //1) copy frame to hal memory
        hcan->pTxMsg->StdId = frame.id & 0x7FF;
        hcan->pTxMsg->DLC = frame.size;
        memcpy(hcan->pTxMsg->Data, frame.data, frame.size);
        
        //2) transmit frame
        res = HAL_CAN_Transmit_IT(hcan);
#endif
    
        //the only possible return values are HAL_BUSY or HAL_OK
        if(res == HAL_OK)
        {
            //3) if transmission is ok, than I remove the frame from tx queue
            vector<Frame>::iterator b = s_Qtx->begin();
            s_Qtx->erase(b);
            if(nullptr != s_config.ontxframe.callback)
            {
                s_config.ontxframe.callback(s_config.ontxframe.arg);
            }
        }
        else
        {
            ;
            //3) if transmission is not ok, than I leave frame in queue.
            //NOTE: improve management of max num of attempts of transmission.
        }
            
        return; // resOK;
    }
    
    void callbackOnTXcompletion(CAN_HandleTypeDef* hcan)
    {
        //this function is called inside IRQ handler of stm32hal, so hcan could be can1 or can2.
        //therefore i need to check that the interrupt is on the peritherical I already initted.
        
        if( (hcan == (&hcan1)) && initialised(Port::one) )
        {
            s_transmit(hcan);
        }
        //currently I have not can2!
    }
    
    void callbackOnRXcompletion(CAN_HandleTypeDef* hcan)
    {
        //to make better
        if(hcan != (&hcan1))
            return;
        
        if(false == initialised(Port::one))
        {
            return;
        }
        
        Frame rxframe;

#if defined(STM32HAL_HAS_CAN_API_Vxxx)
    
#else          
        rxframe.id = hcan->pRxMsg->StdId;
        rxframe.size = hcan->pRxMsg->DLC;
        memcpy(rxframe.data, hcan->pRxMsg->Data, rxframe.size);
#endif        
        
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
    
    void callbackOnError(CAN_HandleTypeDef* hcan)
    {
        hcan = hcan;
        static uint32_t error_count = 0;
        error_count++;
    }


    
    result_t embot::hw::can::init(Port p, const Config &config)
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

#if defined(STM32HAL_HAS_CAN_API_Vxxx)
    return resNOK;
#else          
        //configure txframe like a data frame with standard ID. (we never use ext or remote frames)
        TxMessage.ExtId = 0;
        TxMessage.IDE = CAN_ID_STD;
        TxMessage.RTR = CAN_RTR_DATA;
        
        //gives memeory to HAL
        hcan1.pTxMsg = &TxMessage;
        hcan1.pRxMsg = &RxMessage;    
    
        // init peripheral
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
        if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
        {
        
        }
    
        embot::binary::bit::set(initialisedmask, port2index(p));

        return resOK;
#endif        
    }

    result_t embot::hw::can::enable(Port p)
    {
        if(false == initialised(p))
        {
            return resNOK;
        }  

        
#if defined(STM32HAL_HAS_CAN_API_Vxxx)
        return resNOK;
#else
        //enable rx interrupt
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_TME); //disable interrupt because driver sents frames only on user's request by func hal_can_transmit
        //__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
        HAL_StatusTypeDef res = HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);
 
        if(res != HAL_OK)
        {
            return resNOK;
        }
        
        return resOK;       
#endif       
    }
    
    
    result_t embot::hw::can::disable(Port p)
    {
        if(false == initialised(p))
        {
            return resNOK;
        }  

        // do whatever is needed
#if defined(STM32HAL_HAS_CAN_API_Vxxx)
        return resNOK;
#else        
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_FMP0);
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_TME);
        
    return resOK; 
#endif                  
    }    

    result_t embot::hw::can::put(Port p, const Frame &frame)
    {
        if(false == initialised(p))
        {
            return resNOK;
        }  
        
#if defined(STM32HAL_HAS_CAN_API_Vxxx)
        return resNOK;
#else        
        uint8_t tx_is_enabled = __HAL_CAN_IS_ENABLE_IT(&hcan1, CAN_IT_TME);
        
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_TME);
        if(s_Qtx->size() == s_config.txcapacity)
        {
            s_Qtx->erase(s_Qtx->begin());
        }
        // do whatever is needed
        
        s_Qtx->push_back(frame);
        if(tx_is_enabled)
        {
            __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_TME);
        }
        
        return resOK;    
#endif               
    }   
    
    

    std::uint8_t embot::hw::can::outputqueuesize(Port p)
    {
        if(false == initialised(p))
        {
            return 0;
        } 
        
#if defined(STM32HAL_HAS_CAN_API_Vxxx)
        return 0;
#else  
        
        uint8_t tx_is_enabled = __HAL_CAN_IS_ENABLE_IT(&hcan1, CAN_IT_TME); 
        
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_TME);
        uint8_t size = s_Qtx->size();
        if(tx_is_enabled)
        {
            __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_TME);
        }
                
        return size;   
#endif    
    }
    
    std::uint8_t embot::hw::can::inputqueuesize(Port p)
    {
        if(false == initialised(p))
        {
            return 0;
        }  
        
#if defined(STM32HAL_HAS_CAN_API_Vxxx)
        return 0;
#else 
        uint32_t size = 0;
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_FMP0);
        size = s_rxQ->size();
        __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
        return size;
#endif                
    }

    
    
    result_t embot::hw::can::transmit(Port p)
    {
        if(false == initialised(p))
        {
            return resNOK;
        } 

        if(Port::one == p)
        {
            embot::hw::can::s_transmit(&hcan1);
            //embot::hw::can::s_transmit_noirq(&hcan1);
            return resOK;
        }
        else
        {
            return resNOK;
        }
    }
    
   

    result_t embot::hw::can::get(Port p, Frame &frame, std::uint8_t &remaining)
    {
        if(false == initialised(p))
        {
            return resNOK;
        } 
        

#if defined(STM32HAL_HAS_CAN_API_Vxxx)
        return resNOK;
#else     
        bool empty = true;        
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_FMP0);
        empty = s_rxQ->empty();
        __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
       
        if(empty)
        {
            remaining = 0;
            return resNOK;
        }
        
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_FMP0);
        frame = s_rxQ->front();        
        s_rxQ->erase(s_rxQ->begin());
        remaining = s_rxQ->size();
        __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
              
        return resOK;     
#endif     
    }

    
    result_t setfilters(Port p, std::uint8_t address)
    {
        if(false ==  supported(p))
        {
            return resNOK;
        } 

#if defined(STM32HAL_HAS_CAN_API_Vxxx)
        return resNOK;
#else 
        
         /* Configure the CAN Filter for message of class polling sensor */
        CAN_FilterConfTypeDef sFilterConfig;
        sFilterConfig.FilterNumber = 0;
        sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
        sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
        sFilterConfig.FilterIdHigh = (0x200 | address) << 5;
        sFilterConfig.FilterIdLow = 0x0000;
        sFilterConfig.FilterMaskIdHigh = 0x0000;
        sFilterConfig.FilterMaskIdLow = 0x0000;
        sFilterConfig.FilterFIFOAssignment = 0;
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.BankNumber = 0;
        if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
        {
            return resNOK;
        }
        
        sFilterConfig.FilterNumber = 1;
        sFilterConfig.FilterIdHigh = 0x20F << 5;
        sFilterConfig.FilterIdLow = 0x0000;
        sFilterConfig.FilterMaskIdHigh = 0x0000;
        sFilterConfig.FilterMaskIdLow = 0x0000;
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.BankNumber = 0;
        if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
        {
            return resNOK;
        }
        
         /* Configure the CAN Filter for message of class bootloader sensor */
        sFilterConfig.FilterNumber = 2;
        sFilterConfig.FilterIdHigh = (0x700 | address) << 5;
        sFilterConfig.FilterIdLow = 0x0000;
        sFilterConfig.FilterMaskIdHigh = 0x0000;
        sFilterConfig.FilterMaskIdLow = 0x0000;
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.BankNumber = 1;
        if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
        {
            return resNOK;
        }
        
        sFilterConfig.FilterNumber = 3;
        sFilterConfig.FilterIdHigh = 0x70F << 5;
        sFilterConfig.FilterIdLow = 0x0000;
        sFilterConfig.FilterMaskIdHigh = 0x0000;
        sFilterConfig.FilterMaskIdLow = 0x0000;
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.BankNumber = 1;
        if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
        {
            return resNOK;
        }


        /* Configure the CAN Filter for message of class bootloader sensor */
        sFilterConfig.FilterNumber = 4;
        sFilterConfig.FilterIdHigh = (0x000 | address) << 5;
        sFilterConfig.FilterIdLow = 0x0000;
        sFilterConfig.FilterMaskIdHigh = 0x0000;
        sFilterConfig.FilterMaskIdLow = 0x0000;
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.BankNumber = 2;
        if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
        {
            return resNOK;
        }
        
        sFilterConfig.FilterNumber = 5;
        sFilterConfig.FilterIdHigh = 0x00F << 5;
        sFilterConfig.FilterIdLow = 0x0000;
        sFilterConfig.FilterMaskIdHigh = 0x0000;
        sFilterConfig.FilterMaskIdLow = 0x0000;
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.BankNumber = 2;
        if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
        {
            return resNOK;
        }
        
        return resOK;   
#endif
    }
    
    
}}} // namespace embot { namespace hw { namespace can {


// - stm32hal.lib needs some handlers being compiled in here: IRQ handlers and callbacks.

#if defined(STM32HAL_HAS_CAN1)

void CAN1_TX_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}

#endif


// these functions must be re-defined. they are weakly defined in the stm32hal.lib 

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
    embot::hw::can::callbackOnTXcompletion(hcan);
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    embot::hw::can::callbackOnRXcompletion(hcan);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    embot::hw::can::callbackOnError(hcan);
}


#endif //defined(HAL_CAN_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

