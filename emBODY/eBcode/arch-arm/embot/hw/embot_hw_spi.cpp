
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_spi.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_spi_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"
//#include "embot_hw_gpio.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace embot::hw;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(HAL_SPI_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_spi)

namespace embot { namespace hw { namespace spi {

    bool supported(SPI b)
    { return false; }
    bool initialised(SPI b)
    { return false; }
    result_t init(SPI b, const Config &config)
    { return resNOK; }
    result_t deinit(SPI b)
    { return resNOK; }
    
    // blocking   
    bool isbusy(embot::hw::SPI b, embot::core::relTime timeout, embot::core::relTime &remaining) 
    { return false; }      
//    bool ping(embot::hw::SPI b, ADR adr, embot::core::relTime timeout) { return false; }   
    result_t read(embot::hw::SPI b, embot::core::Data &destination, embot::core::relTime timeout) 
    { return resNOK; } 
    result_t write(embot::hw::SPI b, const embot::core::Data &content, embot::core::relTime timeout) 
    { return resNOK; }     
//    result_t receive(embot::hw::SPI b, ADR adr, embot::core::Data &destination, embot::core::relTime timeout) { return resNOK; }  
//    result_t transmit(embot::hw::SPI b, ADR adr, const embot::core::Data &content, embot::core::relTime timeout) { return resNOK; } 
//    result_t transmitzeroaddress(embot::hw::SPI b, embot::core::relTime timeout) { return resNOK; }   
         
    // non blocking
//    bool isbusy(embot::hw::SPI b) { return false; }
//    result_t read(embot::hw::SPI b, ADR adr, Reg reg, embot::core::Data &destination, const embot::core::Callback &oncompletion) { return resNOK; }         
//    result_t write(embot::hw::SPI b, ADR adr, Reg reg, const embot::core::Data &content, const embot::core::Callback &oncompletion) { return resNOK; }     
//    result_t receive(embot::hw::SPI b, ADR adr, embot::core::Data &destination, const embot::core::Callback &oncompletion) { return resNOK; } 
//    result_t transmit(SPI b, ADR adr, const embot::core::Data &content, const embot::core::Callback &oncompletion) { return resNOK; }

}}} // namespace embot { namespace hw { namespace spi {

#else

namespace embot { namespace hw { namespace spi {
    
    // types
    struct Transaction
    {
        volatile bool ongoing {false};
        volatile bool txactive {false};
        volatile bool rxactive {false};
        static constexpr uint8_t txbuffercapacity = 16;
        embot::core::Data recdata {};
        std::uint8_t txbuffer[txbuffercapacity] = {0};
        std::uint8_t txbuffersize {0};
        embot::core::Callback oncompletion {};
        void clear() { ongoing = txactive = rxactive = false; recdata.clear(); oncompletion.clear(); std::memset(txbuffer, 0, sizeof(txbuffer)); txbuffersize = 0;}    
        Transaction () { clear(); }        
    };
        
    struct PrivateData
    {    
        Config config[embot::core::tointegral(SPI::maxnumberof)];  
        Transaction transaction[embot::core::tointegral(SPI::maxnumberof)]; 
        SPI_HandleTypeDef* handles[embot::core::tointegral(SPI::maxnumberof)];           
        PrivateData() { }
    };
    
    // utility functions
    
    
    
    
     
    static result_t s_wait_for_transaction_completion(SPI b, embot::core::relTime timeout);   
    static void s_SPI_TX_completed(SPI_HandleTypeDef *hspi);
    static void s_SPI_rx_completed(SPI_HandleTypeDef *hspi);
    static void s_SPI_error(SPI_HandleTypeDef *hspi);
    
//    static result_t s_IRQ_write_transaction_start(SPI b, const embot::core::Data &content, const embot::core::Callback &oncompletion);
    void ontransactionterminated(std::uint8_t index);  
    
    
    
    // internal variables
    PrivateData s_privatedata {};
              
    // initialised mask       
    std::uint32_t initialisedmask = 0;
    
    // the public functions
    
    bool supported(embot::hw::SPI p)
    {
        return embot::hw::spi::getBSP().supported(p);
    }

    bool initialised(embot::hw::SPI p)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(p));
    }    
        
