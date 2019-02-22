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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stm32hal.h"

#include "embot_binary.h"

#include "embot_hw_bsp.h"

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

    bool supported(embot::hw::CAN p) { return false; }
    
    bool initialised(embot::hw::CAN p) { return false; }
    
    result_t init(embot::hw::CAN p, const Config &config) { return resNOK; }
    
    result_t enable(embot::hw::CAN p)  { return resNOK; }
    
    result_t disable(embot::hw::CAN p)  { return resNOK; }
    
    result_t put(embot::hw::CAN p, const Frame &frame)  { return resNOK; }
    
    std::uint8_t outputqueuesize(embot::hw::CAN p)  { return 0; }
    
    result_t transmit(embot::hw::CAN p)  { return resNOK; }
    
    std::uint8_t inputqueuesize(embot::hw::CAN p)  { return 0; }
    
    result_t get(embot::hw::CAN p, Frame &frame, std::uint8_t &remaining)  { return resNOK; }    
    
}}} // namespace embot { namespace hw { namespace can {

#elif   defined(HAL_CAN_MODULE_ENABLED)

namespace embot { namespace hw { namespace can {
        
    #if (STM32HAL_DRIVER_VERSION < 183)
        #define STM32HAL_HAS_CAN_API_PRE_V183
        #warning using can api pre hal driver v183 ... think of updating the stm32hal
    #else
        #undef STM32HAL_HAS_CAN_API_PRE_V183
    #endif
    
      
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
        
    struct CANdata
    {
        Config config; // so far only one. however, it should be an array
        CAN_HandleTypeDef *handle;
        std::vector<Frame> *Qtx;
        std::vector<Frame> *rxQ;
        #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
        CAN_TxHeaderTypeDef TxMessage;
        CAN_RxHeaderTypeDef RxMessage;
        #else    
        //allocate memory for HAL
        CanTxMsgTypeDef        TxMessage;
        CanRxMsgTypeDef        RxMessage;
        #endif        
    };
    

    CANdata candata;
        

    //////////////////// private function  //////////////////
    static void s_transmit(CAN_HandleTypeDef *hcan);
    static void interrupt_RX_enable(void);
    static void interrupt_RX_disable(void);
    static void interrupt_TX_enable(void);
    static void interrupt_TX_disable(void);
    static bool interrupt_TX_is_enabled(void);
    void callbackOnTXcompletion(CAN_HandleTypeDef* hcan);
    void callbackOnRXcompletion(CAN_HandleTypeDef* hcan);
    void callbackOnError(CAN_HandleTypeDef* hcan);

}}};


using namespace embot::hw;
using namespace embot::binary; //to use bit::
        
static void can::interrupt_RX_enable(void)
{
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    HAL_CAN_ActivateNotification(candata.handle, CAN_IT_RX_FIFO0_MSG_PENDING);
    #else         
    __HAL_CAN_ENABLE_IT(candata.handle, CAN_IT_FMP0);
    #endif
}
static void can::interrupt_RX_disable(void)
{
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    HAL_CAN_DeactivateNotification(candata.handle, CAN_IT_RX_FIFO0_MSG_PENDING);
    #else 
    __HAL_CAN_DISABLE_IT(candata.handle, CAN_IT_FMP0);
    #endif        
}
static void can::interrupt_TX_enable(void)
{
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    HAL_CAN_ActivateNotification(candata.handle, CAN_IT_TX_MAILBOX_EMPTY);
    #else        
    __HAL_CAN_ENABLE_IT(candata.handle, CAN_IT_TME);
    #endif
}
static void can::interrupt_TX_disable(void)
{
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)  
    HAL_CAN_DeactivateNotification(candata.handle, CAN_IT_TX_MAILBOX_EMPTY);
    #else        
    __HAL_CAN_DISABLE_IT(candata.handle, CAN_IT_TME);
    #endif
}


