
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
     
    bool isbusy(embot::hw::SPI b, embot::core::relTime timeout, embot::core::relTime &remaining) 
    { return false; }  
    // blocking      
    result_t read(embot::hw::SPI b, embot::core::Data &destination, embot::core::relTime timeout) 
    { return resNOK; } 
    result_t write(embot::hw::SPI b, const embot::core::Data &source, embot::core::relTime timeout) 
    { return resNOK; }  
    // non blocking
    result_t read(embot::hw::SPI b, embot::core::Data &destination, const embot::core::Callback &oncompletion)
    { return resNOK; }
    result_t write(embot::hw::SPI b, const embot::core::Data &source, const embot::core::Callback &oncompletion)
    { return resNOK; }    

}}} // namespace embot { namespace hw { namespace spi {

#else

namespace embot { namespace hw { namespace spi {
    
    struct Transaction
    {
        enum class Direction : uint8_t { NONE, TX, RX, TXRX };
        Direction direction {Direction::NONE};
        volatile bool ongoing {false};
        embot::core::Callback oncompletion {};    
        Transaction() = default;
        void clear() { direction = Direction::NONE; oncompletion.clear(); ongoing = false; } //recdata.clear(); data2send.clear(); }
        void start(Direction d, const embot::core::Callback &onc) { ongoing = true; direction = d; oncompletion = onc; }
        void stop(Direction d) { if(d == direction) { oncompletion.execute(); clear(); } }         
    };
        
    struct PrivateData
    {    
        Config config[embot::core::tointegral(SPI::maxnumberof)] {};  
        Transaction transaction[embot::core::tointegral(SPI::maxnumberof)] {}; 
        SPI_HandleTypeDef* handles[embot::core::tointegral(SPI::maxnumberof)] {};           
        PrivateData() = default;
    };
    
    // utility functions     
    result_t s_wait_for_transaction_completion(SPI b, embot::core::relTime timeout);   
    void s_SPI_TX_completed(SPI_HandleTypeDef *hspi);
    void s_SPI_rx_completed(SPI_HandleTypeDef *hspi);
    void s_SPI_TXrx_completed(SPI_HandleTypeDef *hspi);
    void s_SPI_error(SPI_HandleTypeDef *hspi);    
       
    
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
        
        
        bool bspinit = embot::hw::spi::getBSP().init(b, config);
        if(false == bspinit)
        {
            // do init in here
        }
        
        std::uint8_t index = embot::core::tointegral(b);
        s_privatedata.config[index] = config;
        s_privatedata.handles[index] = embot::hw::spi::getBSP().getPROP(b)->handle;
        
        // set callbacks on rx and tx and error
        HAL_SPI_RegisterCallback(s_privatedata.handles[index], HAL_SPI_TX_COMPLETE_CB_ID, s_SPI_TX_completed);
        HAL_SPI_RegisterCallback(s_privatedata.handles[index], HAL_SPI_RX_COMPLETE_CB_ID, s_SPI_rx_completed);
        HAL_SPI_RegisterCallback(s_privatedata.handles[index], HAL_SPI_TX_RX_COMPLETE_CB_ID, s_SPI_TXrx_completed);
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
        HAL_SPI_UnRegisterCallback(s_privatedata.handles[index], HAL_SPI_TX_RX_COMPLETE_CB_ID);
        HAL_SPI_UnRegisterCallback(s_privatedata.handles[index], HAL_SPI_ERROR_CB_ID);
        
        s_privatedata.config[index] = {};
        s_privatedata.handles[index] = nullptr;
            
        bool bspdeinit = embot::hw::spi::getBSP().deinit(b);
        if(false == bspdeinit)
        {
            // do deinit in here
        }
        
        embot::core::binary::bit::clear(initialisedmask, embot::core::tointegral(b));
                
        return resOK;
    }        

    
    bool isbusy(SPI b)
    {
        if(false == initialised(b))
        {
            return false;
        } 
        return s_privatedata.transaction[embot::core::tointegral(b)].ongoing;     
    }

    
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
        
    // -
    // blocking mode

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
        s_privatedata.transaction[index].start(Transaction::Direction::RX, {});        
        // start reading
        HAL_StatusTypeDef rr = HAL_SPI_Receive_IT(s_privatedata.handles[index], reinterpret_cast<std::uint8_t*>(destination.pointer), destination.capacity);
 
        if(HAL_OK != rr)
        {
            // maybe ... clear transaction ...
            s_privatedata.transaction[index].clear();
            return resNOK; 
        }
        
        if(resOK == r)
        {   // the transaction has started, we must wait for its completion
            r = s_wait_for_transaction_completion(b, remaining);
        }
        
        return r;                      
    }    
    

    result_t write(SPI b, const embot::core::Data &source, embot::core::relTime timeout)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 

        if(false == source.isvalid())
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
        s_privatedata.transaction[index].start(Transaction::Direction::TX, {});
                    
        // start writing
        HAL_StatusTypeDef rr = HAL_SPI_Transmit_IT(s_privatedata.handles[index], reinterpret_cast<std::uint8_t*>(source.pointer), source.capacity);

        if(HAL_OK != rr)
        {
            // maybe ... clear transaction ...
            s_privatedata.transaction[index].clear();
            return resNOK; 
        }
        
        if(resOK == r)
        {   // the transaction has started, we must wait for its completion
            r = s_wait_for_transaction_completion(b, remaining);
        }
        