    result_t init(SPI b, const Config &config)
    {
        if(false == supported(b))
        {
            return resNOK;
        }
        
        if(true == initialised(b))
        {
            return resOK;
        }  

        if(!embot::hw::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
        
        
        embot::hw::spi::getBSP().init(b);
        
        std::uint8_t index = embot::core::tointegral(b);
        s_privatedata.config[index] = config;
        s_privatedata.handles[index] = embot::hw::spi::getBSP().getPROP(b)->handle;
        
        // set callbacks on rx and tx and error
        HAL_SPI_RegisterCallback(s_privatedata.handles[index], HAL_SPI_TX_COMPLETE_CB_ID, s_SPI_TX_completed);
        HAL_SPI_RegisterCallback(s_privatedata.handles[index], HAL_SPI_RX_COMPLETE_CB_ID, s_SPI_rx_completed);
        HAL_SPI_RegisterCallback(s_privatedata.handles[index], HAL_SPI_ERROR_CB_ID, s_SPI_error);        
                       
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(b));
                
        return resOK;
    }
           
    result_t deinit(SPI b)
    {       
        if(false == initialised(b))
        {
            return resOK;
        }  
        
        std::uint8_t index = embot::core::tointegral(b);
        
        
        HAL_SPI_UnRegisterCallback(s_privatedata.handles[index], HAL_SPI_TX_COMPLETE_CB_ID);
        HAL_SPI_UnRegisterCallback(s_privatedata.handles[index], HAL_SPI_RX_COMPLETE_CB_ID);
        HAL_SPI_UnRegisterCallback(s_privatedata.handles[index], HAL_SPI_ERROR_CB_ID);
        
        s_privatedata.config[index] = {};
        s_privatedata.handles[index] = nullptr;
            
        embot::hw::spi::getBSP().deinit(b);

        embot::core::binary::bit::clear(initialisedmask, embot::core::tointegral(b));
                
        return resOK;
    }        
    
    // -
    // blocking mode

#if 0    
    result_t tx(embot::hw::SPI b, ADR adr, const embot::core::Data &content, embot::core::relTime timeout)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 
                
        embot::core::relTime remaining = timeout;       
        if(true == isbusy(b, timeout, remaining))
        {
            return resNOKtimeout;
        }
        
        // in here we just transmit over the bus w/ HAL_SPI_Master_Transmit() w/out DMA. the call is blocking w/ timeout
        // we must nevertheless set the bus busy before trasmission and set it free afterwards. 
        std::uint8_t index = embot::core::tointegral(b);             
        embot::hw::spi::s_privatedata.transaction[index].ongoing = true;       
        volatile HAL_StatusTypeDef r = HAL_SPI_Master_Transmit(s_privatedata.handles[index], static_cast<uint16_t>(adr), reinterpret_cast<uint8_t*>(content.pointer), static_cast<uint16_t>(content.capacity), timeout/1000);               
        embot::hw::spi::s_privatedata.transaction[index].ongoing = false;
        
        return (HAL_OK == r) ? resOK : resNOK;
    }        
    