static bool can::interrupt_TX_is_enabled(void)
{
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)  
    //__HAL_CAN_IS_ENABLE_IT(candata.handle, CAN_IT_TX_MAILBOX_EMPTY);
    return ((candata.handle->Instance->IER & (CAN_IT_TX_MAILBOX_EMPTY)) == (CAN_IT_TX_MAILBOX_EMPTY));
    #else        
    //__HAL_CAN_IS_ENABLE_IT(candata.handle, CAN_IT_TME);
    return ((candata.handle->Instance->IER & (CAN_IT_TME)) == (CAN_IT_TME));
    #endif
}


bool can::supported(embot::hw::CAN p)
{
    return embot::hw::bsp::can::getBSP().supported(p);
}

bool can::initialised(embot::hw::CAN p)
{
    return embot::binary::bit::check(initialisedmask, embot::common::tointegral(p));
}    

static void can::s_transmit(CAN_HandleTypeDef *hcan)
{
    if(0 == candata.Qtx->size())
    {
        // the execution of the callback is protected
        candata.config.txqueueempty.execute();
        return; // resOK;
    }
    
    Frame& frame = candata.Qtx->front();
    
    HAL_StatusTypeDef res = HAL_ERROR;

    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    HAL_CAN_ActivateNotification(candata.handle, CAN_IT_TX_MAILBOX_EMPTY);
    uint32_t TxMailboxNum;
    candata.TxMessage.StdId = frame.id & 0x7FF;
    candata.TxMessage.DLC = frame.size;
    res = HAL_CAN_AddTxMessage(hcan, &candata.TxMessage, frame.data, &TxMailboxNum); //copies frame into mailbox and enable interrupt
    //currently I don't take care of the number of tx mail box used (TxMailboxNum)
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
        vector<Frame>::iterator b = candata.Qtx->begin();
        candata.Qtx->erase(b);
        // the execution of the callback is protected
        candata.config.ontxframe.execute();
    }
    else
    {
        ;
        //3) if transmission is not ok, than I leave frame in queue.
        //NOTE: improve management of max num of attempts of transmission.
    }
        
    return; // resOK;
}
    
void can::callbackOnTXcompletion(CAN_HandleTypeDef* hcan)
{
    //this function is called inside IRQ handler of stm32hal, so hcan could be can1 or can2.
    //therefore i need to check that the interrupt is on the peritherical I already initted.    
    if( (hcan == (candata.handle)) && initialised(embot::hw::CAN::one) )
    {
        s_transmit(hcan);
    }
    //currently I have not can2!
}
    
void can::callbackOnRXcompletion(CAN_HandleTypeDef* hcan)
{
    //to make better
    if(hcan != (candata.handle))
        return;
    
    if(false == initialised(embot::hw::CAN::one))
    {
        return;
    }
    
    Frame rxframe;

#if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    HAL_StatusTypeDef res = HAL_CAN_GetRxMessage (hcan, CAN_RX_FIFO0, &candata.RxMessage, rxframe.data);
    /*
        if(res!=HAL_OK) dai errore
     */
    rxframe.id = candata.RxMessage.StdId;
    rxframe.size = candata.RxMessage.DLC;
#else          
    rxframe.id = hcan->pRxMsg->StdId;
    rxframe.size = hcan->pRxMsg->DLC;
    memcpy(rxframe.data, hcan->pRxMsg->Data, rxframe.size);
#endif        
    
    if(candata.rxQ->size() == candata.config.rxcapacity)
    {
        //remove the oldest frame
        candata.rxQ->erase(candata.rxQ->begin());
    }
    candata.rxQ->push_back(rxframe);
    
    // the execution of a callback is protected
    candata.config.onrxframe.execute();
    
}
    
