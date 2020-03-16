/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

#include "embot_core_binary.h"
#include "embot_hw_bsp.h"
#include "embot_hw_bsp_config.h"
#include "embot_hw_sys.h"
#include "osal_task.h"

#include <cstring>
#include <vector>


using namespace std;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#if !defined(HAL_CAN_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_can)

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

#else

namespace embot { namespace hw { namespace can {
        
    #if defined(STM32HAL_STM32L4) && (STM32HAL_DRIVER_VERSION < 0x183)
        #define STM32HAL_HAS_CAN_API_PRE_V183
        #warning using can api pre hal driver v183 ... think of updating the stm32hal
        #error UPDATE the application to use a newer version of stm32 lib
    #endif
         
       
    struct CANdata
    {
        Config config {};
        CAN_HandleTypeDef *handle {nullptr};
        std::vector<Frame> *Qtx {nullptr};
        std::vector<Frame> *rxQ {nullptr};
        
        CANdata() = default;
    };
    
    // initialised mask: only one for all buses      
    static std::uint32_t initialisedmask = 0;
    
    // it should be an array of them CAN. but so far we only have CAN::one, hence let's do it simple.
    // std::array<CANdata, embot::core::tointegral(embot::hw::CAN::maxnumberof)> candata_ {};
    CANdata candata {};

    
    // - declaration of helper functions which are not in the API  
    static void s_tx_start(CAN_HandleTypeDef *hcan);
    static void s_tx_oneframehasgone(CAN_HandleTypeDef *hcan);

    void callbackOnTXcompletion(CAN_HandleTypeDef* hcan);
    void callbackOnRXcompletion(CAN_HandleTypeDef* hcan);
    // void callbackOnError(CAN_HandleTypeDef* hcan); // not used, so far
        
    static void RX_IRQenable(void);
    static void RX_IRQdisable(void);
        
    static void tx_IRQenable();
    static bool tx_IRQdisable(); // returns status before disabling it
    static void tx_IRQresume(const bool previouslyenabled); // previouslyenabled = tx_IRQdisable();
       
}}};


using namespace embot::hw;      

bool can::supported(embot::hw::CAN p)
{
    return embot::hw::bsp::can::getBSP().supported(p);
}

bool can::initialised(embot::hw::CAN p)
{
    return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(p));
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
    
    // do whatever else is required .... for instance... init the buffers.
    
    candata.Qtx = new std::vector<Frame>;
    candata.rxQ = new std::vector<Frame>;
    candata.Qtx->reserve(config.txcapacity);
    candata.rxQ->reserve(config.rxcapacity);
              
    /*##-2- Configure the CAN Filter ###########################################*/
    CAN_FilterTypeDef sFilterConfig;

    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = 0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.SlaveStartFilterBank = 14;

    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {    
    }

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
    
    embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(p));

    return resOK;
}


result_t can::enable(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return resNOK;
    }  
    // i dont enable the TX interrupt because the driver sends frames only on user's request
    // the RX interrupt is instead always enabled
    can::tx_IRQdisable();
    can::RX_IRQenable();
    return resOK;
}


result_t can::disable(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return resNOK;
    }  
    // i disable both TX and RX
    can::RX_IRQdisable();
    can::tx_IRQdisable();
    return resOK;                  
}    


result_t can::put(embot::hw::CAN p, const Frame &frame)
{
    if(false == initialised(p))
    {
        return resNOK;
    }  
    
    // protect Qtx: i disable tx interrupt but i keep info if it was enabled, so that at the end i re-enable it
    volatile bool isTXenabled = tx_IRQdisable();
    
    // add the frame in the circular tx fifo
    if(candata.Qtx->size() == candata.config.txcapacity)
    {
        candata.Qtx->erase(candata.Qtx->begin());
    }    
    candata.Qtx->push_back(frame);
    
    // re-enable the tx interrupt if it was previously enabled
    tx_IRQresume(isTXenabled);
    
    return resOK;                 
}   