#endif
    
    bool isbusy(SPI b, embot::core::relTime timeout, embot::core::relTime &remaining)
    {
        if(false == initialised(b))
        {
            return false;
        } 

        if(0 == timeout)
        {
            remaining = timeout;
            return s_privatedata.transaction[embot::core::tointegral(b)].ongoing;   
        }
              
        embot::core::Time deadline = embot::core::now() + timeout;
        
        bool res = true;
        for(;;)
        {
            volatile std::int64_t rem = deadline - embot::core::now();
            
            if(rem <= 0)
            {   
                remaining = 0;
                res = true;
                break;
            }
            else if(false == s_privatedata.transaction[embot::core::tointegral(b)].ongoing)
            {
                remaining = static_cast<embot::core::relTime>(rem);
                res = false;   
                break;                
            }
            
        }
        
        return res;        
    }   
        

    result_t read(SPI b, embot::core::Data &destination, embot::core::relTime timeout)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 
             
        if(false == destination.isvalid())
        {
            return resNOK;
        }
        
        embot::core::relTime remaining = timeout;       
        if(true == isbusy(b, timeout, remaining))
        {
            return resNOKtimeout;
        }
          
        result_t r {resOK};

        std::uint8_t index = embot::core::tointegral(b);        
        s_privatedata.transaction[index].ongoing = true;        
        // start reading
        HAL_StatusTypeDef rr = HAL_SPI_Receive_IT(s_privatedata.handles[index], reinterpret_cast<std::uint8_t*>(destination.pointer), destination.capacity);

        
        if(resOK == r)
        {   // it transaction has started, we must wait for its completion
            r = s_wait_for_transaction_completion(b, remaining);
        }
        
        return r;                      
    }    
    

    result_t write(SPI b, const embot::core::Data &content, embot::core::relTime timeout)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 

        if(false == content.isvalid())
        {
            return resNOK;
        }
        
        embot::core::relTime remaining = timeout;
        if(true == isbusy(b, timeout, remaining))
        {
            return resNOKtimeout;
        }        

        result_t r {resOK};           
        
        std::uint8_t index = embot::core::tointegral(b);        
        s_privatedata.transaction[index].ongoing = true;
                    
        // start writing
        HAL_StatusTypeDef rr = HAL_SPI_Transmit_IT(s_privatedata.handles[index], reinterpret_cast<std::uint8_t*>(content.pointer), content.capacity);

        if(HAL_OK != rr)
        {
           return resNOK; 
        }
        
        if(resOK == r)
        {   // it transaction has started, we must wait for its completion
            r = s_wait_for_transaction_completion(b, remaining);
        }
        
        return r;
    }
    
 
           
    
    // -
    // non-blocking mode
    
        
    bool isbusy(SPI b)
    {
        if(false == initialised(b))
        {
            return false;
        } 
        return s_privatedata.transaction[embot::core::tointegral(b)].ongoing;     
    }

 
    
    // -
    // utility functions
    
            
    static result_t s_wait_for_transaction_completion(SPI b, embot::core::relTime timeout)
    {
        embot::core::Time deadline = embot::core::now() + timeout;
        
        result_t res = resOK;
        for(;;)
        {
            if(false == isbusy(b))
            {
                break;
            }
            
            if(embot::core::now() > deadline)
            {
                res = resNOK;
                break;
            }
        } 

        return res;
    } 


//    static result_t s_IRQ_write_transaction_start(SPI b, const embot::core::Data &content, const embot::core::Callback &oncompletion)
//    { 
//        result_t res = resNOK; 
//        
//        std::uint8_t index = embot::core::tointegral(b);        
//        s_privatedata.transaction[index].oncompletion = oncompletion;
//        s_privatedata.transaction[index].ongoing = true;
//        s_privatedata.transaction[index].txactive = true;
//        s_privatedata.transaction[index].recdata.clear(); // invalide recdata, so that tx callback() knows it is a write operation.
//        
//        
//        HAL_StatusTypeDef r = HAL_SPI_Transmit_IT(s_privatedata.handles[index], reinterpret_cast<std::uint8_t*>(content.pointer), content.capacity);        
//        res = (HAL_OK == r) ? resOK : resNOK;
//        
// 
//#if 1
//    #warning SPI WIP
//#else 
//        if(Reg::addressNONE == reg.address)
//        {
//            // i need to transmit the pattern [data] because the slave already knows into which register to put data. and does not expect a [reg] byte.
//            uint16_t txsize = content.capacity;
//            if(txsize > Transaction::txbuffercapacity)
//            {
//                s_privatedata.transaction[index].ongoing = false;
//                return resNOK;
//            }
//            std::memmove(&s_privatedata.transaction[index].txbuffer[0], content.pointer, content.capacity);
//            s_privatedata.transaction[index].txbuffersize = content.capacity;            
//        }
//        else
//        {
//            // i need to transmit the pattern [reg.address][data] because the slave must know into which register to put data.            
//            uint16_t txsize = (Reg::Size::eightbits == reg.size) ? (content.capacity + 1) : (content.capacity + 2);
//            if(txsize > Transaction::txbuffercapacity)
//            {
//                s_privatedata.transaction[index].ongoing = false;
//                return resNOK;
//            }
//            
//            if(Reg::Size::eightbits == reg.size)
//            {
//                s_privatedata.transaction[index].txbuffer[0] = reg.address & 0xff;
//                std::memmove(&s_privatedata.transaction[index].txbuffer[1], content.pointer, content.capacity);
//                s_privatedata.transaction[index].txbuffersize = content.capacity + 1;                
//            }
//            else
//            {
//                s_privatedata.transaction[index].txbuffer[0] = (reg.address >> 8 ) & 0xff;
//                s_privatedata.transaction[index].txbuffer[1] = reg.address & 0xff;                
//                std::memmove(&s_privatedata.transaction[index].txbuffer[2], content.pointer, content.capacity);
//                s_privatedata.transaction[index].txbuffersize = content.capacity + 2;
//            }
//        }
//        
//        HAL_StatusTypeDef r = HAL_SPI_Master_Transmit_DMA(s_privatedata.handles[index], adr, static_cast<std::uint8_t*>(s_privatedata.transaction[index].txbuffer), s_privatedata.transaction[index].txbuffersize);        
//        res = (HAL_OK == r) ? resOK : resNOK;
//        
//#endif        
//        
//        if(resOK != res)
//        {
//            s_privatedata.transaction[index].ongoing = false;
//        }
//        
//        return res;
//    }            
        
