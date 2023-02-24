
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_i2ce.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_i2ce_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"
#include "embot_hw_gpio.h"
#include "embot_hw_sys.h"

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


#if !defined(EMBOT_ENABLE_hw_i2ce)

namespace embot { namespace hw { namespace i2ce {

    bool supported(I2CE b) { return false; }
    bool initialised(I2CE b) { return false; }
    result_t init(I2CE b, const Config &config) { return resNOK; }
    result_t deinit(I2CE b) { return resNOK; }
          
    // blocking   
    bool isbusy(embot::hw::I2CE b, embot::core::relTime timeout, embot::core::relTime &remaining) { return false; }      
    bool ping(embot::hw::I2CE b, ADR adr, embot::core::relTime timeout) { return false; }   
    result_t read(embot::hw::I2CE b, ADR adr, Reg reg, embot::core::Data &destination, embot::core::relTime timeout) { return resNOK; } 
    result_t write(embot::hw::I2CE b, ADR adr, Reg reg, const embot::core::Data &content, embot::core::relTime timeout) { return resNOK; }     
    result_t receive(embot::hw::I2CE b, ADR adr, embot::core::Data &destination, embot::core::relTime timeout) { return resNOK; }  
    result_t transmit(embot::hw::I2CE b, ADR adr, const embot::core::Data &content, embot::core::relTime timeout) { return resNOK; } 
    result_t transmitzeroaddress(embot::hw::I2CE b, embot::core::relTime timeout) { return resNOK; }   
         
    // non blocking
    bool isbusy(embot::hw::I2CE b) { return false; }
    result_t read(embot::hw::I2CE b, ADR adr, Reg reg, embot::core::Data &destination, const embot::core::Callback &oncompletion) { return resNOK; }         
    result_t write(embot::hw::I2CE b, ADR adr, Reg reg, const embot::core::Data &content, const embot::core::Callback &oncompletion) { return resNOK; }     
    result_t receive(embot::hw::I2CE b, ADR adr, embot::core::Data &destination, const embot::core::Callback &oncompletion) { return resNOK; } 
    result_t transmit(I2CE b, ADR adr, const embot::core::Data &content, const embot::core::Callback &oncompletion) { return resNOK; }

}}} // namespace embot { namespace hw { namespace i2ce {

#elif defined(EMBOT_ENABLE_hw_i2ce)

namespace embot { namespace hw { namespace i2ce {
    
//    #warning marco.accame.says: Transaction::txbuffercapacity is limited to 16 so be careful we may use dynamic allocation

    struct Transaction
    {
        volatile bool ongoing {false};
        static constexpr uint8_t txbuffercapacity = 16;
        ADR addr {};
        embot::core::Data recdata {};
        std::uint8_t txbuffer[txbuffercapacity];
        std::uint8_t txbuffersize {0};
        embot::core::Callback oncompletion {};
        void clear() { ongoing = false; addr = 0; recdata.clear(); oncompletion.clear(); std::memset(txbuffer, 0, sizeof(txbuffer)); txbuffersize = 0;}    
        Transaction () { clear(); }        
    };
    
    // this struct contains the interface to the I2C driver. 
    // we may, later on, add the versions w/ callback...
    // so that we could use it for both DMA and GPIO driver types
    struct I2Cdriver
    {
        enum class TYPE { gpio = 0, dma = 1 };
        
        virtual TYPE type() const = 0;
        virtual void init(const embot::hw::GPIO &_scl, const embot::hw::GPIO &_sda) = 0;
        virtual void deinit() = 0;
        virtual bool ping(const ADR adr, const embot::core::relTime timeout = embot::core::reltimeWaitForever) = 0;    
        virtual bool read(const ADR adr, embot::core::Data &destination, const embot::core::relTime timeout = embot::core::reltimeWaitForever) = 0;
        virtual bool write(const ADR adr, const embot::core::Data &source, const embot::core::relTime timeout = embot::core::reltimeWaitForever) = 0;        
        
    protected:
        virtual ~I2Cdriver() {}
    };
    
    // this function gets the implementation of the I2C driver
    I2Cdriver * getI2Cdriver();
    // and this gives it back
    void releaseI2Cdriver(I2Cdriver *d);
        
    struct PrivateData
    {    
        Config config[embot::core::tointegral(I2CE::maxnumberof)] {};  
        I2Cdriver* driver[embot::core::tointegral(I2CE::maxnumberof)] {};
        Transaction transaction[embot::core::tointegral(I2CE::maxnumberof)] {};          
        PrivateData() = default;
    };
    
    // the utility functions (marked static)
    
     
    // the transaction is a sequence of operations such as:
    // - <write><read>: e.g., when we write <write(adr+regaddr)> and then we read <read(data)> back
    // - <write>: e.g., when we write <write(adr+regaddr+data)>
    // - <read>: e.g., when we just read data from a a device, even if we affectively write its addr first
    static result_t s_I2C_write_transaction_start(I2CE b, ADR adr, Reg reg, const embot::core::Data &content, const embot::core::Callback &oncompletion = embot::core::Callback(nullptr, nullptr));
    static result_t s_I2C_read_transaction_start(I2CE b, ADR adr, Reg reg, embot::core::Data &destination, const embot::core::Callback &oncompletion = embot::core::Callback(nullptr, nullptr));        
    static result_t s_I2C_wait_for_transaction_completion(I2CE b, embot::core::relTime timeout);   
    
