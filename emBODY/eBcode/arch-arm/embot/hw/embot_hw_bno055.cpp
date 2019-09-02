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

#include "embot_hw_bno055.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include "stm32hal.h"
#include "embot_binary.h"
#include "embot_hw_sys.h"
#include "embot_hw_gpio.h"
#include "embot_hw_bsp.h"

using namespace std;

using namespace embot::hw;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#define EMBOT_BNO055_ENABLED

#if     !defined(EMBOT_BNO055_ENABLED)


namespace embot { namespace hw { namespace bno055 {

    bool supported(BNO055 s)                                                                        { return false; }
    bool initialised(BNO055 s)                                                                      { return false; }
    result_t init(BNO055 s, const Config &config)                                                   { return resNOK; }
    bool isalive(BNO055 s, embot::common::relTime timeout)                                          { return false; }
    result_t get(BNO055 s, Info &info, embot::common::relTime timeout)                              { return resNOK; }
    result_t set(BNO055 s, Mode m, embot::common::relTime timeout)                                  { return resNOK; }
    bool isacquiring(BNO055 s)                                                                      { return false; }
    bool canacquire(BNO055 s)                                                                       { return false; }
    result_t acquisition(BNO055 s, Set set, const embot::common::Callback &oncompletion)            { return resNOK; }
    bool operationdone(BNO055 s)                                                                    { return false; }
    result_t read(BNO055 s, Data &data)                                                             { return resNOK; }
    result_t write(BNO055 s, embot::hw::bno055::Register reg, std::uint8_t value, const embot::common::relTime timeout){ return resNOK; }
    result_t write(BNO055 s, embot::hw::bno055::Register reg, std::uint8_t value, const embot::common::Callback &oncompletion){ return resNOK; }
    result_t read(BNO055 s, embot::hw::bno055::Register reg, embot::utils::Data &data, const embot::common::relTime timeout){ return resNOK; }
    result_t read(BNO055 s, embot::hw::bno055::Register reg, embot::utils::Data &data, const embot::common::Callback &oncompletion){ return resNOK; }
    
}}} // namespace embot { namespace hw { namespace BNO055 {


#elif   defined(EMBOT_BNO055_ENABLED)


namespace embot { namespace hw { namespace bno055 {
        
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(BNO055 s)
    {
        return embot::hw::bsp::bno055::getBSP().supported(s);
    }
    
    bool initialised(BNO055 s)
    {
        return embot::binary::bit::check(initialisedmask, embot::common::tointegral(s));
    }     

    struct Acquisition
    {
        volatile bool done;
        volatile bool ongoing;
        std::uint8_t buffer[48];
        embot::utils::Data data;
        embot::common::Callback userdefCBK;  
        Acquisition() { clear(); }
        void clear() { data.load(buffer, 0); done = false; ongoing = false; std::memset(buffer, 0, sizeof(buffer)); userdefCBK.clear(); }         
        void startread(std::uint8_t size, const embot::common::Callback &cbk) { clear(); data.size = size; userdefCBK = cbk; ongoing = true; }
        void startread(std::uint8_t size) { clear(); data.size = size; ongoing = true; }
        void startread(embot::utils::Data &da, const embot::common::Callback &cbk) { clear(); data = da; userdefCBK = cbk; ongoing = true; }
        void startread(embot::utils::Data &da) { clear(); data = da; ongoing = true; }
        void startwrite(std::uint8_t val) { clear(); buffer[0] = val; data.size = 1; ongoing = true; }
        void startwrite(std::uint8_t val, const embot::common::Callback &cbk) { clear(); buffer[0] = val; data.size = 1; userdefCBK = cbk; ongoing = true; }
        void stop() { ongoing = false; done = true; userdefCBK.execute(); }
    };
    
    struct PrivateData
    {    
        std::uint8_t i2caddress[embot::common::tointegral(BNO055::maxnumberof)];
        Config config[embot::common::tointegral(BNO055::maxnumberof)];        
        Acquisition acquisition[embot::common::tointegral(BNO055::maxnumberof)];
        PrivateData() { }
    };
    
    // power-on-reset wait time. 650 ms is enough, datasheet say, before the device can talk over i2c. but much better to be devil-compliant ;-).
    static const embot::common::relTime PORtime = 666*embot::common::time1millisec;
    
//    static const std::uint8_t i2caddress = 0x52;
//    #if     defined(STM32HAL_BOARD_STRAIN2)
//        static const embot::hw::GPIO gpioBOOT(BNO055_BOOT_GPIO_Port, BNO055_BOOT_Pin);
//        static const embot::hw::GPIO gpioRESET(BNO055_RESET_GPIO_Port, BNO055_RESET_Pin);
//    #elif   defined(STM32HAL_BOARD_MTB4)
//        static const embot::hw::GPIO gpioBOOT(BNO055_BOOT_GPIO_Port, BNO055_BOOT_Pin);
//        static const embot::hw::GPIO gpioRESET(BNO055_RESET_GPIO_Port, BNO055_RESET_Pin);  
//    #elif   defined(STM32HAL_BOARD_RFE)
//        static const embot::hw:::GPIO gpioBOOT(BNO055_BOOT_GPIO_Port, BNO055_BOOT_Pin);
//        static const embot::hw:::GPIO gpioRESET(BNO055_RESET_GPIO_Port, BNO055_RESET_Pin);     
//    #endif
    
