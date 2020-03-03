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
#include "embot_hw_bsp_config.h"
#include "stm32hal.h"


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
          
    bool isbusy(embot::hw::I2C b) { return false; }   
    bool isbusy(embot::hw::I2C b, embot::common::relTime timeout, embot::common::relTime &remaining) { return false; }   
    
    bool ping(embot::hw::I2C b, ADR adr, embot::common::relTime timeout) { return false; }   
    result_t transmit(embot::hw::I2C b, ADR adr, const embot::utils::Data &content, embot::common::relTime timeout) { return resNOK; }           
    result_t read(embot::hw::I2C b, ADR adr, REG reg, embot::utils::Data &destination, const embot::common::Callback &oncompletion) { return resNOK; }     
    result_t read(embot::hw::I2C b, ADR adr, REG reg, embot::utils::Data &destination, embot::common::relTime timeout) { return resNOK; } 
    result_t write(embot::hw::I2C b, ADR adr, REG reg, const embot::utils::Data &content, const embot::common::Callback &oncompletion) { return resNOK; } 
    result_t write(embot::hw::I2C b, ADR adr, REG reg, const embot::utils::Data &content, embot::common::relTime timeout) { return resNOK; }         

}}} // namespace embot { namespace hw { namespace i2c {

#else

namespace embot { namespace hw { namespace i2c {
        
      
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(embot::hw::I2C p)
    {
        return embot::hw::bsp::i2c::getBSP().supported(p);
    }

    bool initialised(embot::hw::I2C p)
    {
        return embot::binary::bit::check(initialisedmask, embot::common::tointegral(p));
    }    
 
     
    struct Transaction
    {
        volatile bool ongoing;
        static constexpr uint8_t txbuffercapacity = 16;
        ADR addr;
        embot::utils::Data recdata;
        std::uint8_t txbuffer[txbuffercapacity];
        std::uint8_t txbuffersize;
        embot::common::Callback oncompletion;
        void clear() { ongoing = false; addr = 0; recdata.clear(); oncompletion.clear(); std::memset(txbuffer, 0, sizeof(txbuffer)); txbuffersize = 0;} 
//        bool loadtx(const embot::utils::Data &txdata) 
//        { 
//            if(txdata.size >= txbuffercapacity) { return false; } 
//            
    };
    
    
    struct PrivateData
    {    
        Config config[embot::common::tointegral(I2C::maxnumberof)];  
        Transaction  transaction[embot::common::tointegral(I2C::maxnumberof)]; 
        I2C_HandleTypeDef* handles[embot::common::tointegral(I2C::maxnumberof)];  
        //DMA_HandleTypeDef* handlesdmatx[static_cast<unsigned int>(I2C::maxnumberof)];   
        //DMA_HandleTypeDef* handlesdmarx[static_cast<unsigned int>(I2C::maxnumberof)];         
        PrivateData() { }
    };
    
    static PrivateData s_privatedata;
    
    static result_t s_read(I2C b, ADR adr, REG reg, embot::utils::Data &destination, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
    static result_t s_write(I2C b, ADR adr, REG reg, const embot::utils::Data &content, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
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
        
        embot::hw::bsp::i2c::getBSP().init(b);
        
        std::uint8_t index = embot::common::tointegral(b);
        s_privatedata.config[index] = config;
        s_privatedata.handles[index] = embot::hw::bsp::i2c::getBSP().getPROP(b)->handle;
        //s_privatedata.handlesdmatx[index] = embot::hw::bsp::i2c::getBSP().getPROP(b)->handledmatx);
        //s_privatedata.handlesdmarx[index] = embot::hw::bsp::i2c::getBSP().getPROP(b)->handledmarx);
        
        

        
        embot::binary::bit::set(initialisedmask, embot::common::tointegral(b));
                
        return resOK;
    }
    
    
    bool ping(I2C b, ADR adr, embot::common::relTime timeout)
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
        
        std::uint8_t index = embot::common::tointegral(b);
        
        s_privatedata.transaction[index].ongoing = true;
                
        HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(s_privatedata.handles[index], adr, 1, remaining/1000);
        
        s_privatedata.transaction[index].ongoing = false;
        
        return (HAL_OK == status) ? true : false;       
    }

    
    result_t read(I2C b, ADR adr, REG reg, embot::utils::Data &destination, const embot::common::Callback &oncompletion)
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
    
    result_t read(I2C b, ADR adr, REG reg, embot::utils::Data &destination, embot::common::relTime timeout)
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
    

    result_t write(I2C b, ADR adr, REG reg, const embot::utils::Data &content, const embot::common::Callback &oncompletion)
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


    result_t write(I2C b, ADR adr, REG reg, const embot::utils::Data &content, embot::common::relTime timeout)
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

        return s_privatedata.transaction[embot::common::tointegral(b)].ongoing;     
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
            return s_privatedata.transaction[embot::common::tointegral(b)].ongoing;   
        }
       
        
        embot::common::Time deadline = embot::hw::bsp::now() + timeout;
        