    // these two are called when a tx or an rx is completed. 
    static void s_I2C_OnTXcompletion(embot::hw::I2CE id);
    static void s_I2C_OnRXcompletion(embot::hw::I2CE id);
    
    // and finally this one is called
    static void s_I2C_ontransactionterminated(std::uint8_t index);
      
    
    // the holder of the embot driver
    
    static PrivateData s_privatedata;
              
    // initialised mask       
    
    static std::uint32_t initialisedmask = 0; 
    
    // ----------------------------------------------------------------------------------------------------------------
    // the public functions
    // ----------------------------------------------------------------------------------------------------------------
   
    bool supported(embot::hw::I2CE p)
    {
        return embot::hw::i2ce::bsp::getBSP().supported(p);
    }

    
    bool initialised(embot::hw::I2CE p)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(p));
    }    
   
    
    result_t init(I2CE b, const Config &config)
    {
        if(false == supported(b))
        {
            return resNOK;
        }
        
        if(true == initialised(b))
        {
            return resOK;
        }        
        
        embot::hw::i2ce::bsp::getBSP().init(b, config);
        
        std::uint8_t index = embot::core::tointegral(b);
        s_privatedata.config[index] = config;
        
        // get the driver and init it w/ scl and sda
        s_privatedata.driver[index] = getI2Cdriver();        
        s_privatedata.driver[index]->init(embot::hw::i2ce::bsp::getBSP().getPROP(b)->scl(), embot::hw::i2ce::bsp::getBSP().getPROP(b)->sda());
        
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(b));
                
