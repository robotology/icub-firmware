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

#include <cstring>
#include <vector>

using namespace std;

#include "embot_binary.h"
#include "embot_hw_sys.h"
#include "embot_hw_bsp.h"
#include "stm32hal.h"


using namespace embot::hw;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if     !defined(HAL_I2C_MODULE_ENABLED)


namespace embot { namespace hw { namespace i2c {

    bool supported(I2C b)                                                                           { return false; }
    bool initialised(I2C b)                                                                         { return false; }
    result_t init(I2C b, const Config &config)                                                      { return resNOK; }
    
    bool ping(I2C b, std::uint8_t adr, std::uint8_t retries, embot::common::relTime timeout)        { return false; }       
    result_t read(I2C b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, embot::common::Callback oncompletion) { return resNOK; }
    bool isbusy(I2C b)                                                                              { return false; }

}}} // namespace embot { namespace hw { namespace i2c {


#elif   defined(HAL_I2C_MODULE_ENABLED)


namespace embot { namespace hw { namespace i2c {
        
      
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(embot::hw::I2C p)
    {
        return embot::hw::bsp::i2c::getMAP()->supported(p);
    }

    bool initialised(embot::hw::I2C p)
    {
        return embot::binary::bit::check(initialisedmask, embot::hw::bsp::i2c::MAP::toindex(p));
    }    
 
     
    struct Transaction
    {
        volatile bool ongoing;
        std::uint8_t adr;
        embot::common::Data recdata;
        std::uint8_t txbuffer[4];
        std::uint8_t txbuffersize;
        embot::common::Callback oncompletion;
        void clear() { ongoing = false; adr = 0; recdata.clear(); oncompletion.clear(); std::memset(txbuffer, 0, sizeof(txbuffer)); txbuffersize = 0;}         
    };
    
    
    struct PrivateData
    {    
        Config config[static_cast<unsigned int>(I2C::maxnumberof)];  
        Transaction  transaction[static_cast<unsigned int>(I2C::maxnumberof)]; 
        I2C_HandleTypeDef* handles[static_cast<unsigned int>(I2C::maxnumberof)];  
        //DMA_HandleTypeDef* handlesdmatx[static_cast<unsigned int>(I2C::maxnumberof)];   
        //DMA_HandleTypeDef* handlesdmarx[static_cast<unsigned int>(I2C::maxnumberof)];         
        PrivateData() { }
    };
    
    static PrivateData s_privatedata;
    
    static result_t s_read(I2C b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
    static result_t s_write(I2C b, std::uint8_t adr, std::uint8_t reg, const embot::common::Data &content, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
    static result_t s_wait(I2C b, embot::common::relTime timeout);
    

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
        
        std::uint8_t index = embot::hw::bsp::i2c::MAP::toindex(b);
        s_privatedata.config[index] = config;
        s_privatedata.handles[index] = reinterpret_cast<I2C_HandleTypeDef*>(embot::hw::bsp::i2c::getMAP()->getprops(b)->handle);
        //s_privatedata.handlesdmatx[index] = reinterpret_cast<DMA_HandleTypeDef*>(embot::hw::bsp::i2c::getMAP()->getprops(b)->handledmatx);
        //s_privatedata.handlesdmarx[index] = reinterpret_cast<DMA_HandleTypeDef*>(embot::hw::bsp::i2c::getMAP()->getprops(b)->handledmarx);
        
        
        embot::hw::bsp::i2c::getMAP()->init(b);
        
        embot::binary::bit::set(initialisedmask, embot::hw::bsp::i2c::MAP::toindex(b));
                
        return resOK;
    }
    
    
    bool ping(I2C b, std::uint8_t adr, embot::common::relTime timeout)
    {
        if(false == initialised(b))
        {
            return false;
        } 
          
        embot::common::relTime remaining = timeout;
        if(true == isbusy(b, timeout, remaining))
        {
            return false;
        }
        
        std::uint8_t index = embot::hw::bsp::i2c::MAP::toindex(b);
        
        s_privatedata.transaction[index].ongoing = true;
                
        HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(s_privatedata.handles[index], adr, 1, remaining/1000);
        
        s_privatedata.transaction[index].ongoing = false;
        
        return (HAL_OK == status) ? true : false;       
    }

    
    result_t read(I2C b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, const embot::common::Callback &oncompletion)
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
                           
        return s_read(b, adr, reg, destination, oncompletion);
    }
    
    result_t read(I2C b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, embot::common::relTime timeout)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 
             
        if(false == destination.isvalid())
        {
            return resNOK;
        }
        
        embot::common::relTime remaining = timeout;       
        if(true == isbusy(b, timeout, remaining))
        {
            return resNOK;
        }
                        
        result_t r = s_read(b, adr, reg, destination);
        
        if(resOK == r)
        {
            r = s_wait(b, remaining);
        }        
        return r;                      
    }
    

    result_t write(I2C b, std::uint8_t adr, std::uint8_t reg, const embot::common::Data &content, const embot::common::Callback &oncompletion)
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
                        
        return s_write(b, adr, reg, content, oncompletion);
    }    


    result_t write(I2C b, std::uint8_t adr, std::uint8_t reg, const embot::common::Data &content, embot::common::relTime timeout)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 

        if(false == content.isvalid())
        {
            return resNOK;
        }
        
        embot::common::relTime remaining = timeout;
        if(true == isbusy(b, timeout, remaining))
        {
            return resNOK;
        }        
         
