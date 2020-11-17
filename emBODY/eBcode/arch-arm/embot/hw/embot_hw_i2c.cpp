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

#include "embot_hw_i2c.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_i2c_bsp.h"

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


#if !defined(HAL_I2C_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_i2c)

namespace embot { namespace hw { namespace i2c {

    bool supported(I2C b)                                                                           { return false; }
    bool initialised(I2C b)                                                                         { return false; }
    result_t init(I2C b, const Config &config)                                                      { return resNOK; }
          
    // blocking   
    bool isbusy(embot::hw::I2C b, embot::core::relTime timeout, embot::core::relTime &remaining) { return false; }      
    bool ping(embot::hw::I2C b, ADR adr, embot::core::relTime timeout) { return false; }   
    result_t read(embot::hw::I2C b, ADR adr, Reg reg, embot::core::Data &destination, embot::core::relTime timeout) { return resNOK; } 
    result_t write(embot::hw::I2C b, ADR adr, Reg reg, const embot::core::Data &content, embot::core::relTime timeout) { return resNOK; }     
    result_t receive(embot::hw::I2C b, ADR adr, embot::core::Data &destination, embot::core::relTime timeout) { return resNOK; }  
    result_t transmit(embot::hw::I2C b, ADR adr, const embot::core::Data &content, embot::core::relTime timeout) { return resNOK; } 
    result_t transmitzeroaddress(embot::hw::I2C b, embot::core::relTime timeout) { return resNOK; }   
         
    // non blocking
    bool isbusy(embot::hw::I2C b) { return false; }
    result_t read(embot::hw::I2C b, ADR adr, Reg reg, embot::core::Data &destination, const embot::core::Callback &oncompletion) { return resNOK; }         
    result_t write(embot::hw::I2C b, ADR adr, Reg reg, const embot::core::Data &content, const embot::core::Callback &oncompletion) { return resNOK; }     
    result_t receive(embot::hw::I2C b, ADR adr, embot::core::Data &destination, const embot::core::Callback &oncompletion) { return resNOK; } 
    result_t transmit(I2C b, ADR adr, const embot::core::Data &content, const embot::core::Callback &oncompletion) { return resNOK; }

}}} // namespace embot { namespace hw { namespace i2c {

#else

namespace embot { namespace hw { namespace i2c {
    
    // types
    struct Transaction
    {
        volatile bool ongoing;
        static constexpr uint8_t txbuffercapacity = 16;
        ADR addr;
        embot::core::Data recdata;
        std::uint8_t txbuffer[txbuffercapacity];
        std::uint8_t txbuffersize;
        embot::core::Callback oncompletion;
        void clear() { ongoing = false; addr = 0; recdata.clear(); oncompletion.clear(); std::memset(txbuffer, 0, sizeof(txbuffer)); txbuffersize = 0;}    
        Transaction () { clear(); }        
    };
        
    struct PrivateData
    {    
        Config config[embot::core::tointegral(I2C::maxnumberof)];  
        Transaction  transaction[embot::core::tointegral(I2C::maxnumberof)]; 
        I2C_HandleTypeDef* handles[embot::core::tointegral(I2C::maxnumberof)];           
        PrivateData() { }
    };
    
    // utility functions
    
      
    static result_t s_wait_for_transaction_completion(I2C b, embot::core::relTime timeout);   
    static result_t s_DMA_write_transaction_start(I2C b, ADR adr, Reg reg, const embot::core::Data &content, const embot::core::Callback &oncompletion = embot::core::Callback(nullptr, nullptr));
    static result_t s_DMA_read_transaction_start(I2C b, ADR adr, Reg reg, embot::core::Data &destination, const embot::core::Callback &oncompletion = embot::core::Callback(nullptr, nullptr));        
    
    void ontransactionterminated(std::uint8_t index);  
    
    
    
    // internal variables
    static PrivateData s_privatedata;
              
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    // the public functions
    
    bool supported(embot::hw::I2C p)
    {
        return embot::hw::i2c::getBSP().supported(p);
    }

    bool initialised(embot::hw::I2C p)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(p));
    }    
        
    result_t init(I2C b, const Config &config)
    {
        if(false == supported(b))
        {
            return resNOK;
        }
        
        if(true == initialised(b))
        {
            return resOK;
        }        
        
        embot::hw::i2c::getBSP().init(b);
        
        std::uint8_t index = embot::core::tointegral(b);
        s_privatedata.config[index] = config;
        s_privatedata.handles[index] = embot::hw::i2c::getBSP().getPROP(b)->handle;
                       
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(b));
                