        return resOK;
    }
    
    result_t deinit(I2CE b)
    {
        if(false == supported(b))
        {
            return resNOK;
        }
        
        if(false == initialised(b))
        {
            return resOK;
        }        
        
        
        std::uint8_t index = embot::core::tointegral(b);
        //s_privatedata.config[index] = config;
        
        // get the driver and init it w/ scl and sda
        //s_privatedata.driver[index] = getI2Cdriver();        
        //s_privatedata.driver[index]->init(embot::hw::i2ce::bsp::getBSP().getPROP(b)->scl(), embot::hw::i2ce::bsp::getBSP().getPROP(b)->sda());
        
        s_privatedata.driver[index]->deinit();
        releaseI2Cdriver(s_privatedata.driver[index]);
        s_privatedata.driver[index] = nullptr;
        s_privatedata.config[index] = {};
        embot::hw::i2ce::bsp::getBSP().deinit(b);
        

        embot::core::binary::bit::clear(initialisedmask, embot::core::tointegral(b));
                
        return resOK;        
    }
 
    
    bool discover(embot::hw::I2CE b, std::vector<ADR> &adrs)
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
    
    result_t tx(embot::hw::I2CE b, ADR adr, const embot::core::Data &content, embot::core::relTime timeout)
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
        
        std::uint8_t index = embot::core::tointegral(b);
        
        // in here we just transmit over the bus. the call is blocking w/ timeout
        // we must nevertheless set the transaction ongoing before trasmission and clear it afterwards. 
                     
        s_privatedata.transaction[index].ongoing = true;    
        
        bool r = s_privatedata.driver[index]->write(adr, content, remaining);
        
        embot::hw::i2ce::s_privatedata.transaction[index].ongoing = false;
        
        return r ? resOK : resNOK;
    }        
    
    
    bool isbusy(I2CE b, embot::core::relTime timeout, embot::core::relTime &remaining)
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
        
    
    bool ping(I2CE b, ADR adr, embot::core::relTime timeout)
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
        
        std::uint8_t index = embot::core::tointegral(b);   

        
        s_privatedata.transaction[index].ongoing = true;    
                   
        bool r = s_privatedata.driver[index]->ping(adr, remaining);
        
        s_privatedata.transaction[index].ongoing = false;
        
        return r;       
    }


    result_t read(I2CE b, ADR adr, Reg reg, embot::core::Data &destination, embot::core::relTime timeout)
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
          
        // the read transaction requires either write<adr+reg>+read<data> or write<adr>+read<data>
        // the second case happens if reg is Reg::none
        result_t r = s_I2C_read_transaction_start(b, adr, reg, destination);
        
        
        if(resOK == r)
        {   // it transaction has started, we must wait for its completion
            r = s_I2C_wait_for_transaction_completion(b, remaining);
        }
        
        return r;                      
    }    
    

    result_t write(I2CE b, ADR adr, Reg reg, const embot::core::Data &content, embot::core::relTime timeout)
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
    
        result_t r = s_I2C_write_transaction_start(b, adr, reg, content);

        if(resOK == r)
        {   // it transaction has started, we must wait for its completion
            r = s_I2C_wait_for_transaction_completion(b, remaining);
        }
        
        return r;
    }
    
    
    result_t receive(I2CE b, ADR adr, embot::core::Data &destination, embot::core::relTime timeout)
    {
        constexpr Reg regnone { Reg::addressNONE, Reg::Size::none }; 
        // with addressNONE, inside read() the function s_I2C_read_transaction_start() starts a single-stage transaction using DMA
        return read(b, adr, regnone, destination, timeout);
    }    
        
    result_t transmit(embot::hw::I2CE b, ADR adr, const embot::core::Data &content, embot::core::relTime timeout)
    {
        constexpr Reg regnone { Reg::addressNONE, Reg::Size::none }; 
        // with addressNONE, inside read() the function s_I2C_read_transaction_start() starts a single-stage transaction using DMA
        return write(b, adr, regnone, content, timeout);       
    }
           
           
    
    // -
    // non-blocking mode
    
        
    bool isbusy(I2CE b)
    {
        if(false == initialised(b))
        {
            return false;
        } 
        return s_privatedata.transaction[embot::core::tointegral(b)].ongoing;     
    }

    
    result_t read(I2CE b, ADR adr, Reg reg, embot::core::Data &destination, const embot::core::Callback &oncompletion)
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
        result_t r = s_I2C_read_transaction_start(b, adr, reg, destination, oncompletion);
        return r;
    }    
    

    result_t write(I2CE b, ADR adr, Reg reg, const embot::core::Data &content, const embot::core::Callback &oncompletion)
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
        result_t r = s_I2C_write_transaction_start(b, adr, reg, content, oncompletion);
        return r;
    }    
    

    result_t receive(I2CE b, ADR adr, embot::core::Data &destination, const embot::core::Callback &oncompletion)
    {
        constexpr Reg regnone { Reg::addressNONE, Reg::Size::none };        
        // with addressNONE, inside read() the function s_I2C_read_transaction_start() starts a single-stage transaction using DMA
        return read(b, adr, regnone, destination, oncompletion);
    }  

    
    result_t transmit(I2CE b, ADR adr, const embot::core::Data &content, const embot::core::Callback &oncompletion)
    {
        constexpr Reg regnone { Reg::addressNONE, Reg::Size::none }; 
        // with addressNONE, inside read() the function s_I2C_read_transaction_start() starts a single-stage transaction using DMA
        return write(b, adr, regnone, content, oncompletion);       
    }    
 
 
    // ----------------------------------------------------------------------------------------------------------------
    // the utility functions (marked static)    
    // ----------------------------------------------------------------------------------------------------------------
    
            
    static result_t s_I2C_wait_for_transaction_completion(I2CE b, embot::core::relTime timeout)
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
   
    
    static result_t s_I2C_write_transaction_start(I2CE b, ADR adr, Reg reg, const embot::core::Data &content, const embot::core::Callback &oncompletion)
    { 
        result_t res = resNOK; 
        
        std::uint8_t index = embot::core::tointegral(b);  

        // we prepare and lock the transaction        
        s_privatedata.transaction[index].addr = adr;
        s_privatedata.transaction[index].oncompletion = oncompletion;
        s_privatedata.transaction[index].ongoing = true;
        // IMPORTANT: invalide recdata, so that s_I2C_OnTXcompletion() / HAL_I2C_MasterTxCpltCallback() knows it is a write transaction
        s_privatedata.transaction[index].recdata.clear(); 
        
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
            {   // i append content after the first byte
                s_privatedata.transaction[index].txbuffer[0] = reg.address & 0xff;
                std::memmove(&s_privatedata.transaction[index].txbuffer[1], content.pointer, content.capacity);
                s_privatedata.transaction[index].txbuffersize = content.capacity + 1;                
            }
            else
            {   // i append content after the second byte
                s_privatedata.transaction[index].txbuffer[0] = (reg.address >> 8 ) & 0xff;
                s_privatedata.transaction[index].txbuffer[1] = reg.address & 0xff;                
                std::memmove(&s_privatedata.transaction[index].txbuffer[2], content.pointer, content.capacity);
                s_privatedata.transaction[index].txbuffersize = content.capacity + 2;
            }
        }
        
        // and now i transmit the embot::core::Data formed by {txbuffer, txbuffersize}
        
        embot::core::Data data2tx {s_privatedata.transaction[index].txbuffer, s_privatedata.transaction[index].txbuffersize};
        bool r = s_privatedata.driver[index]->write(adr, data2tx);
        res = r ? resOK : resNOK;
        
        // for the case of TYPE::gpio the end of the writing is now...
        if(true == r)
        {
            s_I2C_OnTXcompletion(b);
        }
        
        // that is required if any read() / write() failed. because in such a case no call is done to terminate the transaction
        if(resOK != res)
        {
            s_privatedata.transaction[index].ongoing = false;
        }  

 
        return res;
    }        


    static result_t s_I2C_read_transaction_start(I2CE b, ADR adr, Reg reg, embot::core::Data &destination, const embot::core::Callback &oncompletion)
    {
        result_t res = resNOK;
        
        std::uint8_t index = embot::core::tointegral(b);
        
        // we prepare and lock the transaction
        s_privatedata.transaction[index].addr = adr;
        s_privatedata.transaction[index].oncompletion = oncompletion;
        s_privatedata.transaction[index].ongoing = true;
        // IMPORTANT: recdata is NOT invalid so that s_I2C_OnTXcompletion() / HAL_I2C_MasterTxCpltCallback() (if called) knows it is a read transaction
        s_privatedata.transaction[index].recdata = destination;  
        
        if(reg.address == Reg::addressNONE)
        {
            // it is a single-stage operation: read<values>
            // i dont need to ask which register to read from because the slave already knows what to send.
            // i do the read<> request with driver->read() which behaves differently according to its TYPE.
            // - TYPE::dma: it uses HAL_I2C_Master_Receive_DMA(recdata); the completion of reading operation 
            //   is signalled by the call of HAL_I2C_MasterRxCpltCallback(). at this stage recdata will contain 
            //   the received data. and the callback calls the likes of s_I2C_OnRXcompletion();
            // - TYPE::gpio: it is a blocking operation ... still w/ WIP. 
            //   So far, after driver->read() we call s_I2C_OnRXcompletion(); this one calls s_I2C_ontransactionterminated()
            //   which unlocks the transaction and calls transaction[index].oncompletion.execute().            

            bool r = s_privatedata.driver[index]->read(adr, destination);
            
            if(true == r)
            {
                s_I2C_OnRXcompletion(b);
            }                
            
            res = r ? resOK : resNOK;
        }
        else
        {
            // it is a two-stage operation: write<reg>, read<values>
            // i must at first tx the value of the register i want to read from. i use driver->write()
            // - TYPE::dma: i use HAL_I2C_Master_Transmit_DMA(). the callback of HAL_I2C_MasterTxCpltCallback() signals 
            //   the end of the fist stage and ... if recdata.isvalid() then i perform the read<> request with 
            //   HAL_I2C_Master_Receive_DMA(recdata). the completion of this second stage (the reading operation) is 
            //   signalled by the callback HAL_I2C_MasterRxCpltCallback(). at this stage recdata will contain the 
            //   received dat. and the callback calls the likes of s_I2C_OnRXcompletion()
            // - TYPE:gpio: it is a blocking operation ... still w/ WIP.
            //   So far, after driver->write() we call s_I2C_OnTXcompletion(); this one will trigger the second stage in blocking
            //   mode and will finally call s_I2C_OnRXcompletion(); this one calls s_I2C_ontransactionterminated()
            //   which unlocks the transaction and calls transaction[index].oncompletion.execute(). 
                    
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
            
            // marco.accame: i must transmit the bytes of the address. when over, i call s_I2C_OnTXcompletion()
            // which starts to receive data. and then calls s_I2C_OnRXcompletion() ...
            bool r = s_privatedata.driver[index]->write(adr, {s_privatedata.transaction[index].txbuffer, s_privatedata.transaction[index].txbuffersize});
            
            if(true == r)
            {
                s_I2C_OnTXcompletion(b);
            }
            
            res = r ? resOK : resNOK;
        }

        // that is required if any read() / write() failed. because in such a case no call is done to terminate the transaction
        if(resOK != res)
        {
            s_privatedata.transaction[index].ongoing = false;
        }
        
        
        return res;
    } 
    

    static void s_I2C_ontransactionterminated(std::uint8_t index)
    {
        s_privatedata.transaction[index].ongoing = false;        
        s_privatedata.transaction[index].oncompletion.execute();                                
    }   

    
