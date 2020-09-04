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

#include <cstring>
#include <vector>


using namespace std;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#if !defined(EMBOT_ENABLE_hw_can)

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
                         
    static std::uint32_t initialisedmask = 0;
    
    bool supported(embot::hw::CAN p)
    {
        return embot::hw::bsp::can::getBSP().supported(p);
    }

    bool initialised(embot::hw::CAN p)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(p));
    } 
    
    
    struct CANdata
    {
        Config config {};
        embot::hw::bsp::can::CAN_Handle *handle {nullptr};
        std::vector<Frame> *Qtx {nullptr};
        std::vector<Frame> *rxQ {nullptr};
        
        CANdata() = default;
    };
    
   
    std::array<CANdata, embot::core::tointegral(embot::hw::CAN::maxnumberof)> _candata_array {};
        
    static embot::hw::CAN toCAN(embot::hw::bsp::can::CAN_Handle *hcan)
    {
        // look inside _candata_array               
        for(uint8_t i=0; i<_candata_array.size(); i++)
        {
            if((nullptr != hcan) && (hcan == _candata_array[i].handle))
            {
                return static_cast<embot::hw::CAN>(i);
            }
        }
        
        return embot::hw::CAN::none;        
    }
    
    // - declaration of helper functions which are not in the API  
    static void s_tx_start(embot::hw::CAN p);
    static void s_tx_oneframehasgone(embot::hw::CAN p, embot::hw::bsp::can::CAN_Handle *hcan);  
    static void s_rx_oneframehascome(embot::hw::CAN p, embot::hw::bsp::can::CAN_Handle *hcan);      
    static bool s_registercallbacks(embot::hw::bsp::can::CAN_Handle *hcan);
    static bool s_startdriver(embot::hw::bsp::can::CAN_Handle *hcan);
    static bool s_filters_init(embot::hw::bsp::can::CAN_Handle *hcan);
    static result_t s_filters_set(embot::hw::bsp::can::CAN_Handle *hcan, std::uint8_t address);
    bool s_filters_set_by_class(embot::hw::bsp::can::CAN_Handle *hcan, uint8_t filternumber, uint8_t cls, uint8_t src, uint8_t dst);

    void callbackOnTXcompletion(embot::hw::bsp::can::CAN_Handle* hcan);
    void callbackOnRXcompletion(embot::hw::bsp::can::CAN_Handle* hcan);
    void callbackOnRXcompletionCANFD(embot::hw::bsp::can::CAN_Handle* hcan, uint32_t RxFifo0ITs);
    void callbackOnTXcompletionCANFD(embot::hw::bsp::can::CAN_Handle* hcan, uint32_t BufferIndexes);
    // void callbackOnError(embot::hw::bsp::can::CAN_Handle* hcan); // not used, so far
        
    static void RX_IRQenable(embot::hw::CAN p);
    static void RX_IRQdisable(embot::hw::CAN p);
        
    static void tx_IRQenable(embot::hw::CAN p);
    static bool tx_IRQdisable(embot::hw::CAN p); // returns status before disabling it
    static void tx_IRQresume(embot::hw::CAN p, const bool previouslyenabled); // previouslyenabled = tx_IRQdisable();
    
    constexpr uint8_t cls_mc_polling = 0;
    constexpr uint8_t cls_as_polling = 2;
    constexpr uint8_t cls_bootloader = 7;
    
    void s_addtxmessagetoqueue(embot::hw::bsp::can::CAN_Handle *hcan, Frame& frame);   
    
    void s_getrxmessagefromqueue(embot::hw::bsp::can::CAN_Handle *hcan, Frame& frame);
      
}}};


using namespace embot::hw;      

   
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
    
    std::uint8_t index = embot::core::tointegral(p);
    
    _candata_array[index].config = config;
    _candata_array[index].handle = embot::hw::bsp::can::getBSP().getPROP(p)->handle;
    
    // do whatever else is required .... for instance... init the buffers.
    
    _candata_array[index].Qtx = new std::vector<Frame>;
    _candata_array[index].rxQ = new std::vector<Frame>;
    _candata_array[index].Qtx->reserve(config.txcapacity);
    _candata_array[index].rxQ->reserve(config.rxcapacity);
              
    // init the filters
    s_filters_init(_candata_array[index].handle);
    
    // register the callbacks
    if(false == s_registercallbacks(_candata_array[index].handle))
    {
        return resNOK;
    }
    
    // start the driver
    if(false == s_startdriver(_candata_array[index].handle))
    {
        return resNOK;
    }    
        
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
    can::tx_IRQdisable(p);
    can::RX_IRQenable(p);
    return resOK;
}