//    static result_t s_DMA_write_transaction_start(SPI b, const embot::core::Data &content, const embot::core::Callback &oncompletion)
//    { 
//        result_t res = resNOK; 
//        
//        std::uint8_t index = embot::core::tointegral(b);        
////        s_privatedata.transaction[index].addr = adr;
//        s_privatedata.transaction[index].oncompletion = oncompletion;
//        s_privatedata.transaction[index].ongoing = true;
//        s_privatedata.transaction[index].recdata.clear(); // invalide recdata, so that HAL_SPI_MasterTxCpltCallback() knows it is a write operation.
// 
//#if 1
//    #warning SPI WIP
//#else 
//        if(Reg::addressNONE == reg.address)
//        {
//            // i need to transmit the pattern [data] because the slave already knows into which register to put data. and does not expect a [reg] byte.
//            uint16_t txsize = content.capacity;
//            if(txsize > Transaction::txbuffercapacity)
//            {
//                s_privatedata.transaction[index].ongoing = false;
//                return resNOK;
//            }
//            std::memmove(&s_privatedata.transaction[index].txbuffer[0], content.pointer, content.capacity);
//            s_privatedata.transaction[index].txbuffersize = content.capacity;            
//        }
//        else
//        {
//            // i need to transmit the pattern [reg.address][data] because the slave must know into which register to put data.            
//            uint16_t txsize = (Reg::Size::eightbits == reg.size) ? (content.capacity + 1) : (content.capacity + 2);
//            if(txsize > Transaction::txbuffercapacity)
//            {
//                s_privatedata.transaction[index].ongoing = false;
//                return resNOK;
//            }
//            
//            if(Reg::Size::eightbits == reg.size)
//            {
//                s_privatedata.transaction[index].txbuffer[0] = reg.address & 0xff;
//                std::memmove(&s_privatedata.transaction[index].txbuffer[1], content.pointer, content.capacity);
//                s_privatedata.transaction[index].txbuffersize = content.capacity + 1;                
//            }
//            else
//            {
//                s_privatedata.transaction[index].txbuffer[0] = (reg.address >> 8 ) & 0xff;
//                s_privatedata.transaction[index].txbuffer[1] = reg.address & 0xff;                
//                std::memmove(&s_privatedata.transaction[index].txbuffer[2], content.pointer, content.capacity);
//                s_privatedata.transaction[index].txbuffersize = content.capacity + 2;
//            }
//        }
//        
//        HAL_StatusTypeDef r = HAL_SPI_Master_Transmit_DMA(s_privatedata.handles[index], adr, static_cast<std::uint8_t*>(s_privatedata.transaction[index].txbuffer), s_privatedata.transaction[index].txbuffersize);        
//        res = (HAL_OK == r) ? resOK : resNOK;
//        
//#endif        
//        
//        if(resOK != res)
//        {
//            s_privatedata.transaction[index].ongoing = false;
//        }        
//        return res;
//    }        


//    static result_t s_DMA_read_transaction_start(SPI b, embot::core::Data &destination, const embot::core::Callback &oncompletion)
//    {
//        result_t res = resNOK;
//        
//        std::uint8_t index = embot::core::tointegral(b);
////        s_privatedata.transaction[index].addr = adr;
//        s_privatedata.transaction[index].oncompletion = oncompletion;
//        s_privatedata.transaction[index].ongoing = true;
//        s_privatedata.transaction[index].recdata = destination;  