        result_t r = s_write(b, adr, reg, content);
        
        if(resOK == r)
        {
            r = s_wait(b, remaining);
        }
        
        return r;
    }

    
    bool isbusy(I2C b)
    {
        if(false == initialised(b))
        {
            return false;
        } 

        return s_privatedata.transaction[embot::hw::bsp::i2c::MAP::toindex(b)].ongoing;     
    }

    
    bool isbusy(I2C b, embot::common::relTime timeout, embot::common::relTime &remaining)
    {
        if(false == initialised(b))
        {
            return false;
        } 

        if(0 == timeout)
        {
            remaining = timeout;
            return s_privatedata.transaction[embot::hw::bsp::i2c::MAP::toindex(b)].ongoing;   
        }
       
        
        embot::common::Time deadline = embot::hw::sys::now() + timeout;
        
        bool res = true;
        for(;;)
        {
            std::int64_t rem = deadline - embot::hw::sys::now();
            
            if(rem <= 0)
            {   
                remaining = 0;
                res = true;
                break;
            }
            else if(false == s_privatedata.transaction[embot::hw::bsp::i2c::MAP::toindex(b)].ongoing)
            {
                remaining = static_cast<embot::common::relTime>(rem);
                res = false;   
                break;                
            }
            
        }
        
        return res;        
    }    
    
    void ontransactionterminated(std::uint8_t index)
    {
        embot::hw::i2c::s_privatedata.transaction[index].ongoing = false;        
        embot::hw::i2c::s_privatedata.transaction[index].oncompletion.execute();                                
    }
    
    
    static result_t s_wait(I2C b, embot::common::relTime timeout)
    {
        embot::common::Time start = embot::hw::sys::now();
        
        result_t res = resOK;
        for(;;)
        {
            if(false == isbusy(b))
            {
                break;
            }
            
            if(embot::hw::sys::now() > (start+timeout))
            {
                res = resNOK;
                break;
            }
        } 

        return res;
    }   
    
       
    
    static result_t s_write(I2C b, std::uint8_t adr, std::uint8_t reg, const embot::common::Data &content, const embot::common::Callback &oncompletion)
    {   
        std::uint8_t index = embot::hw::bsp::i2c::MAP::toindex(b);        
        s_privatedata.transaction[index].adr = adr;
        s_privatedata.transaction[index].oncompletion = oncompletion;
        s_privatedata.transaction[index].ongoing = true;
        s_privatedata.transaction[index].recdata.clear();
        s_privatedata.transaction[index].txbuffer[0] = reg;
        std::memmove(&s_privatedata.transaction[index].txbuffer[1], content.pointer, content.size);
        s_privatedata.transaction[index].txbuffersize = content.size + 1;
        
        HAL_StatusTypeDef r = HAL_I2C_Master_Transmit_DMA(s_privatedata.handles[index], adr, static_cast<std::uint8_t*>(s_privatedata.transaction[index].txbuffer), s_privatedata.transaction[index].txbuffersize);        
                
        return (HAL_OK == r) ? resOK : resNOK;
    }    
    
    static result_t s_read(I2C b, std::uint8_t adr, std::uint8_t reg, embot::common::Data &destination, const embot::common::Callback &oncompletion)
    {
        std::uint8_t index = embot::hw::bsp::i2c::MAP::toindex(b);        
        s_privatedata.transaction[index].adr = adr;
        s_privatedata.transaction[index].oncompletion = oncompletion;
        s_privatedata.transaction[index].ongoing = true;
        s_privatedata.transaction[index].recdata = destination;  
        // register address is 1 byte long ...
        s_privatedata.transaction[index].txbuffer[0] = reg;
        s_privatedata.transaction[index].txbuffersize = 1;
        HAL_StatusTypeDef r = HAL_I2C_Master_Transmit_DMA(s_privatedata.handles[index], adr, static_cast<std::uint8_t*>(s_privatedata.transaction[index].txbuffer), s_privatedata.transaction[index].txbuffersize);        
        
        return (HAL_OK == r) ? resOK : resNOK;
    }
     
}}} // namespace embot { namespace hw { namespace i2c {


    
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{    
    std::uint8_t index = 0;
    
    if(I2cHandle->Instance == I2C1)
    {
        index = 0;
    }
    else if(I2cHandle->Instance == I2C2)
    {
        index = 1;
    }
    else
    {
        return;
    }
        
        
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
                                    embot::hw::i2c::s_privatedata.transaction[index].adr, 
                                    static_cast<std::uint8_t*>(embot::hw::i2c::s_privatedata.transaction[index].recdata.pointer),  
                                    static_cast<std::uint16_t>(embot::hw::i2c::s_privatedata.transaction[index].recdata.size)
                                    );
    }
    
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
    std::uint8_t index = 0;
    
    if(I2cHandle->Instance == I2C1)
    {
       index = 0;
    }
    else if(I2cHandle->Instance == I2C2)
    {
       index = 1;
    }
    else
    {
        return;
    }
    
    embot::hw::i2c::ontransactionterminated(index);        
}    
    

// marco.accame on 6 feb 2019: in case any other board or driver version needs different IRQHandlers ...
// we could move them into the embot_hw_bsp.cpp file (but oustide the hw::bsp::i2c namespace !!!!)

// IRQ handlers are in hw bsp file
    


#endif //defined(HAL_I2C_MODULE_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