    static PrivateData s_privatedata;
     
    static void s_powerON(BNO055 s, embot::common::relTime waittime);    
    static result_t s_programregister(BNO055 s, std::uint8_t reg, std::uint8_t val, embot::common::relTime timeout);    
    static result_t s_writeregister(BNO055 s, std::uint8_t reg, std::uint8_t val, const embot::common::Callback &oncompletion);
    static void s_sharedCBK(void *p);
    
            
    result_t init(BNO055 s, const Config &config)
    {
        if(false == supported(s))
        {
            return resNOK;
        }
        
        if(true == initialised(s))
        {
            return resOK;
        }
        
        // init peripheral
        embot::hw::bsp::bno055::getBSP().init(s);
        
        // power on and wait until the i2c is working. datasheet say: POR time > 650 ms.
        s_powerON(s, PORtime);
        
        std::uint8_t index = embot::common::tointegral(s);
        
                        
        // init i2c ..
        embot::hw::i2c::init(config.i2cdes.bus, config.i2cdes.config);
        
        if(false == embot::hw::i2c::ping(config.i2cdes.bus, embot::hw::bsp::bno055::getBSP().getPROP(s)->i2caddress, 3*embot::common::time1millisec))
        {
            return resNOK;
        }
        
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].clear();
        s_privatedata.i2caddress[index] = embot::hw::bsp::bno055::getBSP().getPROP(s)->i2caddress;
        
        embot::binary::bit::set(initialisedmask, embot::common::tointegral(s));
                
        return resOK;
    }



    bool isalive(BNO055 s, embot::common::relTime timeout)
    {
        if(false == initialised(s))
        {
            return false;
        } 
        std::uint8_t index = embot::common::tointegral(s);
        return embot::hw::i2c::ping(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], timeout);  
    }        


    
    result_t get(BNO055 s, Info &info, embot::common::relTime timeout)
    {
        embot::utils::Data data(&info, sizeof(info));
        result_t r = read(s, embot::hw::bno055::Register::CHIP_ID, data, timeout);  
        return r;        
    }
    
    
    result_t set(BNO055 s, Mode m, embot::common::relTime timeout)
    { 
        if(false == initialised(s))
        {
            return resNOK;
        }  
        
        if((Mode::none == m) || (Mode::maxnumberof == m))
        {
            return resNOK;
        }
               
        return write(s, Register::OPR_MODE, static_cast<std::uint8_t>(m), timeout);                                       
    } 


    bool isacquiring(BNO055 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::common::tointegral(s);        
        return s_privatedata.acquisition[index].ongoing;     
    }    
    
        
    bool canacquire(BNO055 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::common::tointegral(s);  
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return false;
        }
        