//#if 1
//    #warning SPI WIP
//#else 
//    
//        if(reg.address == Reg::addressNONE)
//        {
//            // it is a single-stage operation: read<values>
//            // i dont need to ask which register to read from because the slave already knows what to send.
//            // i do the read<> request with HAL_SPI_Master_Receive_DMA(recdata).
//            // the completion of reading operation is signalled by the call of HAL_SPI_MasterRxCpltCallback(). 
//            // at this stage recdata will contain the received data.            
//            SPI_HandleTypeDef* hi2cx = embot::hw::spi::s_privatedata.handles[index];
//            HAL_StatusTypeDef r = HAL_SPI_Master_Receive_DMA( hi2cx, 
//                                        embot::hw::spi::s_privatedata.transaction[index].addr, 
//                                        static_cast<std::uint8_t*>(embot::hw::spi::s_privatedata.transaction[index].recdata.pointer),  
//                                        static_cast<std::uint16_t>(embot::hw::spi::s_privatedata.transaction[index].recdata.capacity)
//                                    );  
//            res = (HAL_OK == r) ? resOK : resNOK;                                        
//        }
//        else
//        {
//            // it is a two-stage operation: write<reg>, read<values>
//            // i must at first tx the value of the register i want to read from. i use HAL_SPI_Master_Transmit_DMA().
//            // the call of HAL_SPI_MasterTxCpltCallback() signals the end of teh fist stage and ... if recdata.isvalid() then ...
//            // i perform the read<> request with HAL_SPI_Master_Receive_DMA(recdata).
//            // the completion of reading operation is signalled by the call of HAL_SPI_MasterRxCpltCallback(). 
//            // at this stage recdata will contain the received data.
//                    
//            // so far we only manage addresses which are 1 or 2 bytes long ...
//            if(Reg::Size::eightbits == reg.size)
//            {
//                s_privatedata.transaction[index].txbuffer[0] = reg.address & 0xff;
//                s_privatedata.transaction[index].txbuffersize = 1;
//            }
//            else
//            {   // big endianess ....
//                s_privatedata.transaction[index].txbuffer[0] = (reg.address >> 8 ) & 0xff;
//                s_privatedata.transaction[index].txbuffer[1] = reg.address & 0xff;
//                s_privatedata.transaction[index].txbuffersize = 2;
//            }
//            HAL_StatusTypeDef r = HAL_SPI_Master_Transmit_DMA(s_privatedata.handles[index], adr, static_cast<std::uint8_t*>(s_privatedata.transaction[index].txbuffer), s_privatedata.transaction[index].txbuffersize);        
//            res = (HAL_OK == r) ? resOK : resNOK;
//        }
// 
//#endif 

//        if(resOK != res)
//        {
//            s_privatedata.transaction[index].ongoing = false;
//        }
//        return res;
//    } 
//    

    void ontransactionterminated(std::uint8_t index)
    {
        embot::hw::spi::s_privatedata.transaction[index].ongoing = false;        
        embot::hw::spi::s_privatedata.transaction[index].oncompletion.execute();                                
    }   


#if 1
    #warning SPI WIP
    static void s_SPI_TX_completed(SPI_HandleTypeDef *hspi)
    {
        embot::hw::SPI id = embot::hw::spi::getBSP().toID({hspi});
        if(embot::hw::SPI::none == id)
        {
            return;
        }
        
        std::uint8_t index = embot::core::tointegral(id);  
        embot::hw::spi::s_privatedata.transaction[index].ongoing = false;
        //embot::hw::spi::ontransactionterminated(index);
               
    }
    
    static void s_SPI_rx_completed(SPI_HandleTypeDef *hspi)
    {   
        embot::hw::SPI id = embot::hw::spi::getBSP().toID({hspi});
        if(embot::hw::SPI::none == id)
        {
            return;
        }
        
        std::uint8_t index = embot::core::tointegral(id);  
        embot::hw::spi::s_privatedata.transaction[index].ongoing = false;
       //embot::hw::spi::ontransactionterminated(index);    
    }
    
    static void s_SPI_error(SPI_HandleTypeDef *hspi)
    {
        
    }
    
    
#endif     
     
}}} // namespace embot { namespace hw { namespace spi {


// functions required by the hal of stm32. they are called by the hw at the completion of a tx or rx transaction

   


#endif //defined(HAL_SPI_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