result_t can::disable(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return resNOK;
    }  
    // i disable both TX and RX
    can::RX_IRQdisable(p);
    can::tx_IRQdisable(p);
    return resOK;                  
}    


result_t can::put(embot::hw::CAN p, const Frame &frame)
{
    if(false == initialised(p))
    {
        return resNOK;
    }  
    
    std::uint8_t index = embot::core::tointegral(p);
    
    // protect Qtx: i disable tx interrupt but i keep info if it was enabled, so that at the end i re-enable it
    volatile bool isTXenabled = tx_IRQdisable(p);
    
    // add the frame in the circular tx fifo
    if(_candata_array[index].Qtx->size() == _candata_array[index].config.txcapacity)
    {
        _candata_array[index].Qtx->erase(_candata_array[index].Qtx->begin());
    }    
    _candata_array[index].Qtx->push_back(frame);
    
    // re-enable the tx interrupt if it was previously enabled
    tx_IRQresume(p, isTXenabled);
    
    return resOK;                 
}   


std::uint8_t can::outputqueuesize(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return 0;
    } 
    
    std::uint8_t index = embot::core::tointegral(p);
    
    // protect Qtx: i disable tx interrupt but i keep info if it was enabled, so that at the end i re-enable it
    volatile bool isTXenabled = tx_IRQdisable(p);
    
    volatile size_t size = _candata_array[index].Qtx->size();
    
    tx_IRQresume(p, isTXenabled);
            
    return static_cast<uint8_t>(size);   
}


std::uint8_t can::inputqueuesize(embot::hw::CAN p)
{
    if(false == initialised(p))
    {
        return 0;
    } 
    
    std::uint8_t index = embot::core::tointegral(p);
        
    // protect rxQ: i disable rx interrupt and then i re-enable it
    can::RX_IRQdisable(p);           
    volatile size_t size = _candata_array[index].rxQ->size();    
    can::RX_IRQenable(p);

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
        can::s_tx_start(p);     
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
    Frame rxf {}; // this must stay on the stack and MUST NOT be a Frame & !!!!!!!
        
    std::uint8_t index = embot::core::tointegral(p);    
    
    // protect rxQ: i disable rx interrupt and then i re-enable it
    can::RX_IRQdisable(p);
    
    if(false == _candata_array[index].rxQ->empty())
    {
        // i get a copy of the rx frame. the copy is done because front() returns a reference. 
        // hence the reference is copied into rxf which is an object and NOT a reference itself.
        rxf = _candata_array[index].rxQ->front();        
        _candata_array[index].rxQ->erase(_candata_array[index].rxQ->begin());
        remaining = _candata_array[index].rxQ->size();  
        res = resOK;        
    }
    
    can::RX_IRQenable(p);

    frame = rxf;
          
    return res;         
}


result_t can::setfilters(embot::hw::CAN p, std::uint8_t address)
{
    if(false == supported(p))
    {
        return resNOK;
    } 
    
    std::uint8_t index = embot::core::tointegral(p);
    
    return s_filters_set(_candata_array[index].handle, address);
}
  

// - private part which does not depend on stm2hal