// it behaves similarly to HAL_I2C_MasterTxCpltCallback() for the case of TYPE::dma
// was HAL_I2C_MasterTxCpltCallback()   
static void s_I2C_OnTXcompletion(embot::hw::I2CE id)
{          
// dont need this check because we call it only from inside this driver always w/ safe values of id    
//    if(embot::hw::I2CE::none == id)
//    {
//        return;
//    }
    
    std::uint8_t index = embot::core::tointegral(id);
                   
    if(false == s_privatedata.transaction[index].recdata.isvalid())
    {
        // it is a write operation: the transaction is terminated
        s_I2C_ontransactionterminated(index);        
    }
    else
    {
        // it is a read operation w/ previous writing of the reg address, so i still need to receive data
        s_privatedata.driver[index]->read(s_privatedata.transaction[index].addr, s_privatedata.transaction[index].recdata);
        // and after that i need to call what releases the RX transaction
        s_I2C_OnRXcompletion(id);
    }
    
}  


// it behaves similarly to HAL_I2C_MasterRxCpltCallback() for the case of TYPE::dma
static void s_I2C_OnRXcompletion(embot::hw::I2CE id)
{  
// dont need this check because we call it only from inside this driver always w/ safe values of id    
//    if(embot::hw::I2CE::none == id)
//    {
//        return;
//    }
    
    std::uint8_t index = embot::core::tointegral(id);    
    s_I2C_ontransactionterminated(index);        
}        
     
}}} // namespace embot { namespace hw { namespace i2ce {