void can::callbackOnError(CAN_HandleTypeDef* hcan)
{
    hcan = hcan;
    static uint32_t error_count = 0;
    error_count++;
}


    
result_t can::init(embot::hw::CAN p, const Config &config)
{
    if(false == supported(p))
    {
        return resNOK;
    }
    
    if(true == initialised(p))
    {
        return resOK;
    }    
    
    // init peripheral
    embot::hw::bsp::can::getBSP().init(p);
    
    candata.config = config;
    candata.handle = embot::hw::bsp::can::getBSP().getPROP(p)->handle;

    //configure txframe like a data frame with standard ID. (we never use ext or remote frames)
    candata.TxMessage.ExtId = 0;
    candata.TxMessage.IDE = CAN_ID_STD;
    candata.TxMessage.RTR = CAN_RTR_DATA;
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    // hal doesn't need memory
    #else
    //gives memeory to HAL
    candata.handle->pTxMsg = &candata.TxMessage;
    candata.handle->pRxMsg = &candata.RxMessage;    
    #endif
    
    
    // do whatever else is required .... for instance... init the buffers.
    
    candata.Qtx = new std::vector<Frame>;
    candata.rxQ = new std::vector<Frame>;
    candata.Qtx->reserve(config.txcapacity);
    candata.rxQ->reserve(config.rxcapacity);
              
    /*##-2- Configure the CAN Filter ###########################################*/
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    CAN_FilterTypeDef sFilterConfig;
    #else        
    CAN_FilterConfTypeDef sFilterConfig;
    #endif
    
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = 0;
    sFilterConfig.FilterActivation = ENABLE;
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    sFilterConfig.FilterBank = 0;
    sFilterConfig.SlaveStartFilterBank = 14;
    #else
    sFilterConfig.FilterNumber = 0;
    sFilterConfig.BankNumber = 14;
    #endif
    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {
    
    }

    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    if(HAL_OK != HAL_CAN_RegisterCallback(candata.handle, HAL_CAN_TX_MAILBOX0_COMPLETE_CB_ID, can::callbackOnTXcompletion))
        return resNOK; //TODO: adding clear of vector
    if(HAL_OK != HAL_CAN_RegisterCallback(candata.handle, HAL_CAN_TX_MAILBOX1_COMPLETE_CB_ID, can::callbackOnTXcompletion))
        return resNOK; //TODO: adding clear of vector
    if(HAL_OK != HAL_CAN_RegisterCallback(candata.handle, HAL_CAN_TX_MAILBOX2_COMPLETE_CB_ID, can::callbackOnTXcompletion))
        return resNOK; //TODO: adding clear of vector
    if(HAL_OK != HAL_CAN_RegisterCallback(candata.handle, HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID, can::callbackOnRXcompletion))
        return resNOK; //TODO: adding clear of vector
    if(HAL_OK != HAL_CAN_Start(candata.handle))
        return resNOK; //TODO: adding clear of vector
    #endif
    
    embot::binary::bit::set(initialisedmask, embot::common::tointegral(p));

    return resOK;

}

result_t can::enable(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return resNOK;
    }  
    //disable TX interrupt because driver sents frames only on user's request by func hal_can_transmit
    can::interrupt_TX_disable();
    can::interrupt_RX_enable();
    return resOK;
}


result_t can::disable(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return resNOK;
    }  

    can::interrupt_RX_disable();
    can::interrupt_TX_disable();
    return resOK;                  
}    

result_t can::put(embot::hw::CAN p, const Frame &frame)
{
    if(false == initialised(p))
    {
        return resNOK;
    }  
    
    uint8_t tx_is_enabled = can::interrupt_TX_is_enabled();
    can::interrupt_TX_disable();
    if(candata.Qtx->size() == candata.config.txcapacity)
    {
        candata.Qtx->erase(candata.Qtx->begin());
    }
    
    candata.Qtx->push_back(frame);
    
    if(tx_is_enabled)
    {
        can::interrupt_TX_enable();
    }
    
    return resOK;                 
}   



std::uint8_t can::outputqueuesize(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return 0;
    } 
    
    uint8_t tx_is_enabled = can::interrupt_TX_is_enabled();
    can::interrupt_TX_disable();
    uint8_t size = candata.Qtx->size();
    if(tx_is_enabled)
    {
        can::interrupt_TX_enable();
    }
            
    return size;   

}

std::uint8_t can::inputqueuesize(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return 0;
    } 
    
    uint32_t size = 0;
    
    can::interrupt_RX_disable();       
    
    size = candata.rxQ->size();
    
    can::interrupt_RX_enable();

    return size;
            
}



result_t can::transmit(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return resNOK;
    } 

    if(embot::hw::CAN::one == p)
    {
        can::s_transmit(candata.handle);
        return resOK;
    }
    else
    {
        return resNOK;
    }
}