std::uint8_t can::outputqueuesize(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return 0;
    } 
    
    // protect Qtx: i disable tx interrupt but i keep info if it was enabled, so that at the end i re-enable it
    volatile bool isTXenabled = tx_IRQdisable();
    
    volatile size_t size = candata.Qtx->size();
    
    tx_IRQresume(isTXenabled);
            
    return static_cast<uint8_t>(size);   
}


std::uint8_t can::inputqueuesize(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return 0;
    } 
        
    // protect rxQ: i disable rx interrupt and then i re-enable it
    can::RX_IRQdisable();           
    volatile size_t size = candata.rxQ->size();    
    can::RX_IRQenable();

    return static_cast<uint8_t>(size);
            
}


result_t can::transmit(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return resNOK;
    } 

    if(embot::hw::CAN::one == p)
    {    
        can::s_tx_start(candata.handle);     
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
    
    result_t res = resNOK;
    remaining = 0;    
    Frame rxf {};
    
    // protect rxQ: i disable rx interrupt and then i re-enable it
    can::RX_IRQdisable();
    
    if(false == candata.rxQ->empty())
    {
        // i get a copy of the rx frame
        rxf = candata.rxQ->front();        
        candata.rxQ->erase(candata.rxQ->begin());
        remaining = candata.rxQ->size();  
        res = resOK;        
    }
    
    can::RX_IRQenable();

    frame = rxf;
          
    return res;         
}


result_t can::setfilters(embot::hw::CAN p, std::uint8_t address)
{
    if(false == supported(p))
    {
        return resNOK;
    } 


    CAN_FilterTypeDef sFilterConfig;    
    
    /* Configure the CAN Filter for message of class polling sensor */
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = (0x200 | address) << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = 0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.SlaveStartFilterBank = 0;

    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {
        return resNOK;
    }
        
    sFilterConfig.FilterIdHigh = 0x20F << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterBank = 1;
    sFilterConfig.SlaveStartFilterBank = 0;
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
    sFilterConfig.FilterBank = 2;
    sFilterConfig.SlaveStartFilterBank = 1;

    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {
        return resNOK;
    }
       
    sFilterConfig.FilterIdHigh = 0x70F << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterBank = 3;
    sFilterConfig.SlaveStartFilterBank = 1;
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
    sFilterConfig.FilterBank = 4;
    sFilterConfig.SlaveStartFilterBank = 2;

    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {
        return resNOK;
    }
        
    sFilterConfig.FilterIdHigh = 0x00F << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterBank = 5;
    sFilterConfig.SlaveStartFilterBank = 2;
    
    if(HAL_CAN_ConfigFilter(candata.handle, &sFilterConfig) != HAL_OK)
    {
        return resNOK;
    }
    
    return resOK;   
}
    


// - definition of helper functions which are not in the API

static void can::RX_IRQenable(void)
{
    HAL_CAN_ActivateNotification(candata.handle, CAN_IT_RX_FIFO0_MSG_PENDING);
}


static void can::RX_IRQdisable(void)
{
    HAL_CAN_DeactivateNotification(candata.handle, CAN_IT_RX_FIFO0_MSG_PENDING);      
}


static void can::tx_IRQenable()
{
    HAL_CAN_ActivateNotification(candata.handle, CAN_IT_TX_MAILBOX_EMPTY);
} 

static bool can::tx_IRQdisable()
{
    volatile bool enabled = ((candata.handle->Instance->IER & (CAN_IT_TX_MAILBOX_EMPTY)) == (CAN_IT_TX_MAILBOX_EMPTY));
    HAL_CAN_DeactivateNotification(candata.handle, CAN_IT_TX_MAILBOX_EMPTY);
    return enabled;    
}

static void can::tx_IRQresume(const bool previouslyenabled)
{
    if(true == previouslyenabled)
    {
        HAL_CAN_ActivateNotification(candata.handle, CAN_IT_TX_MAILBOX_EMPTY);
    }
} 