namespace embot { namespace hw { namespace i2ce {
    
    
    // logging features
    
    
//    #define ENABLE_i2clogger

#if defined(ENABLE_i2clogger)
    
    struct I2Clogger
    {
        struct Sample
        {
            uint32_t delta {0};         // time in usec after previous sample
            uint8_t sclvalue {0};       // binary value of scl
            uint8_t sdavalue {0};       // binary value of sda
            bool sdainput {false};      // boolean value tellin if the sda in input to the master 
            bool delaycalled {false};   // delay just called
            bool sdasampled {false};    // sda just sampled
            void clear()   
            {
                delta = 0;
                sclvalue = sdavalue = 0;
                sdainput = delaycalled = sdasampled = false;
            }
            
            void load(uint32_t d, uint8_t sclv, uint8_t sdav, bool inp, bool del, bool sdas)
            {
                delta = d;
                sclvalue = sclv;
                sdavalue = sdav;
                sdainput = inp;
                delaycalled = del;
                sdasampled = sdas;
            }
        };
    
        embot::core::Time timeofstart {0};
        static constexpr size_t capacity {2048};
        size_t size {0};
        std::array<Sample, capacity> samples {};
            
        volatile uint8_t Vscl {0};      // latest know binary value of SCL
        volatile uint8_t Vsda {0};      // latest know binary value of SDA
        volatile bool Isda {false};     // if true the SDA is input to the master
        volatile bool delaynow {false}; // if true we have just applied a delay
        volatile bool sampled {false};  // if true we have just sampled the sda value as driven by the slave
        
        uint32_t delta {0};
        
        embot::hw::GPIO sclgpio {};     // the gpio of teh scl
        embot::hw::GPIO sdagpio {};     // teh gpio of the sda
        
        void start(const embot::hw::GPIO &_scl, const embot::hw::GPIO &_sda)
        {
            sclgpio = _scl;
            sdagpio = _sda;
            
            Vscl = Vsda = size = 0;
            Isda = delaynow = sampled = false;
            for(auto &i : samples) i.clear();
            timeofstart = embot::core::now();
        }
        
        void add(uint32_t t)
        {
            readsda();
            samples[size].load(t, Vscl, Vsda, Isda, delaynow, sampled);
            size++;                
        }

        bool cantouch()
        {
            if(true == size < (capacity-2))
            {
                delta = embot::core::now() - timeofstart;
                return true;
            }
            return false;
        }   

        void readsda()
        {
            if(true == Isda)
            {
                embot::hw::gpio::State s = embot::hw::gpio::get(sdagpio);
                Vsda = (embot::hw::gpio::State::RESET == s) ? 0 : 1;
            }
        }                

        void delay()
        {
            if(!cantouch())
            {
                return;
            }
            
            delaynow = false;
            add(delta-1);
            delaynow = true;
            add(delta);
            delaynow = false;
            add(delta+1);
        }
        
        void sample()
        {
            if(!cantouch())
            {
                return;
            }
            sampled = false;
            add(delta-1);
            sampled = true;
            add(delta);
            sampled = false;
            add(delta+1);                                
        }
        
        void scl(bool on)
        {
            if(!cantouch())
            {
                return;
            }
            add(delta-1);
            Vscl = on ? 1 : 0;
            add(delta);               
        }   

        void sda(bool on)
        {
            if(!cantouch())
            {
                return;
            }
            add(delta-1);
            Vsda = on ? 1 : 0;
            add(delta);               
        }            

        void sdainput(bool input)
        {
            if(!cantouch())
            {
                return;
            }
            add(delta-1);
            Isda = input;
            add(delta);               
        }

        char str[256] {0};

        void report()
        {
            constexpr embot::core::relTime de = 100;
            volatile uint32_t index {0};
            snprintf(str, sizeof(str), "size: %d", size);
            embot::core::print(str);
            for(uint32_t i=0; i<size; i++)
            {
                snprintf(str, sizeof(str), "%d, %d, %d, %d, %d, %d, %d", i, samples[i].delta, samples[i].sclvalue, samples[i].sdavalue, samples[i].sdainput, samples[i].delaycalled, samples[i].sdasampled);
                embot::core::print(str);
                index ++;
                if(0 == (index%25))
                {
                    index = index;
                    embot::core::wait(de);
                }
            }
            
            embot::core::wait(de);

        }                
        
    };
    
#endif
    

    // the struct 2Cdriver1 implements a GPIO-based driver as explained by
    // AN12841: GPIO Simulated I2C for S08, Rev. 0 — May 2020
    // status @ 29 dec 2022: still WIP
    // so far only the ping() works.
    // the read() works until i write the slave address and the master gets its ack.
    // see comments in WIP-note-001 and WIP-note-002
    
    struct I2Cdriver1 : public I2Cdriver
    {
        static constexpr bool simulateACKfromslave {false};

#if defined(ENABLE_i2clogger)       
        static constexpr bool enablelogger {true}; // or a macro ...
        I2Clogger *i2clogger {nullptr};         
#endif
        