        return r;
    }

    
    result_t writeread(SPI b, const embot::core::Data &source, embot::core::Data &destination, embot::core::relTime timeout)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 

        if((false == source.isvalid()) || (false == destination.isvalid()))
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
        s_privatedata.transaction[index].start(Transaction::Direction::TXRX, {});
        
        // we transmit receive without getting out of tx or rx buffer.
        size_t size = std::min(destination.capacity, source.capacity);
                    
        // start writing / reading
        HAL_StatusTypeDef rr = HAL_SPI_TransmitReceive_IT(  s_privatedata.handles[index], 
                                                            reinterpret_cast<std::uint8_t*>(source.pointer),  
                                                            reinterpret_cast<std::uint8_t*>(destination.pointer), 
                                                            size);

        if(HAL_OK != rr)
        {
            // maybe ... clear transaction ...
            s_privatedata.transaction[index].clear();
            return resNOK; 
        }
        
        if(resOK == r)
        {   // the transaction has started, we must wait for its completion ... ok. 
            r = s_wait_for_transaction_completion(b, remaining);
        }
        
        return r;
    }    
             
    
    // -
    // non-blocking mode
    

    result_t read(SPI b, embot::core::Data &destination, const embot::core::Callback &oncompletion)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 
             
        if(false == destination.isvalid())
        {
            return resNOK;
        }
            
        if(true == isbusy(b))
        {
            return resOK;
        }
          
        result_t r {resOK};
              
        std::uint8_t index = embot::core::tointegral(b);  
        s_privatedata.transaction[index].start(Transaction::Direction::RX, oncompletion);        
        
        // start reading
        HAL_StatusTypeDef rr = HAL_SPI_Receive_IT(  s_privatedata.handles[index], 
                                                    reinterpret_cast<std::uint8_t*>(destination.pointer), 
                                                    destination.capacity
                                                 );
                                                    
        if(HAL_OK != rr)
        {
           // maybe ... clear transaction ...
           s_privatedata.transaction[index].clear();
        }
        
        // dont wait for end of operation                                                    
        return (HAL_OK == rr) ? resOK : resNOK;                      
    }    
    

    result_t write(SPI b, const embot::core::Data &source, const embot::core::Callback &oncompletion)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 

        if(false == source.isvalid())
        {
            return resNOK;
        }
        
        if(true == isbusy(b))
        {
            return resNOK;
        }        

        result_t r {resOK};           
        
        std::uint8_t index = embot::core::tointegral(b);        
        s_privatedata.transaction[index].start(Transaction::Direction::TX, oncompletion);
                    
        // start writing
        HAL_StatusTypeDef rr = HAL_SPI_Transmit_IT( s_privatedata.handles[index],
                                                    reinterpret_cast<std::uint8_t*>(source.pointer), 
                                                    source.capacity);

        if(HAL_OK != rr)
        {
           // maybe ... clear transaction ...
           s_privatedata.transaction[index].clear();
           return resNOK; 
        }
        
        // dont wait for end of operation                                                    
        return (HAL_OK == rr) ? resOK : resNOK;    
    }


    result_t writeread(SPI b, const embot::core::Data &source, embot::core::Data &destination, const embot::core::Callback &oncompletion)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 

        if((false == source.isvalid()) || (false == destination.isvalid()))
        {
            return resNOK;
        }
                   
        if(true == isbusy(b))
        {
            return resNOK;
        } 
        
        result_t r {resOK};        
        
        std::uint8_t index = embot::core::tointegral(b);        
        s_privatedata.transaction[index].start(Transaction::Direction::TXRX, oncompletion);
        
        // we transmit receive without getting out of tx or rx buffer.
        size_t size = std::min(destination.capacity, source.capacity);
                    
        // start writing / reading
        HAL_StatusTypeDef rr = HAL_SPI_TransmitReceive_IT(  s_privatedata.handles[index], 
                                                            reinterpret_cast<std::uint8_t*>(source.pointer),  
                                                            reinterpret_cast<std::uint8_t*>(destination.pointer), 
                                                            size);

        if(HAL_OK != rr)
        {
           // maybe ... clear transaction ...
           s_privatedata.transaction[index].clear();
           return resNOK; 
        }
        
        // dont wait for end of operation                                                    
        return (HAL_OK == rr) ? resOK : resNOK;
    }        
    
    // -
    // utility functions
                
    result_t s_wait_for_transaction_completion(SPI b, embot::core::relTime timeout)
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
 

    void s_SPI_TX_completed(SPI_HandleTypeDef *hspi)
    {
        embot::hw::SPI id = embot::hw::spi::getBSP().toID({hspi});
        if(embot::hw::SPI::none == id)
        {
            return;
        }        
        std::uint8_t index = embot::core::tointegral(id);
        embot::hw::spi::s_privatedata.transaction[index].stop(Transaction::Direction::TX);               
    }
    
    void s_SPI_rx_completed(SPI_HandleTypeDef *hspi)
    {   
        embot::hw::SPI id = embot::hw::spi::getBSP().toID({hspi});
        if(embot::hw::SPI::none == id)
        {
            return;
        }
        std::uint8_t index = embot::core::tointegral(id);
        embot::hw::spi::s_privatedata.transaction[index].stop(Transaction::Direction::RX);            
    }

    void s_SPI_TXrx_completed(SPI_HandleTypeDef *hspi)
    {
        embot::hw::SPI id = embot::hw::spi::getBSP().toID({hspi});
        if(embot::hw::SPI::none == id)
        {
            return;
        }
        std::uint8_t index = embot::core::tointegral(id);
        embot::hw::spi::s_privatedata.transaction[embot::core::tointegral(id)].stop(Transaction::Direction::TXRX);           
    }
        
    void s_SPI_error(SPI_HandleTypeDef *hspi)
    {
        
    }
    
   
           
}}} // namespace embot { namespace hw { namespace spi {

 


#endif //defined(HAL_SPI_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