static void can::s_tx_start(CAN_HandleTypeDef *hcan)
{
    // transmit the first frame inside Qtx and then activate the interrupt (on fifo empty, hence ...) 
    
    CAN_TxHeaderTypeDef headertx = {0}; // KEEP IT IN STACK
    headertx.ExtId = 0;
    headertx.IDE = CAN_ID_STD;
    headertx.RTR = CAN_RTR_DATA;    

    // protect Qtx: i disable tx interrupt but i keep info if it was enabled, so that at the end i re-enable it
    volatile bool isTXenabled = tx_IRQdisable();

    if(true == candata.Qtx->empty())
    {
        // marco.accame: i dont have any data to tx, hence i return. 
        // but i call tx_IRQresume(isTXenabled) anyway because it does not cause any problem.

        // ok-code
        tx_IRQresume(isTXenabled);          
        return;        

#if 0   // test code: remove the ok-code above        
        // here is the reason:
        // 1. if isTXenabled is false, then it has no effect.
        // 2. if isTXenabled is true, then the re-enable does not trigger the call of
        //    s_tx_oneframehasgone() in here
        // 3. in any of teh previous two cases, the next time we transmit is when we call s_tx_start() with some data inside Qtx.
        // 4. the case of (isTXenabled == true) should not happen in here becase when s_tx_oneframehasgone() finds an empty
        //    Qtx it also calls tx_IRQdisable.
        // 5. however, it may happen: the test code under TEST000 does execute ciao++ in some cases of heavy CAN activity
        //    i think because tx_IRQdisable() computes the return value = true and then it is interrupted by the ISR which
        //    empties the Qtx. 
        // CONCLUSION: i dont think it is necessary to resume but i do it anyway. just because it does not cause any problem.
     
        if(isTXenabled)
        {
            static volatile int ciao = 0;
            for(;;) 
            {
                ciao ++;
            }
        }

        tx_IRQresume(isTXenabled);          
        return;
#endif        
    }
    else
    {    
        // else: i transmit the first frame myself and then i use the callback on interrupt
        // tx enable alone is not enough to trigger the callback, hence i must tx manually
        
        // 1. i get a copy of the first frame, I KEEP IT IN STACK SO THAT IT BELONGS TO THIS THREAD, and i remove it from the container
        Frame frame = candata.Qtx->front();
        candata.Qtx->erase(candata.Qtx->begin());
        // modify headertx
        headertx.StdId = frame.id & 0x7FF;
        headertx.DLC = frame.size;    
        // 2. i assign it to stm32
        uint32_t TxMailboxNum {0};
        

        // marco.accame: we transmit the first frame with HAL_CAN_AddTxMessage() and we enable the IRQ handler 
        // just after so that it is executed s_tx_oneframehasgone() which continues the job 
        // if i call tx_IRQenable() before HAL_CAN_AddTxMessage() i have verified that in some cases (heavy CAN traffic, 
        // possible thread delays or rescheduling, then the s_tx_oneframehasgone() is executed in between and that causes
        // a scrambled order of transmission. this situation does not depend on the value of isTXenabled.
        
        // ok-code
        HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);
        tx_IRQenable();

#if 0   // test code: remove ok-code above
        #define TEST_INVERSION
        if(false == isTXenabled)
        {
            // marco.accame: 
            // in order to guarantee the fifo order of transmission:
            // i MUST call HAL_CAN_AddTxMessage() before tx_IRQenable()
            // i have tested that if i call tx_IRQenable() before ... then in some cases it triggers s_tx_oneframehasgone()
            // before HAL_CAN_AddTxMessage() and that causes a scrambled tx order 
            #if !defined(TEST_INVERSION)
            HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);
            tx_IRQenable();
            #else
            // must define a global `volatile int checkit = 0;`, and block inside s_tx_oneframehasgone(): if(0 != checkit) for(;;);
            checkit = 1;
            tx_IRQenable();
            checkit = 2;
            embot::hw::sys::delay(2000); osal_task_wait(2000);
            checkit = 3;
            HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);        
            checkit = 0;           
            #endif
        }
        else
        {
            // marco.accame: 
            // in this case, i must change the order of operations and call HAL_CAN_AddTxMessage() before tx_IRQenable()
            // in order to guarantee the fifo order of transmission.
            // if i activate IRQ before tx this frame, it can happen (i have tested it!) that the s_tx_oneframehasgone()
            // is activated before this HAL_CAN_AddTxMessage() and that causes a scrambled tx order. 
            #if !defined(TEST_INVERSION)
            HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);
            tx_IRQenable(); 
            #else
            #warning JUST FOR TEST: DONT DO THAT
            tx_IRQenable();
            embot::hw::sys::delay(2000); osal_task_wait(2000);
            HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);
            #endif            
        }