        enum class Value { L = 0, H = 1 };
        enum class Mode { OUT = 0, INP = 1 };
        
        TYPE type() const override
        {
            return TYPE::gpio;
        }
        
        void init(const embot::hw::GPIO &_scl, const embot::hw::GPIO &_sda) override
        {
            scl = _scl;
            sda = _sda;
            
            embot::hw::gpio::init(scl, outcfg);
            embot::hw::gpio::set(scl, embot::hw::gpio::State::SET);
            
            embot::hw::gpio::init(sda, outcfg);
            embot::hw::gpio::set(sda, embot::hw::gpio::State::SET);    

#if defined(ENABLE_i2clogger)            
            if(true == enablelogger)
            {
                i2clogger = new I2Clogger;
            }
#endif            
        }
        
        void deinit() override
        {
            embot::hw::gpio::deinit(scl);
            embot::hw::gpio::deinit(sda);
            scl = {};
            sda = {};  

#if defined(ENABLE_i2clogger) 
            if(true == enablelogger)
            {
                delete i2clogger;
            }
#endif             
        }
         
        bool ping(const ADR adr, const embot::core::relTime timeout = embot::core::reltimeWaitForever) override 
        {
            bool r {false};
            
            uint8_t wadr = adr | 0; 
            
            I2Cstart();            
            r = I2Cwrite(wadr);
            
            I2Cstop();
            
            return r;                    
        }
             

            
        bool read(const ADR adr, embot::core::Data &destination, const embot::core::relTime timeout = embot::core::reltimeWaitForever) override
        {
#if defined(ENABLE_i2clogger) 
            if(enablelogger)
            {
                i2clogger->start(scl, sda);
            }
#endif
            
            bool r {false};
            
            uint8_t radr = adr | 1;
            I2Cstart();
            
            // WIP-note-001
            // marco.accame: the original code of the AN12841 is executed if i dont pass true as extra argument to I2Cwrite()
            // original code drives SDA OUT-HIGH for a bit just after reading teh ACK from slave ... and i dont like that.
            // so, i have tried keep it INP. I do that by calling r = I2Cwrite(radr, true) .... but that is still to be tested 
            // some more because it does not solve.
            
            //r = I2Cwrite(radr, false);  
            r = I2Cwrite(radr, true);
            
            if(false == r)
            {
                return r;
            }
            
            // WIP-note-002
            // marco.accame: the read() is correct until here. the master receives the ACK from the slave.
            // but after that ... the function I2Cread() reads only 0xFF.
            // so, we must debug the driver from here.
            
            uint8_t *data = destination.getU08ptr();
            uint8_t n = destination.capacity;
            for(uint8_t k=0; k<n; k++)
            {
                I2Cread(&data[k]);
                
                if(k == (n-1))
                {
                    I2Cnack();
                    I2Cstop();
                    break;            
                }
                else
                {
                    I2Cack();
                }
                
                delay();        
            }  

#if defined(ENABLE_i2clogger) 
            if(enablelogger)
            {
                i2clogger->report();
            }
#endif
            
            return r;             
        }
        
        bool write(const ADR adr, const embot::core::Data &source, const embot::core::relTime timeout = embot::core::reltimeWaitForever) override
        {
            bool r {false};    
            
            uint8_t wadr = adr | 0;
            I2Cstart();
            
            r = I2Cwrite(wadr);
            
            if(false == r)
            {
                return r;
            }
            uint8_t *data = source.getU08ptr();
            uint8_t n = source.capacity;            
            for(uint8_t k=0; k<n; k++)
            {       
                r = I2Cwrite(data[k]);     
            }
          
            I2Cstop();              
            return r;             
        }
        
    protected:
        
        embot::hw::GPIO scl {};
        embot::hw::GPIO sda {};
            
        static constexpr embot::core::relTime us {1};
        
        static constexpr embot::hw::gpio::Config outcfg 
        { 
            embot::hw::gpio::Mode::OUTPUTpushpull, 
            embot::hw::gpio::Pull::pullup, 
            embot::hw::gpio::Speed::veryhigh 
        };

        static constexpr embot::hw::gpio::Config inpcfg
        { 
            embot::hw::gpio::Mode::INPUT, 
            embot::hw::gpio::Pull::pullup, 
            embot::hw::gpio::Speed::veryhigh 
        }; 
        
        void delay()
        {
#if defined(ENABLE_i2clogger) 
            if(enablelogger)
            {
                i2clogger->delay();
            }
#endif          
            embot::hw::sys::delay(us);
        }
        
        void SCL(Value v) 
        {            
#if defined(ENABLE_i2clogger) 
            if(enablelogger)
            {
                i2clogger->scl(embot::core::tointegral(v));
            }
#endif
            embot::hw::gpio::set(scl, static_cast<embot::hw::gpio::State>(v));         
        }
        
        void SDA(Value v) 
        {
#if defined(ENABLE_i2clogger) 
            if(enablelogger)
            {
                i2clogger->sda(embot::core::tointegral(v));
            }
#endif                     
            embot::hw::gpio::set(sda, static_cast<embot::hw::gpio::State>(v));
        }
        