static void can::s_tx_start(embot::hw::CAN p)
{
    std::uint8_t index = embot::core::tointegral(p);    
    embot::hw::bsp::can::CAN_Handle *hcan = _candata_array[index].handle;
    
    // transmit the first frame inside Qtx and then activate the interrupt (on fifo empty, hence ...) 
     
    // protect Qtx: i disable tx interrupt but i keep info if it was enabled, so that at the end i re-enable it
    volatile bool isTXenabled = tx_IRQdisable(p);

    if(true == _candata_array[index].Qtx->empty())
    {
        // marco.accame: i dont have any data to tx, hence i return. 
        // but i call tx_IRQresume(isTXenabled) anyway because it does not cause any problem.

        // ok-code
        tx_IRQresume(p, isTXenabled);          
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
        Frame frame = _candata_array[index].Qtx->front();
        _candata_array[index].Qtx->erase(_candata_array[index].Qtx->begin());
        
        // marco.accame:  
        // we transmit the first frame with s_addtxmessagetoqueue() and we enable the IRQ handler 
        // just after so that it is executed s_tx_oneframehasgone() which continues the job 
        // if i call tx_IRQenable() before s_addtxmessagetoqueue() i have verified that in some cases (heavy CAN traffic, 
        // possible thread delays or rescheduling, then the s_tx_oneframehasgone() is executed in between and that causes
        // a scrambled order of transmission. this situation does not depend on the value of isTXenabled.
        
        s_addtxmessagetoqueue(hcan, frame);   
        tx_IRQenable(p);

//#if 0   // test code: remove ok-code above
//        #define TEST_INVERSION
//        if(false == isTXenabled)
//        {
//            // marco.accame: 
//            // in order to guarantee the fifo order of transmission:
//            // i MUST call HAL_CAN_AddTxMessage() before tx_IRQenable()
//            // i have tested that if i call tx_IRQenable() before ... then in some cases it triggers s_tx_oneframehasgone()
//            // before HAL_CAN_AddTxMessage() and that causes a scrambled tx order 
//            #if !defined(TEST_INVERSION)
//            HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);
//            tx_IRQenable();
//            #else
//            // must define a global `volatile int checkit = 0;`, and block inside s_tx_oneframehasgone(): if(0 != checkit) for(;;);
//            checkit = 1;
//            tx_IRQenable();
//            checkit = 2;
//            embot::hw::sys::delay(2000); osal_task_wait(2000);
//            checkit = 3;
//            HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);        
//            checkit = 0;           
//            #endif
//        }
//        else
//        {
//            // marco.accame: 
//            // in this case, i must change the order of operations and call HAL_CAN_AddTxMessage() before tx_IRQenable()
//            // in order to guarantee the fifo order of transmission.
//            // if i activate IRQ before tx this frame, it can happen (i have tested it!) that the s_tx_oneframehasgone()
//            // is activated before this HAL_CAN_AddTxMessage() and that causes a scrambled tx order. 
//            #if !defined(TEST_INVERSION)
//            HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);
//            tx_IRQenable(); 
//            #else
//            #warning JUST FOR TEST: DONT DO THAT
//            tx_IRQenable();
//            embot::hw::sys::delay(2000); osal_task_wait(2000);
//            HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);
//            #endif            
//        }
//        
//#endif        

    }
       
}

static void can::s_rx_oneframehascome(embot::hw::CAN p, embot::hw::bsp::can::CAN_Handle *hcan)
{
    std::uint8_t index = embot::core::tointegral(p);      
    
    // marco.accame: this funtion in here MUST be called only by the CAN IRQ handler.
    // if so, rxQ surely protected vs concurrent action because... the user-space caller 
    // always disables the TX ISR before manipulating Qtx.    
    
    // use stack variables ...
    Frame rxframe = {};  

    s_getrxmessagefromqueue(hcan, rxframe);        
    
//#if defined(HAL_CAN_MODULE_ENABLED)
//    
//    // use stack variables ...     
//    CAN_RxHeaderTypeDef RxMessage = {0};        
//    HAL_CAN_GetRxMessage (hcan, CAN_RX_FIFO0, &RxMessage, rxframe.data);
//    rxframe.id = RxMessage.StdId;
//    rxframe.size = RxMessage.DLC;   

//#elif defined(HAL_FDCAN_MODULE_ENABLED)  
//    #warning TODO: ...    
//#endif  
    
    // now we put the rxframe inside the rx circular fifo 
    if(_candata_array[index].rxQ->size() >= _candata_array[index].config.rxcapacity)
    {   // remove the oldest frame
        _candata_array[index].rxQ->erase(_candata_array[index].rxQ->begin());
    }
    _candata_array[index].rxQ->push_back(rxframe);
    
    // and we execute teh callback on rx
    _candata_array[index].config.onrxframe.execute();                
     
}


void can::callbackOnTXcompletion(embot::hw::bsp::can::CAN_Handle* hcan)
{
    // this function is called inside IRQ handler of stm32hal.     
    embot::hw::CAN p = toCAN(hcan); 
    
    // i need to check that the interrupt is on the peripheral I have initialised.    
    if( (embot::hw::CAN::none != p) && initialised(p) )
    {        
        s_tx_oneframehasgone(p, hcan);
    }
}

