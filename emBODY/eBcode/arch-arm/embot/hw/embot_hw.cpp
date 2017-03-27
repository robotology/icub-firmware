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



namespace embot { namespace hw { namespace bsp {
    
    static bool initted = false; 

    bool initialised()
    {
        return initted;
    }

    result_t init(const Config &config)
    {
        if(true == embot::hw::bsp::initialised())
        {
            return resOK;
        }
        
        // put whatwever is required for ...
        
        stm32hal_config_t cfg;
        cfg.tick1ms_get = config.get1mstick;
        cfg.initbsp = true;
        
        stm32hal_init(&cfg);
        
        initted = true;
        return resOK;
    }
    

}}} // namespace embot { namespace hw { namespace bsp {




namespace embot { namespace hw { namespace led {

    struct bspmap_t
    {
        std::uint32_t       mask;
        GPIO_PinState       on;
        GPIO_PinState       off;
        embot::hw::GPIO     gpio[static_cast<uint8_t>(LED::maxnumberof)];
    };
    
    // const support maps
    #if     defined(STM32HAL_BOARD_NUCLEO64)    
        
    static const bspmap_t bspmap = 
    {
        0x00000001,
        GPIO_PIN_SET, GPIO_PIN_RESET,
        {
            {LD2_GPIO_Port, LD2_Pin},
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };
   

    #elif   defined(STM32HAL_BOARD_MTB4)
    
    static const bspmap_t bspmap = 
    {
        0x00000003,
        GPIO_PIN_RESET, GPIO_PIN_SET,
        {
            {LED_RED_GPIO_Port, LED_RED_Pin},
            {LED_BLUE_GPIO_Port, LED_BLUE_Pin},
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };

    #else
        #warning embot::hw::led::mask and embot::hw::led::gpiomap[] must be filled    
    #endif
      
    // initialised mask
        
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t led2index(LED led)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(led);
    }
        
    bool supported(LED led)
    {
        if((LED::none == led) || (LED::maxnumberof == led))
        {
            return false;
        }
        return embot::common::bit::check(bspmap.mask, led2index(led));
    }
    
    bool initialised(LED led)
    {
        if(LED::none == led)
        {
            return false;
        }
        return embot::common::bit::check(initialisedmask, led2index(led));
    }
        
    

    result_t init(LED led)
    {
        if(!supported(led))
        {
            return resNOK;
        }  
        
        if(initialised(led))
        {   // dont need to re-init
            return resOK;
        }
        
        // every led initalisation is done in the bsp ...
        
        if(!embot::hw::bsp::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
                
        embot::common::bit::set(initialisedmask, led2index(led));
        
        // we just switch it off        
        embot::hw::led::off(led);
        
        return resOK;        

    }

    
    result_t init_legacy(LED led, const void *par)
    {
        return init(led);
    }
    
    
    result_t on(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = &bspmap.gpio[led2index(led)];       
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), bspmap.on);
        return resOK;        
    }
    
    result_t off(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = &bspmap.gpio[led2index(led)];       
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), bspmap.off);
        return resOK;          
    }
    
    result_t toggle(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = &bspmap.gpio[led2index(led)];       
        HAL_GPIO_TogglePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin));
        return resOK;          
    }
    
    
}}} // namespace embot { namespace hw { namespace led 



namespace embot { namespace hw { namespace button {
    
    struct bspmap_t
    {
        std::uint32_t       mask;
        GPIO_PinState       pressed;
        embot::hw::GPIO     gpio[static_cast<uint8_t>(BTN::maxnumberof)];
    };

    // const support maps
    #if     defined(STM32HAL_BOARD_NUCLEO64)  

