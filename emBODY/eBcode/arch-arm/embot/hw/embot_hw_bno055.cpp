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

#include "embot_hw_bsp_config.h"
#include "embot_hw_bno055_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"
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

#if !defined(EMBOT_ENABLE_hw_bno055)

namespace embot { namespace hw { namespace bno055 {

    bool supported(BNO055 s)                                                                        { return false; }
    bool initialised(BNO055 s)                                                                      { return false; }
    result_t init(BNO055 s, const Config &config)                                                   { return resNOK; }
    bool isalive(BNO055 s, embot::core::relTime timeout)                                          { return false; }
    result_t get(BNO055 s, Info &info, embot::core::relTime timeout)                              { return resNOK; }
    result_t set(BNO055 s, Mode m, embot::core::relTime timeout)                                  { return resNOK; }
    result_t get(BNO055 s, Mode &m, embot::core::relTime timeout) = { return resNOK; }
    bool isacquiring(BNO055 s)                                                                      { return false; }
    bool canacquire(BNO055 s)                                                                       { return false; }
    result_t acquisition(BNO055 s, Set set, const embot::core::Callback &oncompletion)            { return resNOK; }
    bool operationdone(BNO055 s)                                                                    { return false; }
    result_t read(BNO055 s, Data &data)                                                             { return resNOK; }
    result_t write(BNO055 s, embot::hw::bno055::Register reg, std::uint8_t value, const embot::core::relTime timeout){ return resNOK; }
    result_t write(BNO055 s, embot::hw::bno055::Register reg, std::uint8_t value, const embot::core::Callback &oncompletion){ return resNOK; }
    result_t read(BNO055 s, embot::hw::bno055::Register reg, embot::core::Data &data, const embot::core::relTime timeout){ return resNOK; }
    result_t read(BNO055 s, embot::hw::bno055::Register reg, embot::core::Data &data, const embot::core::Callback &oncompletion){ return resNOK; }
    
    result_t set(BNO055 s, Placement p, const embot::core::relTime timeout) { return resNOK; }
    result_t get(BNO055 s, Placement &p, const embot::core::relTime timeout) { return resNOK; }
    
}}} // namespace embot { namespace hw { namespace BNO055 {


#else

namespace embot { namespace hw { namespace bno055 {
        
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(BNO055 s)
    {
        return embot::hw::bno055::getBSP().supported(s);
    }
    
    bool initialised(BNO055 s)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(s));
    }     

    struct Acquisition
    {
        volatile bool done;
        volatile bool ongoing;
        std::uint8_t buffer[48];
        embot::core::Data data;
        embot::core::Callback userdefCBK;  
        Acquisition() { clear(); }
        void clear() { data.load(buffer, 0); done = false; ongoing = false; std::memset(buffer, 0, sizeof(buffer)); userdefCBK.clear(); }         
        void startread(std::uint8_t size, const embot::core::Callback &cbk) { clear(); data.capacity = size; userdefCBK = cbk; ongoing = true; }
        void startread(std::uint8_t size) { clear(); data.capacity = size; ongoing = true; }
        void startread(embot::core::Data &da, const embot::core::Callback &cbk) { clear(); data = da; userdefCBK = cbk; ongoing = true; }
        void startread(embot::core::Data &da) { clear(); data = da; ongoing = true; }
        void startwrite(std::uint8_t val) { clear(); buffer[0] = val; data.capacity = 1; ongoing = true; }
        void startwrite(std::uint8_t val, const embot::core::Callback &cbk) { clear(); buffer[0] = val; data.capacity = 1; userdefCBK = cbk; ongoing = true; }
        void stop() { ongoing = false; done = true; userdefCBK.execute(); }
    };
    
    struct PrivateData
    {    
        embot::hw::i2c::Descriptor i2cdes[embot::core::tointegral(BNO055::maxnumberof)];
        Config config[embot::core::tointegral(BNO055::maxnumberof)];        
        Acquisition acquisition[embot::core::tointegral(BNO055::maxnumberof)];
        PrivateData() { }
    };
    