        return !embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus);             
    }
    
    
    
    result_t acquisition(BNO055 s, Set set, const embot::common::Callback &oncompletion)
    {        
        if(false == canacquire(s))
        {
            return resNOK;
        }
        
        // ok, start a read of nbytes which will go into buffer[]
        const std::uint8_t nbytes = static_cast<std::uint8_t>(set);
        const Register reg = Register::DATASET_START;
        
        std::uint8_t index = embot::common::tointegral(s);
        
        s_privatedata.acquisition[index].startread(nbytes, oncompletion); 
                       
        // now i trigger i2c.
        embot::common::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);
        return embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], static_cast<std::uint8_t>(reg), s_privatedata.acquisition[index].data, cbk);        
    } 
    

    
    result_t acquisition(BNO055 s, Set set, Data &data, const embot::common::Callback &oncompletion)
    {        
        if(false == canacquire(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::common::tointegral(s);
        
        const std::uint8_t nbytes = static_cast<std::uint8_t>(set);        
        // ok, start a read of nbytes only (not all sizeof(data)) which will go into data
        embot::utils::Data da(&data, nbytes);
                
        s_privatedata.acquisition[index].startread(da, oncompletion); 
                       
        // now i trigger i2c.
        embot::common::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);
        return embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], static_cast<std::uint8_t>(Register::DATASET_START), s_privatedata.acquisition[index].data, cbk);        
    } 
    
    
    bool operationdone(BNO055 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        return s_privatedata.acquisition[embot::common::tointegral(s)].done;        
    } 

    
    result_t read(BNO055 s, Data &data)
    { 
        if(false == initialised(s))
        {
            return resNOK;
        } 

        if(false == operationdone(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::common::tointegral(s);
        data.load(s_privatedata.acquisition[index].data.pointer);
        
        return resOK;   
    } 


    result_t write(BNO055 s, embot::hw::bno055::Register reg, std::uint8_t value, const embot::common::relTime timeout)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 
        
        std::uint8_t index = embot::common::tointegral(s);
        
        // i2c must not be used by another device (eg. termometer or ...).
        embot::common::relTime remaining = timeout;
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus, timeout, remaining))
        {
            return resNOK;
        }
        
        // if (when) i2c is not busy (anymore), we just hope there is no another task using the bno055 ...
        // we however just do the check         
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        return s_programregister(s, static_cast<std::uint8_t>(reg), value, remaining);                         
    }

    
    result_t write(BNO055 s, embot::hw::bno055::Register reg, std::uint8_t value, const embot::common::Callback &oncompletion)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = embot::common::tointegral(s);

        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return resNOK;
        } 
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        return s_writeregister(s, static_cast<std::uint8_t>(reg), value, oncompletion);                         
    }        
       

        
    result_t read(BNO055 s, embot::hw::bno055::Register reg, embot::utils::Data &data, const embot::common::relTime timeout)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = embot::common::tointegral(s);
        
        // i2c must not be used by another device (eg. termometer or ...).
        embot::common::relTime remaining = timeout;
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus, timeout, remaining))
        {
            return resNOK;
        }
        
        // if (when) i2c is not busy (anymore), we just hope there is no another task using the bno055 ...
        // we however just do the check         
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
                
        s_privatedata.acquisition[index].startread(data);   
        
        // ok, now i trigger i2c in blocking mode
        result_t r = embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], static_cast<std::uint8_t>(reg), s_privatedata.acquisition[index].data, timeout);
        s_privatedata.acquisition[index].stop();
        
        return r;      
    }

    
    result_t read(BNO055 s, embot::hw::bno055::Register reg, embot::utils::Data &data, const embot::common::Callback &oncompletion)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = embot::common::tointegral(s);
        
        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return resNOK;
        } 
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        s_privatedata.acquisition[index].startread(data, oncompletion); 
                       
        // ok, now i trigger i2c.
        embot::common::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);
        embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], static_cast<std::uint8_t>(reg), s_privatedata.acquisition[index].data, cbk);
                
        return resOK;        
    }
    

    
//    static void s_powerOFF(void)
//    {
//        embot::hw::gpio::set(gpioBOOT, embot::hw::gpio::State::RESET);
//        embot::hw::gpio::set(gpioRESET, embot::hw::gpio::State::RESET);        
//    }
    
    static void s_powerON(BNO055 s, embot::common::relTime waittime)
    {        
        embot::hw::gpio::set(embot::hw::bsp::bno055::getBSP().getPROP(s)->boot, embot::hw::gpio::State::SET);
        embot::hw::gpio::set(embot::hw::bsp::bno055::getBSP().getPROP(s)->reset, embot::hw::gpio::State::SET);
        
        embot::hw::sys::delay(waittime);         
    }
    
//    static result_t s_programregister_safe(BNO055 s, std::uint8_t reg, std::uint8_t val, embot::common::relTime timeout)
//    {
//        std::uint8_t index = embot::common::tointegral(s);   
//        embot::utils::Data data(&val, 1);        
//        result_t r = embot::hw::i2c::write(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], reg, data, timeout);                
//        return r;
//    }

    
    static result_t s_programregister(BNO055 s, std::uint8_t reg, std::uint8_t val, embot::common::relTime timeout)
    {
        std::uint8_t index = embot::common::tointegral(s);   
        s_privatedata.acquisition[index].startwrite(val);        
        result_t r = embot::hw::i2c::write(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], reg, s_privatedata.acquisition[index].data, timeout);  
        s_privatedata.acquisition[index].stop();        
        return r;
    }
    
    
    static result_t s_writeregister(BNO055 s, std::uint8_t reg, std::uint8_t val, const embot::common::Callback &oncompletion)
    {
        std::uint8_t index = embot::common::tointegral(s);   
        s_privatedata.acquisition[index].startwrite(val, oncompletion);   
        embot::common::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);        
        result_t r = embot::hw::i2c::write(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], reg, s_privatedata.acquisition[index].data, cbk);  
      
        return r;
    }
    
    static void s_sharedCBK(void *p)
    {
        Acquisition *acq = reinterpret_cast<Acquisition*>(p);                
        acq->stop();
    }
    

    
}}} // namespace embot { namespace hw { namespace BNO055 {



#endif //defined(EMBOT_BNO055_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