        return resOK;
    }
           
    
    bool discover(embot::hw::I2C b, std::vector<ADR> &adrs)
    {
        if(false == initialised(b))
        {
            return false;    
        }
                      
        adrs.clear();
        
        static const uint8_t start = 2;
        for(int i=start; i<256; i++)
        {
            if((i%2) == 1) { continue; }
            
            if(true == ping(b, i, 3*embot::core::time1millisec))
            {
                adrs.push_back(i);
            }
        } 

        return !adrs.empty();        
    }
    
    
    // -
    // blocking mode
    
    result_t tx(embot::hw::I2C b, ADR adr, const embot::core::Data &content, embot::core::relTime timeout)
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
        
        // in here we just transmit over the bus w/ HAL_I2C_Master_Transmit() w/out DMA. the call is blocking w/ timeout
        // we must nevertheless set the bus busy before trasmission and set it free afterwards. 
        std::uint8_t index = embot::core::tointegral(b);             
        embot::hw::i2c::s_privatedata.transaction[index].ongoing = true;       
        volatile HAL_StatusTypeDef r = HAL_I2C_Master_Transmit(s_privatedata.handles[index], static_cast<uint16_t>(adr), reinterpret_cast<uint8_t*>(content.pointer), static_cast<uint16_t>(content.capacity), timeout/1000);               
        embot::hw::i2c::s_privatedata.transaction[index].ongoing = false;
        
        return (HAL_OK == r) ? resOK : resNOK;
    }        
    
    
    bool isbusy(I2C b, embot::core::relTime timeout, embot::core::relTime &remaining)
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
            std::int64_t rem = deadline - embot::core::now();
            
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
        
    
    bool ping(I2C b, ADR adr, embot::core::relTime timeout)
    {
        if(false == initialised(b))
        {
            return false;
        } 
          
        embot::core::relTime remaining = timeout;
        if(true == isbusy(b, timeout, remaining))
        {
            return false;
        }
        
        // we use a call to HAL_I2C_IsDeviceReady() which has a timeout. nevertheless we set the bus busy and we clear afterwards. 
        std::uint8_t index = embot::core::tointegral(b);        
        s_privatedata.transaction[index].ongoing = true;                
        HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(s_privatedata.handles[index], adr, 1, remaining/1000);        
        s_privatedata.transaction[index].ongoing = false;
        
        return (HAL_OK == status) ? true : false;       
    }


    result_t read(I2C b, ADR adr, Reg reg, embot::core::Data &destination, embot::core::relTime timeout)
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
          
        result_t r = s_DMA_read_transaction_start(b, adr, reg, destination);
        
        if(resOK == r)
        {   // it transaction has started, we must wait for its completion
            r = s_wait_for_transaction_completion(b, remaining);
        }
        
        return r;                      
    }    
    

    result_t write(I2C b, ADR adr, Reg reg, const embot::core::Data &content, embot::core::relTime timeout)
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
        
        // s_DMA_write_transaction_start() starts the transaction.        
        result_t r = s_DMA_write_transaction_start(b, adr, reg, content);
        
        if(resOK == r)
        {   // it transaction has started, we must wait for its completion
            r = s_wait_for_transaction_completion(b, remaining);
        }
        
        return r;
    }
    
    
    result_t receive(I2C b, ADR adr, embot::core::Data &destination, embot::core::relTime timeout)
    {
        constexpr Reg regnone { Reg::addressNONE, Reg::Size::none }; 
        // with addressNONE, inside read() the function s_DMA_read_transaction_start() starts a single-stage transaction using DMA
        return read(b, adr, regnone, destination, timeout);
    }    
        
    result_t transmit(embot::hw::I2C b, ADR adr, const embot::core::Data &content, embot::core::relTime timeout)
    {
        constexpr Reg regnone { Reg::addressNONE, Reg::Size::none }; 
        // with addressNONE, inside read() the function s_DMA_read_transaction_start() starts a single-stage transaction using DMA
        return write(b, adr, regnone, content, timeout);       
    }
        
    