    // power-on-reset wait time. 650 ms is enough, datasheet say, before the device can talk over i2c. but much better to be devil-compliant ;-).
    static const embot::core::relTime PORtime = 666*embot::core::time1millisec;
    
    
    static PrivateData s_privatedata;
     
    static void s_powerON(BNO055 s, embot::core::relTime waittime);    
    static result_t s_programregister(BNO055 s, std::uint8_t reg, std::uint8_t val, embot::core::relTime timeout);    
    static result_t s_writeregister(BNO055 s, std::uint8_t reg, std::uint8_t val, const embot::core::Callback &oncompletion);
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
        embot::hw::bno055::getBSP().init(s);
        
        // power on and wait until the i2c is working. datasheet say: POR time > 650 ms.
        s_powerON(s, PORtime);
        
        std::uint8_t index = embot::core::tointegral(s);
        
                        
        // init i2c ..
        embot::hw::i2c::init(embot::hw::bno055::getBSP().getPROP(s)->i2cdes.bus, {});
        
        if(false == embot::hw::i2c::ping(embot::hw::bno055::getBSP().getPROP(s)->i2cdes.bus, embot::hw::bno055::getBSP().getPROP(s)->i2cdes.adr, 3*embot::core::time1millisec))
        {
            return resNOK;
        }
        
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].clear();
        s_privatedata.i2cdes[index] = embot::hw::bno055::getBSP().getPROP(s)->i2cdes;
        
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(s));
        
        // after initialised mask i can do other things:
        
        // enable the external oscillator by writing 0x80 in reg 0x3F
        embot::hw::bno055::write(s, embot::hw::bno055::Register::SYS_TRIGGER, 0x80, 5*embot::core::time1millisec);
                
        return resOK;
    }



    bool isalive(BNO055 s, embot::core::relTime timeout)
    {
        if(false == initialised(s))
        {
            return false;
        } 
        std::uint8_t index = embot::core::tointegral(s);
        return embot::hw::i2c::ping(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, timeout);  
    }        


    
    result_t get(BNO055 s, Info &info, embot::core::relTime timeout)
    {
        embot::core::Data data(&info, sizeof(info));
        result_t r = read(s, embot::hw::bno055::Register::CHIP_ID, data, timeout);  
        return r;        
    }
    
    
    result_t set(BNO055 s, Mode m, embot::core::relTime timeout)
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
    
    result_t get(BNO055 s, Mode &m, embot::core::relTime timeout)
    {
        if(false == initialised(s))
        {
            m = Mode::none;
            return resNOK;
        }  
                       
        uint8_t dd = {0};
        embot::core::Data data {&dd, 1};
        if(resOK == read(s, Register::OPR_MODE, data, timeout))
        {   
            m = static_cast<Mode>(dd);;
            return resOK;            
        }
        
        return resNOK;        
    }


    bool isacquiring(BNO055 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(s);        
        return s_privatedata.acquisition[index].ongoing;     
    }    
    
        
    bool canacquire(BNO055 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(s);  
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return false;
        }
        
        return !embot::hw::i2c::isbusy(s_privatedata.i2cdes[index].bus);             
    }
    
    
    
    result_t acquisition(BNO055 s, Set set, const embot::core::Callback &oncompletion)
    {        
        if(false == canacquire(s))
        {
            return resNOK;
        }
        
        // ok, start a read of nbytes which will go into buffer[]
        const std::uint8_t nbytes = static_cast<std::uint8_t>(set);
        const Register reg = Register::DATASET_START;
        
        std::uint8_t index = embot::core::tointegral(s);
        
        s_privatedata.acquisition[index].startread(nbytes, oncompletion); 
                       
        // now i trigger i2c.
        embot::core::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);
        return embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, static_cast<std::uint8_t>(reg), s_privatedata.acquisition[index].data, cbk);        
    } 
    
    
    
    result_t acquisition(BNO055 s, Set set, Data &data, const embot::core::Callback &oncompletion)
    {        
        if(false == canacquire(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::core::tointegral(s);
        
        const std::uint8_t nbytes = static_cast<std::uint8_t>(set);        
        // ok, start a read of nbytes only (not all sizeof(data)) which will go into data
        embot::core::Data da(&data, nbytes);
                
        s_privatedata.acquisition[index].startread(da, oncompletion); 
                       
        // now i trigger i2c.
        embot::core::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);
        return embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, static_cast<std::uint8_t>(Register::DATASET_START), s_privatedata.acquisition[index].data, cbk);        
    } 
    

    result_t acquisition(BNO055 s, Set set, Data &data, const embot::core::relTime timeout)
    { 
        const std::uint8_t nbytes = static_cast<std::uint8_t>(set);        
        // ok, start a read of nbytes only (not all sizeof(data)) which will go into data
        embot::core::Data da(&data, nbytes);
        
        return read(s, Register::DATASET_START, da, timeout);
    }     
    
    bool operationdone(BNO055 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        return s_privatedata.acquisition[embot::core::tointegral(s)].done;        
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
        
        std::uint8_t index = embot::core::tointegral(s);
        data.load(s_privatedata.acquisition[index].data.pointer);
        
        return resOK;   
    } 


    result_t write(BNO055 s, embot::hw::bno055::Register reg, std::uint8_t value, const embot::core::relTime timeout)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 
        
        std::uint8_t index = embot::core::tointegral(s);
        
        // i2c must not be used by another device (eg. termometer or ...).
        embot::core::relTime remaining = timeout;
        if(true == embot::hw::i2c::isbusy(s_privatedata.i2cdes[index].bus, timeout, remaining))
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

    
    result_t write(BNO055 s, embot::hw::bno055::Register reg, std::uint8_t value, const embot::core::Callback &oncompletion)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = embot::core::tointegral(s);

        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.i2cdes[index].bus))
        {
            return resNOK;
        } 
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        return s_writeregister(s, static_cast<std::uint8_t>(reg), value, oncompletion);                         
    }        
       

        
    result_t read(BNO055 s, embot::hw::bno055::Register reg, embot::core::Data &data, const embot::core::relTime timeout)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = embot::core::tointegral(s);
        
        // i2c must not be used by another device (eg. termometer or ...).
        embot::core::relTime remaining = timeout;
        if(true == embot::hw::i2c::isbusy(s_privatedata.i2cdes[index].bus, timeout, remaining))
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
        result_t r = embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, static_cast<std::uint8_t>(reg), s_privatedata.acquisition[index].data, timeout);
        s_privatedata.acquisition[index].stop();
        
        return r;      
    }

    
    result_t read(BNO055 s, embot::hw::bno055::Register reg, embot::core::Data &data, const embot::core::Callback &oncompletion)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = embot::core::tointegral(s);
        
        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.i2cdes[index].bus))
        {
            return resNOK;
        } 
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        s_privatedata.acquisition[index].startread(data, oncompletion); 
                       
        // ok, now i trigger i2c.
        embot::core::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);
        embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, static_cast<std::uint8_t>(reg), s_privatedata.acquisition[index].data, cbk);
                
        return resOK;        
    }
    
    
    struct AxisRemap
    {
        static constexpr size_t numberofplacements {embot::core::tointegral(Placement::maxnumberof)};  
        
        static constexpr std::array<std::pair<uint8_t, uint8_t>, numberofplacements> regsvalues
        {
            std::pair(0x21, 0x04), std::pair(0x24, 0x00), std::pair(0x24, 0x06), std::pair(0x21, 0x02), 
            std::pair(0x24, 0x03), std::pair(0x21, 0x01), std::pair(0x21, 0x07), std::pair(0x24, 0x05)            
        }; 
        
        static constexpr bool isvalid(const Placement p)
        {
            uint8_t i = embot::core::tointegral(p);
            return (i < numberofplacements) ? true : false;        
        }
        
        static constexpr uint8_t toindex(const Placement p)
        {
            if(isvalid(p))
            {
               return embot::core::tointegral(p); 
            }            
            return embot::core::tointegral(Placement::DEFAULT);
        }
        
        static constexpr uint8_t valueof_AXIS_MAP_CONFIG(const Placement p) 
        {
            return regsvalues[toindex(p)].first;
        } 
        
        static constexpr uint8_t valueof_AXIS_MAP_SIGN(const Placement p) 
        {
            return regsvalues[toindex(p)].second;
        }  
        
        static constexpr Placement find(const uint8_t cfg, const uint8_t sign)
        {
            Placement p {Placement::none};
            for(uint8_t i=0; i<regsvalues.size(); i++) 
            {
                if((regsvalues[i].first == cfg) && (regsvalues[i].second == sign))
                {
                    p = static_cast<Placement>(i);
                    break;
                }
            }
            return p;
        }

        AxisRemap() = default;        
    };

    result_t set(BNO055 s, Placement p, const embot::core::relTime timeout)
    {
        if(false == AxisRemap::isvalid(p))
        {
            return resNOK;
        }
        
        if(resOK == write(s, Register::AXIS_MAP_CONFIG, AxisRemap::valueof_AXIS_MAP_CONFIG(p), timeout))
        {
            return write(s, Register::AXIS_MAP_SIGN, AxisRemap::valueof_AXIS_MAP_SIGN(p), timeout);
        }
        
        return resNOK;
    }
    
    result_t get(BNO055 s, Placement &p, const embot::core::relTime timeout)
    {       
        uint8_t dd[2] = {0, 0};
        embot::core::Data data {dd, 2};
        if(resOK == read(s, embot::hw::bno055::Register::AXIS_MAP_CONFIG, data, timeout))
        {   
            p = AxisRemap::find(dd[0], dd[1]);
            return resOK;            
        }
        
        return resNOK;
    }        
    

    