    static const bspmap_t bspmap = 
    {
        0x00000001,
        GPIO_PIN_RESET,
        {
            {B1_GPIO_Port, B1_Pin},
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };    

    #elif   defined(STM32HAL_BOARD_MTB4)
    
    static const bspmap_t bspmap = 
    {
        0x00000000,
        GPIO_PIN_RESET,
        {
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };
    
    #else
    
    static const bspmap_t bspmap = 
    {
        0x00000000,
        {
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    }; 
    
    #endif
      
    // initialised mask
        
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t btn2index(BTN btn)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(btn);
    }
        
    bool supported(BTN btn)
    {
        if((BTN::none == btn) || (BTN::maxnumberof == btn))
        {
            return false;
        }
        return embot::common::bit::check(bspmap.mask, btn2index(btn));
    }
    
    bool initialised(BTN btn)
    {
        if(BTN::none == btn)
        {
            return false;
        }
        return embot::common::bit::check(initialisedmask, btn2index(btn));
    }
        
    

    result_t init(BTN btn)
    {
        if(!supported(btn))
        {
            return resNOK;
        }  
        
        if(initialised(btn))
        {   // dont need to re-init
            return resOK;
        }
        
        // every button initalisation is done in the bsp ...
        
        if(!embot::hw::bsp::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
                
        embot::common::bit::set(initialisedmask, btn2index(btn));
                
        return resOK;        
    }

    
    
    
    bool pressed(BTN btn)
    {
        if(!initialised(btn))
        {
            return false;
        }  
        const embot::hw::GPIO *gpio = &bspmap.gpio[btn2index(btn)];       
        
        GPIO_PinState b1state = HAL_GPIO_ReadPin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin));    
        return (bspmap.pressed == b1state) ? (true) : (false);         
    }
    
    
}}} // namespace embot { namespace hw { namespace button 



namespace embot { namespace hw { namespace sys {
    
    
#if     defined(STM32HAL_BOARD_NUCLEO64)
    
    const std::uint32_t startOfFLASH            = 0x08000000;
    const std::uint32_t addressOfBootloader     = 0x08000000;
    const std::uint32_t maxsizeOfBootloader     = 124*1024;
    const std::uint32_t addressOfStorage        = 0x0801F000;
    const std::uint32_t maxsizeOfStorage        = 4*1024;
    const std::uint32_t addressOfApplication    = 0x08020000;
    const std::uint32_t maxsizeOfApplication    = 128*1024;
    
#elif   defined(STM32HAL_BOARD_MTB4)
    
    const std::uint32_t startOfFLASH            = 0x08000000;
    const std::uint32_t addressOfBootloader     = 0x08000000;
    const std::uint32_t maxsizeOfBootloader     = 124*1024;
    const std::uint32_t addressOfStorage        = 0x0801F000;
    const std::uint32_t maxsizeOfStorage        = 4*1024;
    const std::uint32_t addressOfApplication    = 0x08020000;
    const std::uint32_t maxsizeOfApplication    = 128*1024;
    
#endif

 
#if     defined(STM32HAL_BOARD_NUCLEO64) || defined(STM32HAL_BOARD_MTB4)
    
__asm static void ss_hl_sys_asm_xnumARMv7ops(uint32_t numberof) 
{
   align
dowaitloop
   subs r0,r0,#1
   bne dowaitloop
   bx lr 
   align    
}

    static void ss_bsp_delay(uint64_t t)
    {   
        static uint64_t s_hl_sys_numofops1sec = 0;
        static uint32_t s_hl_sys_used_systemcoreclock = 0;
        if(s_hl_sys_used_systemcoreclock != SystemCoreClock)
        {

            // to occupy a millisec i execute an operation for a number of times which depends on: 
            // SystemCoreClock, cortex gain(1.25 dmips/mhz), flash access, etc.
            // to overcome all this i just consider SystemCoreClock w/out 1.25 gain and i measures
            // extra gain with on a simple assembly function which should take 4 cycles per iteration (?).      
            //s_hl_sys_numofops1sec = (5*(SystemCoreClock)) / 4; 
            s_hl_sys_used_systemcoreclock = SystemCoreClock;
            s_hl_sys_numofops1sec = SystemCoreClock;
            
            // with art technology enabled the flash is seen as fast as the cpu. wow. 
            s_hl_sys_numofops1sec /= 3;             


            // at this point i normalise the variable to keep not the nymber of operations for 1 sec,
            // but for 1024*1024 microsec. by doing so, later on i shift by 20 instead of using a division. 
            s_hl_sys_numofops1sec <<= 20;
            s_hl_sys_numofops1sec /= 1000000;
        }
        
        
        volatile uint64_t num = s_hl_sys_numofops1sec * t;
        num >>= 20; 
        //num -= offset; //we may remove some cycles to compensates for previous instructions, but ... we dont do it. it depends on c compiler optimisation 
        if(0 == num)
        {
            return;
        }
        ss_hl_sys_asm_xnumARMv7ops((uint32_t)num);
    }
    
#endif
    
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
        ss_bsp_delay(t);
    }
    

}}} // namespace embot { namespace hw { namespace sys { 