void can::callbackOnRXcompletion(embot::hw::bsp::can::CAN_Handle* hcan)
{   
    // this function is called inside IRQ handler of stm32hal.
    embot::hw::CAN p = toCAN(hcan); 
    
    // i need to check that the interrupt is on the peripheral I have initialised. 
    if( (embot::hw::CAN::none != p) && initialised(p) )
    {
        s_rx_oneframehascome(p, hcan);
    }

}

void can::callbackOnTXcompletionCANFD(embot::hw::bsp::can::CAN_Handle* hcan, uint32_t BufferIndexes)
{
    // this function is called inside IRQ handler of stm32hal.     
    embot::hw::CAN p = toCAN(hcan); 
    
    // i need to check that the interrupt is on the peripheral I have initialised.    
    if( (embot::hw::CAN::none != p) && initialised(p) )
    {        
        s_tx_oneframehasgone(p, hcan);
    }
}

void can::callbackOnRXcompletionCANFD(embot::hw::bsp::can::CAN_Handle* hcan, uint32_t RxFifo0ITs)
{   
    // this function is called inside IRQ handler of stm32hal.
    embot::hw::CAN p = toCAN(hcan); 
    
    // i need to check that the interrupt is on the peripheral I have initialised. 
    if( (embot::hw::CAN::none != p) && initialised(p) )
    {
        s_rx_oneframehascome(p, hcan);
    }

}


static void can::s_tx_oneframehasgone(embot::hw::CAN p, embot::hw::bsp::can::CAN_Handle *hcan)
{
    std::uint8_t index = embot::core::tointegral(p);
    
    // marco.accame: this funtion in here MUST be called only by the CAN IRQ handler.
    // if so, Qtx surely protected vs concurrent action because... the user-space caller 
    // always disables the TX ISR before manipulating Qtx.
    
            
    // surely we must execute the user-defined callback of a single tx frame in here
    _candata_array[index].config.ontxframe.execute();
    
    if(true == _candata_array[index].Qtx->empty())
    { 
        // #warning THINK: should we disable the tx interrupt? verify it!
        // yes, we should disable it. it is the funtion s_tx_start() which will re-enable it when it needs
        tx_IRQdisable(p); 
        // surely we must execute the user-defined callback of the tx queue being empty 
        _candata_array[index].config.txqueueempty.execute();
        return; 
    }
    else
    {       
        // i need to tx another frame.   
        Frame frame = _candata_array[index].Qtx->front();
        _candata_array[index].Qtx->erase(_candata_array[index].Qtx->begin());

        // marco.accame: as this code is alreeady executed by the CAN IRQ handler 
        // we can safely use standard burst activation (i enable IRQ and then i tx)
        tx_IRQenable(p);         
        s_addtxmessagetoqueue(hcan, frame);
        
//#if defined(HAL_CAN_MODULE_ENABLED)
//        
//        CAN_TxHeaderTypeDef headertx = {0}; // KEEP IT IN STACK
//        headertx.ExtId = 0;
//        headertx.IDE = CAN_ID_STD;
//        headertx.RTR = CAN_RTR_DATA; 
//        headertx.StdId = frame.id & 0x7FF;
//        headertx.DLC = frame.size;    
//        // 2. i assign it to stm32
//        uint32_t TxMailboxNum {0};    
//         
//        // marco.accame: as this code is alreeady executed by the CAN IRQ handler 
//        // we can safely use standard burst activation (i enable IRQ and then i tx)
//        tx_IRQenable(p); 
//        HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);  
//#elif defined(HAL_FDCAN_MODULE_ENABLED)  
//    #warning TODO: ...    
//#endif 
        
    }
        
}

// - private part which depends strictly on stm32 hal

// we need to manage two cases: one with HAL_CAN_MODULE_ENABLED and one with HAL_FDCAN_MODULE_ENABLED

#if defined(HAL_CAN_MODULE_ENABLED) && defined(STM32HAL_STM32L4) && (STM32HAL_DRIVER_VERSION < 0x183)
    #define STM32HAL_HAS_CAN_API_PRE_V183
    #warning using can api pre hal driver v183 ... think of updating the stm32hal
    #error UPDATE the application to use a newer version of stm32 lib
#endif


// - definition of helper functions which are not in the API