//    result_t transmitzeroaddress(embot::hw::I2C b, embot::core::relTime timeout)
//    {
//        return tx(b, 0, {nullptr, 0}, timeout);
//    }
    

   
           
    
    // -
    // non-blocking mode
    
        
    bool isbusy(I2C b)
    {
        if(false == initialised(b))
        {
            return false;
        } 
        return s_privatedata.transaction[embot::core::tointegral(b)].ongoing;     
    }

    
    result_t read(I2C b, ADR adr, Reg reg, embot::core::Data &destination, const embot::core::Callback &oncompletion)
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
            return resNOK;
        }
               
        // we start a DMA reading transaction and we dont wait for its completion
        return s_DMA_read_transaction_start(b, adr, reg, destination, oncompletion);
    }    
    

    result_t write(I2C b, ADR adr, Reg reg, const embot::core::Data &content, const embot::core::Callback &oncompletion)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 
        
        if(false == content.isvalid())
        {
            return resNOK;
        } 
        
        if(true == isbusy(b))
        {
            return resNOK;
        }
         
        // we start a DMA writing transaction and we dont wait for its completion        
        return s_DMA_write_transaction_start(b, adr, reg, content, oncompletion);
    }    
    

    result_t receive(I2C b, ADR adr, embot::core::Data &destination, const embot::core::Callback &oncompletion)
    {
        constexpr Reg regnone { Reg::addressNONE, Reg::Size::none };        
        // with addressNONE, inside read() the function s_DMA_read_transaction_start() starts a single-stage transaction using DMA
        return read(b, adr, regnone, destination, oncompletion);
    }  

    
    result_t transmit(I2C b, ADR adr, const embot::core::Data &content, const embot::core::Callback &oncompletion)
    {
        constexpr Reg regnone { Reg::addressNONE, Reg::Size::none }; 
        // with addressNONE, inside read() the function s_DMA_read_transaction_start() starts a single-stage transaction using DMA
        return write(b, adr, regnone, content, oncompletion);       
    }    
    
    // -
    // utility functions
    
            
    static result_t s_wait_for_transaction_completion(I2C b, embot::core::relTime timeout)
    {
        embot::core::Time start = embot::core::now();
        
        result_t res = resOK;
        for(;;)
        {
            if(false == isbusy(b))
            {
                break;
            }
            
            if(embot::core::now() > (start+timeout))
            {
                res = resNOK;
                break;
            }
        } 

        return res;
    }   
        
    static result_t s_DMA_write_transaction_start(I2C b, ADR adr, Reg reg, const embot::core::Data &content, const embot::core::Callback &oncompletion)
    { 
        result_t res = resNOK; 
        
        std::uint8_t index = embot::core::tointegral(b);        
        s_privatedata.transaction[index].addr = adr;
        s_privatedata.transaction[index].oncompletion = oncompletion;
        s_privatedata.transaction[index].ongoing = true;
        s_privatedata.transaction[index].recdata.clear(); // invalide recdata, so that HAL_I2C_MasterTxCpltCallback() knows it is a write operation.
        
        if(Reg::addressNONE == reg.address)
        {
            // i need to transmit the pattern [data] because the slave already knows into which register to put data. and does not expect a [reg] byte.
            uint16_t txsize = content.capacity;
            if(txsize > Transaction::txbuffercapacity)
            {
                s_privatedata.transaction[index].ongoing = false;
                return resNOK;
            }
            std::memmove(&s_privatedata.transaction[index].txbuffer[0], content.pointer, content.capacity);
            s_privatedata.transaction[index].txbuffersize = content.capacity;            
        }
        else
        {
            // i need to transmit the pattern [reg.address][data] because the slave must know into which register to put data.            
            uint16_t txsize = (Reg::Size::eightbits == reg.size) ? (content.capacity + 1) : (content.capacity + 2);
            if(txsize > Transaction::txbuffercapacity)
            {
                s_privatedata.transaction[index].ongoing = false;
                return resNOK;
            }
            
            if(Reg::Size::eightbits == reg.size)
            {
                s_privatedata.transaction[index].txbuffer[0] = reg.address & 0xff;
                std::memmove(&s_privatedata.transaction[index].txbuffer[1], content.pointer, content.capacity);
                s_privatedata.transaction[index].txbuffersize = content.capacity + 1;                
            }
            else
            {
                s_privatedata.transaction[index].txbuffer[0] = (reg.address >> 8 ) & 0xff;
                s_privatedata.transaction[index].txbuffer[1] = reg.address & 0xff;                
                std::memmove(&s_privatedata.transaction[index].txbuffer[2], content.pointer, content.capacity);
                s_privatedata.transaction[index].txbuffersize = content.capacity + 2;
            }
        }
        
        HAL_StatusTypeDef r = HAL_I2C_Master_Transmit_DMA(s_privatedata.handles[index], adr, static_cast<std::uint8_t*>(s_privatedata.transaction[index].txbuffer), s_privatedata.transaction[index].txbuffersize);        
        res = (HAL_OK == r) ? resOK : resNOK;
        
        if(resOK != res)
        {
            s_privatedata.transaction[index].ongoing = false;
        }        
        return res;
    }        


    static result_t s_DMA_read_transaction_start(I2C b, ADR adr, Reg reg, embot::core::Data &destination, const embot::core::Callback &oncompletion)
    {
        result_t res = resNOK;
        
        std::uint8_t index = embot::core::tointegral(b);
        s_privatedata.transaction[index].addr = adr;
        s_privatedata.transaction[index].oncompletion = oncompletion;
        s_privatedata.transaction[index].ongoing = true;
        s_privatedata.transaction[index].recdata = destination;  
        
        if(reg.address == Reg::addressNONE)
        {
            // it is a single-stage operation: read<values>
            // i dont need to ask which register to read from because the slave already knows what to send.
            // i do the read<> request with HAL_I2C_Master_Receive_DMA(recdata).
            // the completion of reading operation is signalled by the call of HAL_I2C_MasterRxCpltCallback(). 
            // at this stage recdata will contain the received data.            
            I2C_HandleTypeDef* hi2cx = embot::hw::i2c::s_privatedata.handles[index];
            HAL_StatusTypeDef r = HAL_I2C_Master_Receive_DMA( hi2cx, 
                                        embot::hw::i2c::s_privatedata.transaction[index].addr, 
                                        static_cast<std::uint8_t*>(embot::hw::i2c::s_privatedata.transaction[index].recdata.pointer),  
                                        static_cast<std::uint16_t>(embot::hw::i2c::s_privatedata.transaction[index].recdata.capacity)
                                    );  
            res = (HAL_OK == r) ? resOK : resNOK;                                        
        }
        else
        {
            // it is a two-stage operation: write<reg>, read<values>
            // i must at first tx the value of the register i want to read from. i use HAL_I2C_Master_Transmit_DMA().
            // the call of HAL_I2C_MasterTxCpltCallback() signals the end of teh fist stage and ... if recdata.isvalid() then ...
            // i perform the read<> request with HAL_I2C_Master_Receive_DMA(recdata).
            // the completion of reading operation is signalled by the call of HAL_I2C_MasterRxCpltCallback(). 
            // at this stage recdata will contain the received data.
                    
            // so far we only manage addresses which are 1 or 2 bytes long ...
            if(Reg::Size::eightbits == reg.size)
            {
                s_privatedata.transaction[index].txbuffer[0] = reg.address & 0xff;
                s_privatedata.transaction[index].txbuffersize = 1;
            }
            else
            {   // big endianess ....
                s_privatedata.transaction[index].txbuffer[0] = (reg.address >> 8 ) & 0xff;
                s_privatedata.transaction[index].txbuffer[1] = reg.address & 0xff;
                s_privatedata.transaction[index].txbuffersize = 2;
            }
            HAL_StatusTypeDef r = HAL_I2C_Master_Transmit_DMA(s_privatedata.handles[index], adr, static_cast<std::uint8_t*>(s_privatedata.transaction[index].txbuffer), s_privatedata.transaction[index].txbuffersize);        
            res = (HAL_OK == r) ? resOK : resNOK;
        }
        
        if(resOK != res)
        {
            s_privatedata.transaction[index].ongoing = false;
        }
        return res;
    } 
    

    void ontransactionterminated(std::uint8_t index)
    {
        embot::hw::i2c::s_privatedata.transaction[index].ongoing = false;        
        embot::hw::i2c::s_privatedata.transaction[index].oncompletion.execute();                                
    }    
     
}}} // namespace embot { namespace hw { namespace i2c {