result_t can::get(embot::hw::CAN p, Frame &frame, std::uint8_t &remaining)
{
    if(false == initialised(p))
    {
        return resNOK;
    } 
    
    bool empty = true;
    can::interrupt_RX_disable();
    
    empty = candata.rxQ->empty();
    
    can::interrupt_RX_enable();
   
    if(empty)
    {
        remaining = 0;
        return resNOK;
    }
    
   can::interrupt_RX_disable();
    
    frame = candata.rxQ->front();        
    candata.rxQ->erase(candata.rxQ->begin());
    remaining = candata.rxQ->size();
    
    can::interrupt_RX_enable();
          
    return resOK;         
}


result_t can::setfilters(embot::hw::CAN p, std::uint8_t address)
{
    if(false == supported(p))
    {
        return resNOK;
    } 

    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    CAN_FilterTypeDef sFilterConfig;
    #else        
    CAN_FilterConfTypeDef sFilterConfig;
    #endif        
    
     /* Configure the CAN Filter for message of class polling sensor */
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = (0x200 | address) << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = 0;
    sFilterConfig.FilterActivation = ENABLE;
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    sFilterConfig.FilterBank = 0;
    sFilterConfig.SlaveStartFilterBank = 0;
    #else
    sFilterConfig.FilterNumber = 0;
    sFilterConfig.BankNumber = 0;
    #endif
    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {
        return resNOK;
    }
    
    
    sFilterConfig.FilterIdHigh = 0x20F << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterActivation = ENABLE;
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    sFilterConfig.FilterBank = 1;
    sFilterConfig.SlaveStartFilterBank = 0;
    #else
    sFilterConfig.FilterNumber = 1;
    sFilterConfig.BankNumber = 0;
    #endif
    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {
        return resNOK;
    }
    
     /* Configure the CAN Filter for message of class bootloader sensor */
    
    sFilterConfig.FilterIdHigh = (0x700 | address) << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterActivation = ENABLE;
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    sFilterConfig.FilterBank = 2;
    sFilterConfig.SlaveStartFilterBank = 1;
    #else
    sFilterConfig.FilterNumber = 2;
    sFilterConfig.BankNumber = 1;
    #endif

    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {
        return resNOK;
    }
    
    
    sFilterConfig.FilterIdHigh = 0x70F << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterActivation = ENABLE;
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    sFilterConfig.FilterBank = 3;
    sFilterConfig.SlaveStartFilterBank = 1;
    #else
    sFilterConfig.FilterNumber = 3;
    sFilterConfig.BankNumber = 1;
    #endif
    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {
        return resNOK;
    }


    /* Configure the CAN Filter for message of class bootloader sensor */
    sFilterConfig.FilterIdHigh = (0x000 | address) << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterActivation = ENABLE;
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    sFilterConfig.FilterBank = 4;
    sFilterConfig.SlaveStartFilterBank = 2;
    #else
    sFilterConfig.FilterNumber = 4;
    sFilterConfig.BankNumber = 2;
    #endif

    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {
        return resNOK;
    }
    
    
    sFilterConfig.FilterIdHigh = 0x00F << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterActivation = ENABLE;
    #if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
    sFilterConfig.FilterBank = 5;
    sFilterConfig.SlaveStartFilterBank = 2;
    #else
    sFilterConfig.FilterNumber = 5;
    sFilterConfig.BankNumber = 2;
    #endif
    
    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {
        return resNOK;
    }
    
    return resOK;   
}
    


// - stm32hal.lib needs some handlers being compiled in here: IRQ handlers and callbacks.

// IRQ handlers are in hw bsp file


#if !defined(STM32HAL_HAS_CAN_API_PRE_V183)
////with this version of API we can register the callback dinamically, but currently I kept the old style.
//void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
//{
//    embot::hw::can::callbackOnTXcompletion(hcan);
//}

//void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
//{
//    embot::hw::can::callbackOnTXcompletion(hcan);
//}

//void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
//{
//    embot::hw::can::callbackOnTXcompletion(hcan);
//}

//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//    embot::hw::can::callbackOnRXcompletion(hcan);
//}
#else

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
#endif

#endif //defined(HAL_CAN_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