static void can::RX_IRQenable(embot::hw::CAN p)
{   
    std::uint8_t index = embot::core::tointegral(p);
#if defined(HAL_CAN_MODULE_ENABLED)
    HAL_CAN_ActivateNotification(_candata_array[index].handle, CAN_IT_RX_FIFO0_MSG_PENDING);
#elif defined(HAL_FDCAN_MODULE_ENABLED)  
    #warning TODO: ... check it
    HAL_FDCAN_ActivateNotification(_candata_array[index].handle, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
#endif
}


static void can::RX_IRQdisable(embot::hw::CAN p)
{
    std::uint8_t index = embot::core::tointegral(p);
#if defined(HAL_CAN_MODULE_ENABLED)
    HAL_CAN_DeactivateNotification(_candata_array[index].handle, CAN_IT_RX_FIFO0_MSG_PENDING);
#elif defined(HAL_FDCAN_MODULE_ENABLED)  
    #warning TODO: ... check it   
    HAL_FDCAN_DeactivateNotification(_candata_array[index].handle, FDCAN_IT_RX_FIFO0_NEW_MESSAGE); 
#endif        
}


static void can::tx_IRQenable(embot::hw::CAN p)
{
    std::uint8_t index = embot::core::tointegral(p);
#if defined(HAL_CAN_MODULE_ENABLED)
    HAL_CAN_ActivateNotification(_candata_array[index].handle, CAN_IT_TX_MAILBOX_EMPTY);
#elif defined(HAL_FDCAN_MODULE_ENABLED)  
    #warning TODO: ... check it
    HAL_FDCAN_ActivateNotification(_candata_array[index].handle, FDCAN_IT_TX_FIFO_EMPTY, 0);
    // comments / doubts
    // 1. we use FDCAN_IT_TX_COMPLETE or FDCAN_IT_TX_FIFO_EMPTY?   
    // 2. if dont use FDCAN_IT_TX_COMPLETE, we can remove the FDCAN_TX_BUFFER0 because st says: `This parameter is ignored if ActiveITs does not include one of the following: ...`
         
#endif
    
} 

static bool can::tx_IRQdisable(embot::hw::CAN p)
{
    std::uint8_t index = embot::core::tointegral(p);
    volatile bool enabled = true;
#if defined(HAL_CAN_MODULE_ENABLED)
    enabled = ((_candata_array[index].handle->Instance->IER & (CAN_IT_TX_MAILBOX_EMPTY)) == (CAN_IT_TX_MAILBOX_EMPTY));
    HAL_CAN_DeactivateNotification(_candata_array[index].handle, CAN_IT_TX_MAILBOX_EMPTY);
#elif defined(HAL_FDCAN_MODULE_ENABLED) 
    #warning TODO: ... check it
    enabled = ((_candata_array[index].handle->Instance->IE & (FDCAN_IT_TX_FIFO_EMPTY)) == (FDCAN_IT_TX_FIFO_EMPTY));  
    HAL_FDCAN_DeactivateNotification(_candata_array[index].handle, FDCAN_IT_TX_FIFO_EMPTY);   
#endif    
    return enabled;    
}

static void can::tx_IRQresume(embot::hw::CAN p, const bool previouslyenabled)
{
    std::uint8_t index = embot::core::tointegral(p);
    if(true == previouslyenabled)
    {
#if defined(HAL_CAN_MODULE_ENABLED)        
        HAL_CAN_ActivateNotification(_candata_array[index].handle, CAN_IT_TX_MAILBOX_EMPTY);
#elif defined(HAL_FDCAN_MODULE_ENABLED)  
        #warning TODO: ... check it
        HAL_FDCAN_ActivateNotification(_candata_array[index].handle, FDCAN_IT_TX_FIFO_EMPTY, 0); // or FDCAN_IT_TX_FIFO_EMPTY?   
#endif         
    }
} 

static bool can::s_filters_init(embot::hw::bsp::can::CAN_Handle *hcan)
{
#if defined(HAL_CAN_MODULE_ENABLED)
    
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
    sFilterConfig.SlaveStartFilterBank = 14; // For single CAN instances, this parameter is meaningless

    if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
    {  
        return false;    
    } 
    
    return true;
    
#elif defined(HAL_FDCAN_MODULE_ENABLED)  

    FDCAN_FilterTypeDef sFilterConfig {};     
    sFilterConfig.IdType = FDCAN_STANDARD_ID;
    sFilterConfig.FilterIndex = 0;  
    sFilterConfig.FilterType = FDCAN_FILTER_MASK; // Classic filter: FilterID1 = filter, FilterID2 = mask
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // Store in Rx FIFO 0 if filter matches
    sFilterConfig.FilterID1 = 0; // must be a number between: 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
    sFilterConfig.FilterID2 = 0; // must be a number between: 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
       
    if(HAL_FDCAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
    {
        return false;
    }
    
    return true;
    
#endif    
}

static result_t can::s_filters_set(embot::hw::bsp::can::CAN_Handle *hcan, std::uint8_t address)
{

#if defined(HAL_CAN_MODULE_ENABLED)
    
//    CAN_FilterTypeDef sFilterConfig;    
//    
//    /* Configure the CAN Filter for message of class as polling */
//    sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
//    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
//    sFilterConfig.FilterIdHigh = (0x200 | address) << 5;
//    sFilterConfig.FilterIdLow = 0x0000;
//    sFilterConfig.FilterMaskIdHigh = 0x0000;
//    sFilterConfig.FilterMaskIdLow = 0x0000;
//    sFilterConfig.FilterFIFOAssignment = 0;
//    sFilterConfig.FilterActivation = ENABLE;
//    sFilterConfig.FilterBank = 0;
//    sFilterConfig.SlaveStartFilterBank = 0; // For single CAN instances, this parameter is meaningless

//    if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
//    {
//        return resNOK;
//    }
//        
//    sFilterConfig.FilterIdHigh = 0x20F << 5;
//    sFilterConfig.FilterIdLow = 0x0000;
//    sFilterConfig.FilterMaskIdHigh = 0x0000;
//    sFilterConfig.FilterMaskIdLow = 0x0000;
//    sFilterConfig.FilterActivation = ENABLE;
//    sFilterConfig.FilterBank = 1;
//    sFilterConfig.SlaveStartFilterBank = 0; // For single CAN instances, this parameter is meaningless
//    if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
//    {
//        return resNOK;
//    }
//    
//     /* Configure the CAN Filter for message of class bootloader */
//    
//    sFilterConfig.FilterIdHigh = (0x700 | address) << 5;
//    sFilterConfig.FilterIdLow = 0x0000;
//    sFilterConfig.FilterMaskIdHigh = 0x0000;
//    sFilterConfig.FilterMaskIdLow = 0x0000;
//    sFilterConfig.FilterActivation = ENABLE;
//    sFilterConfig.FilterBank = 2;
//    sFilterConfig.SlaveStartFilterBank = 1; // For single CAN instances, this parameter is meaningless

//    if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
//    {
//        return resNOK;
//    }
//       
//    sFilterConfig.FilterIdHigh = 0x70F << 5;
//    sFilterConfig.FilterIdLow = 0x0000;
//    sFilterConfig.FilterMaskIdHigh = 0x0000;
//    sFilterConfig.FilterMaskIdLow = 0x0000;
//    sFilterConfig.FilterActivation = ENABLE;
//    sFilterConfig.FilterBank = 3;
//    sFilterConfig.SlaveStartFilterBank = 1; // For single CAN instances, this parameter is meaningless
//    if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
//    {
//        return resNOK;
//    }

//    /* Configure the CAN Filter for message of class mc polling */
//    sFilterConfig.FilterIdHigh = (0x000 | address) << 5;
//    sFilterConfig.FilterIdLow = 0x0000;
//    sFilterConfig.FilterMaskIdHigh = 0x0000;
//    sFilterConfig.FilterMaskIdLow = 0x0000;
//    sFilterConfig.FilterActivation = ENABLE;
//    sFilterConfig.FilterBank = 4;
//    sFilterConfig.SlaveStartFilterBank = 2; // For single CAN instances, this parameter is meaningless

//    if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
//    {
//        return resNOK;
//    }
//        
//    sFilterConfig.FilterIdHigh = 0x00F << 5;
//    sFilterConfig.FilterIdLow = 0x0000;
//    sFilterConfig.FilterMaskIdHigh = 0x0000;
//    sFilterConfig.FilterMaskIdLow = 0x0000;
//    sFilterConfig.FilterActivation = ENABLE;
//    sFilterConfig.FilterBank = 5;
//    sFilterConfig.SlaveStartFilterBank = 2; // For single CAN instances, this parameter is meaningless
//    
//    if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
//    {
//        return resNOK;
//    }

    // for CAN driver we need two filters per class
    can::s_filters_set_by_class(hcan, 0, cls_bootloader, 0, address);
    can::s_filters_set_by_class(hcan, 2, cls_as_polling, 0, address); 
    can::s_filters_set_by_class(hcan, 4, cls_mc_polling, 0, address); 
    
#elif defined(HAL_FDCAN_MODULE_ENABLED)    

    #if 0
    for address = z i need to discard every id and keep only those which ends with 
    - 0x------0F    broadcast sent by host w/ id 0
    - 0x------0z    unicast send by host with id 0
    if i want to discard even more, i can accept only those messages belonging to a given class s such as
    mc-polling (s = 0), as-polling (s= 2), bootloader (s = 7)
    - 0x-----c0.
    
    hence i keep only:
    - 0x0000000F [ mc polling broadcast]
    - 0x0000000z [ mc polling unicast]
    - 0x0000020F [ as polling broadcast]
    - 0x0000020z [ as polling unicast] 
    - 0x0000070F [ bootloader broadcast]
    - 0x0000070z [ bootloader unicast]  

    if i use `Filter for dedicated IDs` i can use three rules with ...
    #endif 
    
    can::s_filters_set_by_class(hcan, 0, cls_bootloader, 0, address);
    can::s_filters_set_by_class(hcan, 1, cls_as_polling, 0, address); 
    can::s_filters_set_by_class(hcan, 2, cls_mc_polling, 0, address);    
        
#endif
    
    return resOK;   
}


bool can::s_filters_set_by_class(embot::hw::bsp::can::CAN_Handle *hcan, uint8_t filternumber, uint8_t cls, uint8_t src, uint8_t dst)
{
    
#if defined(HAL_CAN_MODULE_ENABLED)
    
    CAN_FilterTypeDef sFilterConfig {};

    sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    // marco.accame on 4 sept 2020: i dont understand why it is shifted by 5 ... but for now i keep it as it s. i will investigate it later
    //sFilterConfig.FilterIdHigh = (0x200 | address) << 5;
    sFilterConfig.FilterIdHigh = ((static_cast<uint32_t>(cls)<<8) | (static_cast<uint32_t>(src)<<4) | (dst&0xf)) << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = 0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterBank = filternumber;
    sFilterConfig.SlaveStartFilterBank = 0; // For single CAN instances, this parameter is meaningless

    if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
    {
        return false;
    }
        
    // marco.accame on 4 sept 2020: i dont understand why it is shifted by 5 ... but for now i keep it as it s. i will investigate it later    
    //sFilterConfig.FilterIdHigh = 0x20F << 5;
    sFilterConfig.FilterIdHigh = ((static_cast<uint32_t>(cls)<<8) | (static_cast<uint32_t>(src)<<4) | 0x00F) << 5;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterBank = filternumber+1;
    sFilterConfig.SlaveStartFilterBank = 0; // For single CAN instances, this parameter is meaningless
    if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
    {
        return false;
    }  
    
    return true;
    
#elif defined(HAL_FDCAN_MODULE_ENABLED)

    FDCAN_FilterTypeDef sFilterConfig {};     
    sFilterConfig.IdType = FDCAN_STANDARD_ID;
    sFilterConfig.FilterIndex = filternumber;  
    sFilterConfig.FilterType = FDCAN_FILTER_DUAL; // Dual ID filter for FilterID1 or FilterID2
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // Store in Rx FIFO 0 if filter matches
    sFilterConfig.FilterID1 = (static_cast<uint32_t>(cls)<<8) | (static_cast<uint32_t>(src)<<4) | 0x00F; // must be a number between: 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
    sFilterConfig.FilterID2 = (static_cast<uint32_t>(cls)<<8) | (static_cast<uint32_t>(src)<<4) | (dst&0xf); // must be a number between: 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID

       
    if(HAL_FDCAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
    {
        return false;
    }
    
    return true;   
    
#endif 
}


static bool can::s_registercallbacks(embot::hw::bsp::can::CAN_Handle *hcan)
{  

#if defined(HAL_CAN_MODULE_ENABLED)    
    
    if(HAL_OK != HAL_CAN_RegisterCallback(hcan, HAL_CAN_TX_MAILBOX0_COMPLETE_CB_ID, can::callbackOnTXcompletion))
        return false; //TODO: adding clear of vector
    if(HAL_OK != HAL_CAN_RegisterCallback(hcan, HAL_CAN_TX_MAILBOX1_COMPLETE_CB_ID, can::callbackOnTXcompletion))
        return false; //TODO: adding clear of vector
    if(HAL_OK != HAL_CAN_RegisterCallback(hcan, HAL_CAN_TX_MAILBOX2_COMPLETE_CB_ID, can::callbackOnTXcompletion))
        return false; //TODO: adding clear of vector
    if(HAL_OK != HAL_CAN_RegisterCallback(hcan, HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID, can::callbackOnRXcompletion))
        return false; //TODO: adding clear of vector    

#elif defined(HAL_FDCAN_MODULE_ENABLED)   
    #warning TODO: verify s_registercallbacks()   
    
    HAL_FDCAN_RegisterTxBufferCompleteCallback(hcan, can::callbackOnTXcompletionCANFD); 
    HAL_FDCAN_RegisterRxFifo0Callback(hcan, can::callbackOnRXcompletionCANFD);
    
#endif 
    
    return true;
}    


static bool can::s_startdriver(embot::hw::bsp::can::CAN_Handle *hcan)
{  

#if defined(HAL_CAN_MODULE_ENABLED)    
        
    if(HAL_OK != HAL_CAN_Start(hcan))
    {
        return false; 
    }

#elif defined(HAL_FDCAN_MODULE_ENABLED)   

    if(HAL_OK != HAL_FDCAN_Start(hcan))
    {
        return false;
    }
    
#endif 
    
    return true;
}  



void can::s_addtxmessagetoqueue(embot::hw::bsp::can::CAN_Handle *hcan, Frame& frame)
{

#if defined(HAL_CAN_MODULE_ENABLED)         
        // 2. i prepare what stm requires for its tx
        CAN_TxHeaderTypeDef headertx = {0}; // KEEP IT IN STACK
        headertx.ExtId = 0;
        headertx.IDE = CAN_ID_STD;
        headertx.RTR = CAN_RTR_DATA;    
        headertx.StdId = frame.id & 0x7FF;
        headertx.DLC = frame.size;    
        uint32_t TxMailboxNum {0};                
        // ok-code
        HAL_CAN_AddTxMessage(hcan, &headertx, frame.data, &TxMailboxNum);

#elif defined(HAL_FDCAN_MODULE_ENABLED)  
    
        #warning TODO: test it

        FDCAN_TxHeaderTypeDef headertx = {0}; // KEEP IT IN STACK
        headertx.Identifier = frame.id & 0x7FF;
        headertx.IdType = FDCAN_STANDARD_ID;
        headertx.TxFrameType = FDCAN_DATA_FRAME;
        headertx.DataLength = frame.size;
        headertx.ErrorStateIndicator = FDCAN_ESI_ACTIVE; // or FDCAN_ESI_PASSIVE ???
        headertx.BitRateSwitch = FDCAN_BRS_OFF;
        headertx.FDFormat = FDCAN_CLASSIC_CAN;
        headertx.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // or FDCAN_STORE_TX_EVENTS ??
        headertx.MessageMarker = 0; //  Specifies the message marker to be copied into Tx Event FIFO ... between 0 and 0xFF
        HAL_FDCAN_AddMessageToTxFifoQ(hcan, &headertx, frame.data);
#endif 
        
}

void can::s_getrxmessagefromqueue(embot::hw::bsp::can::CAN_Handle *hcan, Frame& frame)
{
        
#if defined(HAL_CAN_MODULE_ENABLED)    
    // use stack variables ...     
    CAN_RxHeaderTypeDef headerRX = {0};        
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &headerRX, frame.data);
    frame.id = headerRX.StdId;
    frame.size = headerRX.DLC;   

#elif defined(HAL_FDCAN_MODULE_ENABLED)  
    #warning TODO: verify it
    FDCAN_RxHeaderTypeDef headerRX = {0}; // KEEP IT IN STACK   
    HAL_FDCAN_GetRxMessage(hcan, FDCAN_RX_FIFO0, &headerRX, frame.data);
    frame.id = headerRX.Identifier & 0x7ff;
    frame.size = headerRX.DataLength;       
#endif  
    
}
 
// not used, so far
//void can::callbackOnError(embot::hw::bsp::can::CAN_Handle* hcan)
//{
//    hcan = hcan;
//    static uint32_t error_count = 0;
//    error_count++;
//}


// - stm32hal.lib needs some handlers being compiled in here: IRQ handlers and callbacks.
//   nothing is required 

#endif // EMBOT_ENABLE_hw_can


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

