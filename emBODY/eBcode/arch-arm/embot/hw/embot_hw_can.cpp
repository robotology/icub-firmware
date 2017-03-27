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



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



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
    
    
    
    static void s_transmit(CAN_HandleTypeDef* hcan)
    {
        if(0 == s_Qtx->size())
        {
            __HAL_CAN_DISABLE_IT(hcan, CAN_IT_TME);
            if(nullptr != s_config.txqueueempty.callback)
            {
                s_config.txqueueempty.callback(s_config.txqueueempty.arg);
            }
            return; // resOK;
        }
        
        Frame& frame = s_Qtx->front();
        
        //1) copy frame to hal memory
        hcan->pTxMsg->StdId = frame.id & 0x7FF;
        hcan->pTxMsg->DLC = frame.size;
        memcpy(hcan->pTxMsg->Data, frame.data, frame.size);

        //2) remove the frame from tx queue
        vector<Frame>::iterator b = s_Qtx->begin();
        s_Qtx->erase(b);
        
        //3) transmit frame
        HAL_StatusTypeDef res = HAL_CAN_Transmit_IT(hcan);
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
            
        return; // resOK;
    }
    
    static void txHandler(CAN_HandleTypeDef* hcan)
    {
        //this function is called inside IRQ handler of stm32hal, so hcan could be can1 or can2.
        //therefore i need to check that the interrupt is on the peritherical I already initted.
        
        if( (hcan == (&hcan1)) && initted(Port::one) )
            s_transmit(hcan);
        
        //currently I have not can2!
    }
    
    static void rxHandler(CAN_HandleTypeDef* hcan)
    {
        //to make better
        if(hcan != (&hcan1))
            return;
        
        if(false == initted(Port::one))
        {
            return;
        }
        
        Frame rxframe;
        
        rxframe.id = hcan->pRxMsg->StdId;
        rxframe.size = hcan->pRxMsg->DLC;
        memcpy(rxframe.data, hcan->pRxMsg->Data, rxframe.size);
        
        
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
    
    static void s_errorHandler(CAN_HandleTypeDef* hcan)
    {
        static uint32_t error_count=0;
        error_count++;
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
        if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
        {
        
        }

        //////// configure IRQ handler
        stm32hal_can_configCallback_t  embot_can_irqHandlers;
        embot_can_irqHandlers.onRx = rxHandler;
        embot_can_irqHandlers.onTx = txHandler;
        embot_can_irqHandlers.onError = s_errorHandler;

        stm32hal_can_configureIRQcallback(&embot_can_irqHandlers);
        
        s_initted[port2index(p)] = true;
        return resOK;
    }

    result_t embot::hw::can::enable(Port p)
    {
        if(false == initted(p))
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
        if(false == initted(p))
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
        if(false == initted(p))
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
        if(false == initted(p))
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
        if(false == initted(p))
        {
            return 0;
        }  
        uint32_t size=0;

        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_FMP0);
        size = s_rxQ->size();
        __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
        return size;
    }

    
    
    result_t embot::hw::can::transmit(Port p)
    {
        if(false == initted(p))
        {
            return resNOK;
        } 

        if(Port::one == p)
        {
            embot::hw::can::s_transmit(&hcan1);
            return resOK;
        }
        else
        {
            return resNOK;
        }
    }
    
   

    result_t embot::hw::can::get(Port p, Frame &frame, std::uint8_t &remaining)
    {
        if(false == initted(p))
        {
            return resNOK;
        } 
        
        if(s_rxQ->empty())
        {
            remaining = 0;
            return resNOK;
        }
        
        frame = s_rxQ->front();
        
        __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_FMP0);
        s_rxQ->erase(s_rxQ->begin());
        remaining = s_rxQ->size();
        __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
        
        return resOK;
        
    }

    
    
}}} // namespace embot { namespace hw { namespace can {

