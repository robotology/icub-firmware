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

#include "embot_hw_usb.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core_binary.h"
#include "stm32hal.h"
//#include "stm32l4xx_hal_pcd.h"

#include "usbd_def.h"

#include "usbd_cdc_if.h" //per la funzione di trasmissione
#include "usb_device.h"


#include <cstring>
#include <vector>

using namespace std;
#define HAL_USB_MODULE_ENABLED 1
#define STM32HAL_BOARD_RFE     1


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

extern USBD_HandleTypeDef hUsbDeviceFS;



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#if     !defined(HAL_USB_MODULE_ENABLED)

// in here we manage the case of no can module being present in stm32hal

namespace embot { namespace hw { namespace usb {

    bool supported(Port p) { return false; }
    
    bool initialised(Port p) { return false; }
    
    result_t init(Port p, const Config &config) { return resNOK; }
    
    result_t enable(Port p)  { return resNOK; }
    
    result_t disable(Port p)  { return resNOK; }
    
//    result_t put(Port p, const Message &msg)  { return resNOK; }
//    
//    std::uint8_t outputqueuesize(Port p)  { return 0; }
//    
//    result_t transmit(Port p)  { return resNOK; }
    
    result_t transmitimmediately(Port P, const Message &msg) { return resNOK; }
    
    std::uint8_t inputqueuesize(Port p)  { return 0; }
    
    result_t get(Port p, Message &msg, std::uint8_t &remaining)  { return resNOK; }    
    
}}} // namespace embot { namespace hw { namespace can {

#elif   defined(HAL_USB_MODULE_ENABLED)



namespace embot { namespace hw { namespace usb {
    
    
    struct bspmap_t
    {
        std::uint32_t       mask;
    };
    

    #if   defined(STM32HAL_BOARD_RFE)
    
    static const bspmap_t bspmap = 
    {
        0x00000001
    };
    
    #else
        #error embot::hw::usb::bspmask must be filled    
    #endif
    
    //////////////////// static variables definitions //////////////////
      
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    static Config s_config;    
    static std::vector<Message> *s_rxQ;
    
    
    PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *)hUsbDeviceFS.pData;

    //////////////////// private function  //////////////////
    static std::uint8_t port2index(Port port);
    static void interrupt_RX_enable(void);
    static void interrupt_RX_disable(void);
    //void callbackOnRXcompletion(uint8_t* Buf, uint32_t *Len);
    static void s_transmit(Message &msg);


}}};


using namespace embot::hw;
using namespace embot::core::binary;


static std::uint8_t usb::port2index(Port port)
{   // use it only after verification of supported() ...
    return static_cast<uint8_t>(port);
}
        

static void usb::interrupt_RX_enable(void)
{
   __HAL_PCD_ENABLE(hpcd);
}
static void usb::interrupt_RX_disable(void)
{
   __HAL_PCD_DISABLE(hpcd);       
}

bool usb::supported(Port p)
{
    if((Port::none == p) || (Port::maxnumberof == p))
    {
        return false;
    }
    return bit::check(bspmap.mask, port2index(p));
}

bool usb::initialised(Port p)
{
    if(Port::none == p)
    {
        return false;
    }
    return bit::check(initialisedmask, port2index(p));
}    

static void usb::s_transmit(usb::Message &msg)
{
    //uint8_t *dataptr = (uint8_t*)msg.data;
    
    while(USBD_BUSY==CDC_Transmit_FS(msg.data, msg.size));
    return; // resOK;
}
    

    
void usb::callbackOnRXcompletion(uint8_t* Buf, uint32_t *Len)
{
    if(false == initialised(Port::one))
    {
        return;
    }
    
    uint32_t maxlen = (*Len > static_cast<std::uint32_t>(usb::MessageSize::max)) ? static_cast<std::uint32_t>(usb::MessageSize::max) : *Len;
    
    Message rxmsg;
    rxmsg.size=maxlen;
    memcpy(rxmsg.data, Buf, maxlen);
   
    if(s_rxQ->size() == s_config.rxcapacity)
    {
        //remove the oldest frame
        s_rxQ->erase(s_rxQ->begin());
    }
    s_rxQ->push_back(rxmsg);
    
    s_config.onrxmessage.execute();
    
}


    
result_t usb::init(Port p, const Config &config)
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
    
    // init peripheral
    MX_USB_DEVICE_Init();
    
    //init the rx buffer
    s_rxQ = new std::vector<Message>;
    s_rxQ->reserve(config.rxcapacity);
    
    embot::core::binary::bit::set(initialisedmask, port2index(p));

    return resOK;

}

result_t usb::enable(Port p)
{
    if(false == initialised(p))
    {
        return resNOK;
    }  
    //todo start??? check usb stack documentation
     return resOK;
}


result_t usb::disable(Port p)
{
    if(false == initialised(p))
    {
        return resNOK;
    }  

     //todo stop??? check usb stack documentation
    return resOK;                  
}    



std::uint8_t usb::inputqueuesize(Port p)
{
    if(false == initialised(p))
    {
        return 0;
    } 
    
    uint32_t size = 0;
    
    usb::interrupt_RX_disable();       
    
    size = s_rxQ->size();
    
    usb::interrupt_RX_enable();

    return size;
            
}



result_t usb::transmitimmediately(usb::Port p, usb::Message &msg)
{
    if(false == initialised(p))
    {
        return resNOK;
    } 

    if(Port::one == p)
    {
        usb::s_transmit(msg);
        return resOK;
    }
    else
    {
        return resNOK;
    }
}



result_t usb::get(usb::Port p, usb::Message &msg, std::uint8_t &remaining)
{
    if(false == initialised(p))
    {
        return resNOK;
    } 
    
    bool empty = true;
    usb::interrupt_RX_disable();
    
    empty = s_rxQ->empty();
    
    usb::interrupt_RX_enable();
   
    if(empty)
    {
        remaining = 0;
        return resNOK;
    }
    
   usb::interrupt_RX_disable();
    
    msg = s_rxQ->front();        
    s_rxQ->erase(s_rxQ->begin());
    remaining = s_rxQ->size();
    
    usb::interrupt_RX_enable();
          
    return resOK;         
}

//void USB_receiveMessageUserCallback(uint8_t* Buf, uint32_t *Len)
//{
//    embot::hw::usb::callbackOnRXcompletion(Buf, Len);
//}

extern "C" {
// it must be linked with a c file.
void USB_receiveMessageUserCallback(uint8_t* Buf, uint32_t *Len)
{embot::hw::usb::callbackOnRXcompletion(Buf, Len);}
}

#endif //defined(HAL_USB_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