namespace embot { namespace hw { namespace flash {
      
    
#if     defined(STM32HAL_BOARD_NUCLEO64)
    
    const std::uint32_t startOfFLASH            = 0x08000000;
    const std::uint32_t sizeOfFLASH             = 1024*1024;
    const std::uint32_t sizeOfPAGE              = 2*1024;
    const std::uint32_t maxNumOfPAGEs           = 512; 
    
#elif   defined(STM32HAL_BOARD_MTB4)

    const std::uint32_t startOfFLASH            = 0x08000000;
    const std::uint32_t sizeOfFLASH             = 256*1024;
    const std::uint32_t sizeOfPAGE              = 2*1024;
    const std::uint32_t maxNumOfPAGEs           = 128;     
    
#endif    
    
    bool isaddressvalid(std::uint32_t address)
    {
        if((address >= startOfFLASH) && (address < (startOfFLASH+sizeOfFLASH)))
        {
            return true;
        }
        return false;
    }
    
    std::uint32_t address2page(std::uint32_t address)
    {
        if(false == isaddressvalid(address))
        {
            return maxNumOfPAGEs;
        }
        
        return ((address - startOfFLASH) % sizeOfPAGE);
    }

    
    bool erase(std::uint32_t page)
    {
        if(page >= maxNumOfPAGEs)
        {
            return false;
        }
        
        FLASH_EraseInitTypeDef erase = {0};
        erase.TypeErase = FLASH_TYPEERASE_PAGES;
        erase.Banks = FLASH_BANK_1;
        erase.Page = page;
        erase.NbPages = 1;
        uint32_t pagenum = 0;
        HAL_FLASH_Unlock();
        HAL_StatusTypeDef r = HAL_FLASHEx_Erase(&erase, &pagenum);
        HAL_FLASH_Lock();
        return (HAL_OK == r) ? true : false;                
    }
    
    bool erase(std::uint32_t address, std::uint32_t size)
    {
        // we still erase by page. we need first page and number of them
        if(false == isaddressvalid(address))
        {
            return false;
        }
        if(false == isaddressvalid(address+size))
        {
            return false;
        }
        
        uint32_t firstpage = address2page(address);
        uint32_t lastpage = address2page(address+size-1);
        uint32_t npages = lastpage - firstpage + 1;
        
        for(uint32_t page=firstpage; page<npages; page++)
        {
            if(false == erase(page))
            {
                return false;
            }
        }
        
        return true;
    }
    
    
    bool read(std::uint32_t address, std::uint32_t size, void *data)
    {        
        if(false == isaddressvalid(address))
        {
            return false;
        }
        if(false == isaddressvalid(address+size))
        {
            return false;
        }
        
        if(NULL == data)
        {
            return false;
        }
        
        // can read directly from flash
        void *st = (void*) address;
        std::memmove(data, st, size); 
        
        return true;
    }
    
    bool write(std::uint32_t address, std::uint32_t size, const void *data)
    {
        if(false == isaddressvalid(address))
        {
            return false;
        }
        if(false == isaddressvalid(address+size))
        {
            return false;
        }
        
        if(NULL == data)
        {
            return false;
        }

        // address must be 8-aligned. size must be multiple of 8.
        // check it
        #warning TO BE DONE
        
        HAL_FLASH_Unlock();
    
        uint32_t tmpadr = address;
        uint32_t n64bitwords = sizeOfPAGE / 8;
        const uint64_t *buffer = reinterpret_cast<const std::uint64_t*>(data);
        for(uint32_t i=0; i<n64bitwords; i++)
        {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, tmpadr, buffer[i]);
            tmpadr += 8;
        }

        volatile uint32_t r =  HAL_FLASH_GetError();
        
        HAL_FLASH_Lock();
        
        return (HAL_FLASH_ERROR_NONE == r) ? true : false;
    }        
    
    
    
    
}}} // namespace embot { namespace hw { namespace flash {
    

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
        if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
        {
        
        }

        
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

        //2) remove the frame from tx queue
        vector<Frame>::iterator b = s_Qtx->begin();
        s_Qtx->erase(b);
        
        //3) transmit frame
        HAL_StatusTypeDef res = HAL_CAN_Transmit_IT(&hcan1);
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