        void SDA(Mode m) 
        {
#if defined(ENABLE_i2clogger) 
            if(enablelogger)
            {
                i2clogger->sdainput((Mode::INP == m) ? true : false);
            }
#endif
            
            embot::hw::gpio::deinit(sda);
            embot::hw::gpio::init(sda, (Mode::INP == m) ? inpcfg : outcfg);
            
//            /* disable PTB6(SDA) output, enable PTB6(SDA) input */
//            i2c_sda_output_off(sda);
//            i2c_sda_input(sda);   

//            /* disable SDA input, enable SDA output */
//            i2c_sda_input_off(sda);
//            i2c_sda_output(sda);    
            
        }
        
        Value SDA() 
        {
            embot::hw::gpio::State s = embot::hw::gpio::get(sda);
            
#if defined(ENABLE_i2clogger) 
            if(enablelogger)
            {
                i2clogger->sample();
                i2clogger->sda(embot::core::tointegral(s));
            }       
#endif
            
            return static_cast<Value>(s);
        }
        
        
        void I2Cstart() 
        {
            SCL(Value::L);
            SDA(Value::H);
            SCL(Value::H);
            delay();
             /* SDA change from high to low when SCL is high, i2c start */
            SDA(Value::L);
//            i2c_scl_low();
//            i2c_sda_high(sda);
//            i2c_scl_high();
//            i2c_scl_delay();
//            /* SDA change from high to low when SCL is high, i2c start */
//            i2c_sda_low(sda);            
        }
        
        void I2Cstop() 
        {
            SCL(Value::L);
            SDA(Value::L);
            SCL(Value::H);
            delay();
            /* SDA change from low to high when SCL is high, i2c stop */
            SDA(Value::H);
//            i2c_scl_low();
//            i2c_sda_low(sda);
//            i2c_scl_high();
//            i2c_scl_delay();
//            /* SDA change from low to high when SCL is high, i2c stop */
//            i2c_sda_high(sda);            
        }

        bool I2Cacked(bool reading) 
        {
            bool r = false;
            
            /* the 9th clock start */
            SCL(Value::L);
            SDA(Mode::INP);
            
            delay();
            SCL(Value::H);
            /* read ACK signal when SCL is high */
            //// maybe a small wait????
            Value v = SDA(); 
            r = (Value::L == v) ? true : false;
            delay();
            /* the 9th clock end */
 
// are u sure it is really required teh code from A to B?
// A -> 

            if(!reading)
            {
            /* disable SDA input, enable SDA output */
//            #warning ... maybe in reading i should not move back sda to output in here ......
            SDA(Mode::OUT);
            }
            
            /* SCL hold low to wait */
            SCL(Value::L);
            if(!reading)
            {
//            #warning ... BUG???? SURELY in reading i should not drive sda to high in here ......
            SDA(Value::H);
            }
// <- B            
            return r;
          
            
        }
        
        bool I2Cacked() 
        {
            bool r = false;
            
            /* the 9th clock start */
            SCL(Value::L);
            SDA(Mode::INP);
            
            delay();
            SCL(Value::H);
            /* read ACK signal when SCL is high */
            //// maybe a small wait????
            Value v = SDA(); 
            r = (Value::L == v) ? true : false;
            delay();
            /* the 9th clock end */
 
// are u sure it is really required teh code from A to B?
// A ->           
            /* disable SDA input, enable SDA output */
//            #warning ... maybe in reading i should not move back sda to output in here ......
            SDA(Mode::OUT);
            
            /* SCL hold low to wait */
            SCL(Value::L);
//            #warning ... BUG???? SURELY in reading i should not drive sda to high in here ......
            SDA(Value::H);
// <- B            
            return r;
            
//            embot::hw::gpio::State  ack;
//            /* the 9th clock start */
//            i2c_scl_low();

//            /* disable PTB6(SDA) output, enable PTB6(SDA) input */
//            i2c_sda_output_off(sda);
//            i2c_sda_input(sda);

//            i2c_scl_delay();
//            i2c_scl_high();
//            /* read ACK signal when SCL is high */
//            ack = i2c_sda_status(sda);
//            i2c_scl_delay();
//            /* the 9th clock end */

//            /* disable SDA input, enable SDA output */
//            i2c_sda_input_off(sda);
//            i2c_sda_output(sda);

//            /* SCL hold low to wait */
//            i2c_scl_low();
//            i2c_sda_high(sda);
//            return ack;            
            
        }
        