        bool res = true;
        for(;;)
        {
            std::int64_t rem = deadline - embot::hw::bsp::now();
            
            if(rem <= 0)
            {   
                remaining = 0;
                res = true;
                break;
            }
            else if(false == s_privatedata.transaction[embot::common::tointegral(b)].ongoing)
            {
                remaining = static_cast<embot::common::relTime>(rem);
                res = false;   
                break;                
            }
            
        }
        
        return res;        
    }   

//    result_t clear(embot::hw::I2C b)
//    {
//        if(false == initialised(b))
//        {
//            return resNOK;
//        } 
//        
//        std::uint8_t index = embot::common::tointegral(b);
//        
//        embot::hw::i2c::s_privatedata.transaction[index].clear();
//        
//        return resOK;
//    }    
    
    void ontransactionterminated(std::uint8_t index)
    {
        embot::hw::i2c::s_privatedata.transaction[index].ongoing = false;        
        embot::hw::i2c::s_privatedata.transaction[index].oncompletion.execute();                                
    }
    
    
    static result_t s_wait(I2C b, embot::common::relTime timeout)
    {
        embot::common::Time start = embot::hw::bsp::now();
        
        result_t res = resOK;
        for(;;)
        {
            if(false == isbusy(b))
            {
                break;
            }
            
            if(embot::hw::bsp::now() > (start+timeout))
            {
                res = resNOK;
                break;
            }
        } 

        return res;
    }   
    
    
    result_t transmit(embot::hw::I2C b, ADR adr, const embot::utils::Data &content, embot::common::relTime timeout)
    {
        if(false == initialised(b))
        {
            return resNOK;
        } 
                
        if(true == isbusy(b))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::common::tointegral(b);     
        
        embot::hw::i2c::s_privatedata.transaction[index].ongoing = true; 
       
        volatile HAL_StatusTypeDef r = HAL_I2C_Master_Transmit(s_privatedata.handles[index], static_cast<uint16_t>(adr), reinterpret_cast<uint8_t*>(content.pointer), static_cast<uint16_t>(content.size), timeout/1000);        
        
        embot::hw::i2c::s_privatedata.transaction[index].ongoing = false;
        
        return (HAL_OK == r) ? resOK : resNOK;
    }
    
    static result_t s_write(I2C b, ADR adr, REG reg, const embot::utils::Data &content, const embot::common::Callback &oncompletion)
    {   
        std::uint8_t index = embot::common::tointegral(b);        
        s_privatedata.transaction[index].addr = adr;
        s_privatedata.transaction[index].oncompletion = oncompletion;
        s_privatedata.transaction[index].ongoing = true;
        s_privatedata.transaction[index].recdata.clear(); // invalide recdata, so that HAL_I2C_MasterTxCpltCallback() knows it is a write operation.
        
        if(regNONE == reg)
        {
            // i need to transmit the pattern [data] because the slave already knows into which register to put data. and does not expect a [reg] byte.
            if(content.size > Transaction::txbuffercapacity)
            {
                return resNOK;
            }
            std::memmove(&s_privatedata.transaction[index].txbuffer[0], content.pointer, content.size);
            s_privatedata.transaction[index].txbuffersize = content.size;            
        }
        else
        {
            // i need to transmit the pattern [reg][data] because the slave must know into which register to put data.
            if(content.size >= Transaction::txbuffercapacity)
            {
                return resNOK;
            }
            s_privatedata.transaction[index].txbuffer[0] = reg;
            std::memmove(&s_privatedata.transaction[index].txbuffer[1], content.pointer, content.size);
            s_privatedata.transaction[index].txbuffersize = content.size + 1;
        }
        
        HAL_StatusTypeDef r = HAL_I2C_Master_Transmit_DMA(s_privatedata.handles[index], adr, static_cast<std::uint8_t*>(s_privatedata.transaction[index].txbuffer), s_privatedata.transaction[index].txbuffersize);        
                
        return (HAL_OK == r) ? resOK : resNOK;
    }    
    
    static result_t s_read(I2C b, ADR adr, REG reg, embot::utils::Data &destination, const embot::common::Callback &oncompletion)
    {
        result_t res = resNOK;
        
        std::uint8_t index = embot::common::tointegral(b);
        s_privatedata.transaction[index].addr = adr;
        s_privatedata.transaction[index].oncompletion = oncompletion;
        s_privatedata.transaction[index].ongoing = true;
        s_privatedata.transaction[index].recdata = destination;  
        
        if(reg == regNONE)
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
                                        static_cast<std::uint16_t>(embot::hw::i2c::s_privatedata.transaction[index].recdata.size)
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
                    
            // so far we only manage addresses which are 1 byte long ...
            s_privatedata.transaction[index].txbuffer[0] = reg;
            s_privatedata.transaction[index].txbuffersize = 1;
            HAL_StatusTypeDef r = HAL_I2C_Master_Transmit_DMA(s_privatedata.handles[index], adr, static_cast<std::uint8_t*>(s_privatedata.transaction[index].txbuffer), s_privatedata.transaction[index].txbuffersize);        
            res = (HAL_OK == r) ? resOK : resNOK;
        }

        return res;
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
    else if(I2cHandle->Instance == I2C3)
    {
        index = 2;
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
                                    embot::hw::i2c::s_privatedata.transaction[index].addr, 
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
    else if(I2cHandle->Instance == I2C3)
    {
        index = 2;
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