// functions required by the hal of stm32. they are called by the hw at the completion of a tx or rx transaction
    
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{        
    embot::hw::I2C id = embot::hw::i2c::getBSP().toID({I2cHandle, embot::hw::i2c::Speed::none});
    
    if(embot::hw::I2C::none == id)
    {
        return;
    }
    
    std::uint8_t index = embot::core::tointegral(id);
    
               
    if(false == embot::hw::i2c::s_privatedata.transaction[index].recdata.isvalid())
    {
        // it is a write operation: the transaction is terminated
        embot::hw::i2c::ontransactionterminated(index);        
    }
    else
    {
        // it is a read operation i still need to receive data 
        I2C_HandleTypeDef* hi2cx = embot::hw::i2c::s_privatedata.handles[index];
        //I2C_HandleTypeDef *hi2cx = (0 == index) ? (&hi2c1) : (&hi2c2);
        HAL_I2C_Master_Receive_DMA( hi2cx, 
                                    embot::hw::i2c::s_privatedata.transaction[index].addr, 
                                    static_cast<std::uint8_t*>(embot::hw::i2c::s_privatedata.transaction[index].recdata.pointer),  
                                    static_cast<std::uint16_t>(embot::hw::i2c::s_privatedata.transaction[index].recdata.capacity)
                                    );
    }
    
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
    embot::hw::I2C id = embot::hw::i2c::getBSP().toID({I2cHandle, embot::hw::i2c::Speed::none});
    
    if(embot::hw::I2C::none == id)
    {
        return;
    }
    
    std::uint8_t index = embot::core::tointegral(id);
    
    embot::hw::i2c::ontransactionterminated(index);        
}    
    

// marco.accame on 6 feb 2019: in case any other board or driver version needs different IRQHandlers ...
// we could move them into the embot_hw_bsp.cpp file (but oustide the hw::bsp::i2c namespace !!!!)

// IRQ handlers are in hw bsp file
    


#endif //defined(HAL_I2C_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