        bool I2Cwrite(uint8_t byte, bool reading = false) 
        {
            bool r = true;
            
            for(uint8_t i=0; i<8; i++) 
            {
                SCL(Value::L);
                delay();
                /* Data can be changed only while SCL is low */
                uint8_t bit = byte >> 7; 
                SDA(static_cast<Value>(bit));
                /* Data must be held stable while SCL is high */
                SCL(Value::H);
                delay();
                // prepare byte for next iteration
                byte = byte << 1;                
            }
            
            /* Must set SCL to low. If SCL is high, SDA change to High/Low will cause Stop/Start signal.*/
            SCL(Value::L);
            /* check ACK signal, ACK signal is 0, NACK signal is 1 */
            if(false == I2Cacked(reading))
            {
                if(!simulateACKfromslave)
                {
                    /* receive NACK signal,stop data transfer */
                    I2Cstop();
                    r = false;
                }
            }
            return r;
            
//            bool ret = true;
//            unsigned char i;
//            /* write 8 bits data */
//            for(i=0;i<8;i++)
//            {
//                i2c_scl_low();
//                i2c_scl_delay();

//                /* Data can be changed only while SCL is low */
//                if(data & 0x80)
//                {
//                    i2c_sda_high(sda1);
//                }
//                else
//                {
//                    i2c_sda_low(sda1);
//                }

//                /* Data must be held stable while SCL is high */
//                i2c_scl_high();
//                i2c_scl_delay();
//                data = data<<1;
//            }
//            /* Must set SCL to low. If SCL is high, SDA change to High/Low will cause Stop/Start signal.*/
//            i2c_scl_low();
//            /* check ACK signal, ACK signal is 0, NACK signal is 1 */
//            if(i2c_read_ACK(sda1) == embot::hw::gpio::State::SET)
//            {
//                /* receive NACK signal,stop data transfer */
//                i2c_stop(sda1);
//                ret = false;
//            //        while(1);
//            }

//            return ret;           
                      
        }   

        bool I2Cread(uint8_t *val)
        {
            bool r = true;
            
            uint8_t data = 0;

            /* disable PTB6(SDA) output, enable PTB6(SDA) input */
            //i2c_sda_output_off(sda1);
            //i2c_sda_input(sda1);
            SDA(Mode::INP);
//            #warning rimuovere il delay qui di seguito
            //i2c_scl_delay(); // messo io: 
//            delay(); // messo io
             /* write 8 bits data */
            for(uint8_t i=0;i<8;i++)
            {
                //i2c_scl_low();
                SCL(Value::L);
                //i2c_scl_delay();
                delay();
                /* Data must be held stable while SCL is high */
                //i2c_scl_high();
                SCL(Value::H);
                //i2c_scl_delay(); // messo io
                //i2c_scl_delay(); // messo io
                data = data << 1;
                /* read SDA data */
//                if(i2c_sda_status(sda1) == embot::hw::gpio::State::SET)
//                {
//                    data |= 0x01;
//                }
//                else
//                {
//                    data &= ~0x01;
//                }
                if(Value::H == SDA())
                {
                    embot::core::binary::bit::set(data, 0);
                }
                else
                {
                    embot::core::binary::bit::clear(data, 0);
                }
                
                // i2c_scl_delay();
                delay();
            }
            
#define CORRECT

#if !defined(CORRECT)            
            
            #warning: io qui farei prima scl low, poi (delay??) e poi sda out ed uguale a low
            /* disable PTB6(SDA) input, enable PTB6(SDA) output */
            //i2c_sda_input_off(sda1);
            //i2c_sda_output(sda1);
            SDA(Mode::OUT);
            
         //   #warning BLOODY HELL: does it miss the ACK to the slave

            //i2c_scl_low(); // set SCL to low
            SCL(Value::L);
#else
            SCL(Value::L); 
           
            SDA(Mode::OUT);
            
#endif            
            *val = data;
         
            return r;
        }  
        
        void I2Cack()
        {
            /* the 9th clock start */
            //i2c_scl_low();
            SCL(Value::L);
            /*send ACK signal */
            //i2c_sda_low(sda1);
            SDA(Value::L);
            //i2c_scl_delay();
            delay();
            //i2c_scl_high();
            SCL(Value::H);
            //i2c_scl_delay();
            delay();
            /* the 9th clock start */
            /* SCL hold low to wait */
            //i2c_scl_low();
            SCL(Value::L);
            //i2c_sda_high(sda1);    
//            #warning ... io non metterei qui SDA ad H. lo metterei in input
//            SDA(Value::H);
        }

        
        void I2Cnack()
        {
            /* the 9th clock start*/
            //i2c_scl_low();
            SCL(Value::L);
            /*send NACK signal */
            //i2c_sda_high(sda1);
            SDA(Value::H); 
            //i2c_scl_delay();
            delay();
            //i2c_scl_high();
            SCL(Value::H);
            //i2c_scl_delay();
            delay();
            /* the 9th clock start */
            /* SCL hold low to wait */
            //i2c_scl_low();
            SCL(Value::L);
            //i2c_sda_high(sda1);  
            #warning verificare che qui sia corretto sda messo high ??
            SDA(Value::H);            
        }   
        
    };
    
    I2Cdriver * getI2Cdriver()
    {
        return new I2Cdriver1;
    }
    
    void releaseI2Cdriver(I2Cdriver *d)
    {
        I2Cdriver1 *p = reinterpret_cast<I2Cdriver1*>(d);       
        delete p;
    }
    
    
}}} // namespace embot { namespace hw { namespace i2ce {


#endif //#elif defined(EMBOT_ENABLE_hw_i2ce)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