#endif        

    }
}


static void can::s_tx_oneframehasgone(CAN_HandleTypeDef *hcan)
{
    // marco.accame: this funtion in here MUST be called only by the CAN IRQ handler.
    // if so, Qtx surely protected vs concurrent action because... the user-space caller 
    // always disables the TX ISR before manipulating Qtx.
    
        
    // surely we must execute the user-defined callback of a single tx frame in here
    candata.config.ontxframe.execute();
    
    if(true == candata.Qtx->empty())
    { 
        // #warning THINK: should we disable the tx interrupt? verify it!
        // yes, we should disable it. it is the funtion s_tx_start() which will re-enable it when it needs
        tx_IRQdisable(); 
        // surely we must execute the user-defined callback of the tx queue being empty 
        candata.config.txqueueempty.execute();
        return; 
    }
    else
    {       
        // i need to tx another frame.   
        Frame frame = candata.Qtx->front();
        candata.Qtx->erase(candata.Qtx->begin());
        CAN_TxHeaderTypeDef headertx = {0}; // KEEP IT IN STACK
        headertx.ExtId = 0;
        headertx.IDE = CAN_ID_STD;
        headertx.RTR = CAN_RTR_DATA; 
        headertx.StdId = frame.id & 0x7FF;
        headertx.DLC = frame.size;    
        // 2. i assign it to stm32
        uint32_t TxMailboxNum {0};    
         
        // marco.accame: as this code is alreeady executed by the CAN IRQ handler 
        // we can safely use standard burst activation (i enable IRQ and then i tx)
        tx_IRQenable(); 
        HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);  
    }
             
}


void can::callbackOnTXcompletion(CAN_HandleTypeDef* hcan)
{
    // this function is called inside IRQ handler of stm32hal, so hcan could be can1 or can2.
    // therefore i need to check that the interrupt is on the peritherical I already initted.    
    if( (hcan == (candata.handle)) && initialised(embot::hw::CAN::one) )
    {        
        s_tx_oneframehasgone(hcan);
    }
}
 

void can::callbackOnRXcompletion(CAN_HandleTypeDef* hcan)
{   
    if( (hcan == (candata.handle)) && initialised(embot::hw::CAN::one) )
    {
        // use stack variables
        Frame rxframe = {};       
        CAN_RxHeaderTypeDef RxMessage = {0};        
        HAL_CAN_GetRxMessage (hcan, CAN_RX_FIFO0, &RxMessage, rxframe.data);
        rxframe.id = RxMessage.StdId;
        rxframe.size = RxMessage.DLC;   
        
        // now we put the rxframe inside the rx circular fifo 
        if(candata.rxQ->size() >= candata.config.rxcapacity)
        {   // remove the oldest frame
            candata.rxQ->erase(candata.rxQ->begin());
        }
        candata.rxQ->push_back(rxframe);
        
        // and we execute teh callback on rx
        candata.config.onrxframe.execute();                
    }       
}
  
// not used, so far
//void can::callbackOnError(CAN_HandleTypeDef* hcan)
//{
//    hcan = hcan;
//    static uint32_t error_count = 0;
//    error_count++;
//}


// - stm32hal.lib needs some handlers being compiled in here: IRQ handlers and callbacks.
//   nothing is required 

#endif //defined(HAL_CAN_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