//    static void s_powerOFF(void)
//    {
//        embot::hw::gpio::set(gpioBOOT, embot::hw::gpio::State::RESET);
//        embot::hw::gpio::set(gpioRESET, embot::hw::gpio::State::RESET);        
//    }
    
    static void s_powerON(BNO055 s, embot::core::relTime waittime)
    {
        const embot::hw::GPIO nBOOT_LOAD_PIN {embot::hw::bno055::getBSP().getPROP(s)->nBOOT_LOAD_PIN};
        const embot::hw::GPIO nRESET {embot::hw::bno055::getBSP().getPROP(s)->nRESET};
        // if we have valid boot and reset we just use them
        if(true == nBOOT_LOAD_PIN.isvalid())
        {
            // init and make sure it stays high so that the bootload is NOT loaded at bootstratp 
            embot::hw::gpio::init(nBOOT_LOAD_PIN, embot::hw::gpio::cfgOUTpp);
            embot::hw::gpio::set(nBOOT_LOAD_PIN, embot::hw::gpio::State::SET);
        }

        if(true == nRESET.isvalid())
        {
            // the bno restarts if nRESET is low for at least 80 ns. then we wait for the POR time 
            embot::hw::gpio::init(nRESET, embot::hw::gpio::cfgOUTpp);
            // we ensure it is high
            embot::hw::gpio::set(nRESET, embot::hw::gpio::State::SET);
            // we set low and stay ... 1 ms
            embot::hw::gpio::set(nRESET, embot::hw::gpio::State::RESET);
            embot::hw::sys::delay(embot::core::time1millisec);
            // we can put it high again and wait for the POR time
            embot::hw::gpio::set(nRESET, embot::hw::gpio::State::SET);
            embot::hw::sys::delay(waittime);            
        }             
    }
    
    
    static result_t s_programregister(BNO055 s, std::uint8_t reg, std::uint8_t val, embot::core::relTime timeout)
    {
        std::uint8_t index = embot::core::tointegral(s);   
        s_privatedata.acquisition[index].startwrite(val);        
        result_t r = embot::hw::i2c::write(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, reg, s_privatedata.acquisition[index].data, timeout);  
        s_privatedata.acquisition[index].stop();        
        return r;
    }
    
    
    static result_t s_writeregister(BNO055 s, std::uint8_t reg, std::uint8_t val, const embot::core::Callback &oncompletion)
    {
        std::uint8_t index = embot::core::tointegral(s);   
        s_privatedata.acquisition[index].startwrite(val, oncompletion);   
        embot::core::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);        
        result_t r = embot::hw::i2c::write(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, reg, s_privatedata.acquisition[index].data, cbk);  
      
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

