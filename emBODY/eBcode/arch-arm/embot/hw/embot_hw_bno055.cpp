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
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

using namespace std;

#include "embot_binary.h"
#include "embot_hw_sys.h"
#include "embot_hw_gpio.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#define EMBOT_BNO055_ENABLED

#if     !defined(EMBOT_BNO055_ENABLED)


namespace embot { namespace hw { namespace BNO055 {

    bool supported(Sensor s)                                                                        { return false; }
    bool initialised(Sensor s)                                                                      { return false; }
    result_t init(Sensor s, const Config &config)                                                   { return resNOK; }
    
    result_t get(Sensor s, Temperature &temp)                                                       { return resNOK; }

}}} // namespace embot { namespace hw { namespace BNO055 {


#elif   defined(EMBOT_BNO055_ENABLED)


namespace embot { namespace hw { namespace BNO055 {
        
    struct bspmap_t
    {
        std::uint32_t       mask;
    };
    
    // const support maps
    #if     defined(STM32HAL_BOARD_STRAIN2)        
    static const bspmap_t bspmap = 
    {
        0x00000001  // means... 1 sensors
    };   
    #else
    static const bspmap_t bspmap = 
    {
        0x00000000
    };
    #endif
      
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t sensor2index(Sensor s)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(s);
    }
        
    bool supported(Sensor s)
    {
        if((Sensor::none == s) || (Sensor::maxnumberof == s))
        {
            return false;
        }
        return embot::binary::bit::check(bspmap.mask, sensor2index(s));
    }
    
    bool initialised(Sensor s)
    {
        if(Sensor::none == s)
        {
            return false;
        }
        return embot::binary::bit::check(initialisedmask, sensor2index(s));
    }    

    struct Acquisition
    {
        volatile bool done;
        volatile bool ongoing;
        std::uint8_t buffer[48];
        embot::common::Data data;
        embot::common::Callback userdefCBK;  
        Acquisition() { clear(); }
        void clear() { data.load(buffer, 0); done = false; ongoing = false; std::memset(buffer, 0, sizeof(buffer)); userdefCBK.callback = nullptr; userdefCBK.arg = nullptr; }         
        void startread(std::uint8_t size, const embot::common::Callback &cbk) { clear(); data.size = size; userdefCBK = cbk; ongoing = true; }
        void startread(std::uint8_t size) { clear(); data.size = size; ongoing = true; }
        void startread(embot::common::Data &da, const embot::common::Callback &cbk) { clear(); data = da; userdefCBK = cbk; ongoing = true; }
        void startread(embot::common::Data &da) { clear(); data = da; ongoing = true; }
        void startwrite(std::uint8_t val) { clear(); buffer[0] = val; data.size = 1; ongoing = true; }
        void startwrite(std::uint8_t val, const embot::common::Callback &cbk) { clear(); buffer[0] = val; data.size = 1; userdefCBK = cbk; ongoing = true; }
        void stop() { ongoing = false; done = true; userdefCBK.execute(); }
    };
    
    struct PrivateData
    {    
        Config config[static_cast<unsigned int>(Sensor::maxnumberof)];        
        Acquisition acquisition[static_cast<unsigned int>(Sensor::maxnumberof)];
        PrivateData() { }
    };
    
    // power-on-reset wait time. 650 ms is enough, datsheet say, before the device can talk over i2c. but much better to be devil-compliant ;-).
    static const embot::common::relTime PORtime = 666*embot::common::time1millisec;
    static const std::uint8_t i2caddress = 0x52;
    static const std::uint8_t registerCHIP_ID = 0x00;
    static const std::uint8_t registerSTARTDATA = 0x08;
    
    #if     defined(STM32HAL_BOARD_STRAIN2)
        static const embot::hw::gpio::GPIO gpioBOOT(BNO055_BOOT_GPIO_Port, BNO055_BOOT_Pin);
        static const embot::hw::gpio::GPIO gpioRESET(BNO055_RESET_GPIO_Port, BNO055_RESET_Pin);
    #elif   defined(STM32HAL_BOARD_MTB4)
        #warning --> MUST define the BOOT and RESET GPIOs  whh now are notvalid 
        static const embot::hw::gpio::GPIO gpioBOOT;
        static const embot::hw::gpio::GPIO gpioRESET;            
    #endif
    
    static PrivateData s_privatedata;
    
    static void s_powerOFF(void);    
    static void s_powerON(embot::common::relTime waittime);    
    static result_t s_programregister(Sensor s, std::uint8_t reg, std::uint8_t val, embot::common::relTime timeout);    
    static result_t s_writeregister(Sensor s, std::uint8_t reg, std::uint8_t val, const embot::common::Callback &oncompletion);
    static void s_sharedCBK(void *p);
    
    // sttaic functions used for testing
    static result_t s_start(Sensor s);
            
    result_t init(Sensor s, const Config &config)
    {
        if(false == supported(s))
        {
            return resNOK;
        }
        
        if(true == initialised(s))
        {
            return resOK;
        }
        
        // ower on and wait until the i2c is working. datasheet say: POR time > 650 ms.
        s_powerON(PORtime);
        
        std::uint8_t index = sensor2index(s);
                        
        // init i2c ..
        embot::hw::i2c::init(config.i2cdes.bus, config.i2cdes.config);
        
        if(false == embot::hw::i2c::ping(config.i2cdes.bus, i2caddress))
        {
            return resNOK;
        }
        
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].clear();
        
        embot::binary::bit::set(initialisedmask, sensor2index(s));
                
        return resOK;
    }

    bool isbusbusy(Sensor s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = sensor2index(s);  
        return embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus);             
    }
    
    bool isacquiring(Sensor s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = sensor2index(s);        
        return s_privatedata.acquisition[index].ongoing;     
    }
    
//    result_t read(Sensor s, embot::hw::BNO055::Register reg, const embot::common::Callback &oncompletion)
//    {
//        if(false == initialised(s))
//        {
//            return resNOK;
//        } 

//        std::uint8_t index = sensor2index(s);
//        
//        if(true == s_privatedata.acquisition[index].ongoing)
//        {
//            return resNOK;
//        }
//        
//        // i2c must not be busy
//        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
//        {
//            return resNOK;
//        }
//                
//        s_privatedata.acquisition[index].clear();
//        s_privatedata.acquisition[index].ongoing = true;
//        s_privatedata.acquisition[index].done = false;
//        s_privatedata.acquisition[index].userdefCBK = oncompletion;
//        s_privatedata.acquisition[index].data.size = 1; // one byte only.
//        
////        s_privatedata.acquisition[index].startread(1, oncompletion);
//        
//        // ok, now i trigger i2c.
//        embot::common::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);
//        embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, i2caddress, static_cast<std::uint8_t>(reg), s_privatedata.acquisition[index].data, cbk);
//                
//        return resOK;        
//    }
    
    
//    result_t read(Sensor s, embot::hw::BNO055::Register reg, std::uint8_t &value, const embot::common::relTime timeout)
//    {
//        if(false == initialised(s))
//        {
//            return resNOK;
//        } 

//        std::uint8_t index = sensor2index(s);
//        
//        if(true == s_privatedata.acquisition[index].ongoing)
//        {
//            return resNOK;
//        }
//        
//        // i2c must not be busy
//        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
//        {
//            return resNOK;
//        }
//                
////        s_privatedata.acquisition[index].clear();
////        s_privatedata.acquisition[index].ongoing = true;
////        s_privatedata.acquisition[index].done = false;
////        s_privatedata.acquisition[index].userdefCBK.clear();
////        s_privatedata.acquisition[index].data.size = 1; // one byte only.
//        
//        s_privatedata.acquisition[index].startread(1);   // one byte only, no user callback
//        
//        // ok, now i trigger i2c in blocking mode
//        result_t r = embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, i2caddress, static_cast<std::uint8_t>(reg), s_privatedata.acquisition[index].data, timeout);
//        s_privatedata.acquisition[index].stop();
//        value = s_privatedata.acquisition[index].data.U08(0);
//        
//        return r;        
//    }        
    
    result_t read(Sensor s, embot::hw::BNO055::Register reg, embot::common::Data &data, const embot::common::relTime timeout)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = sensor2index(s);
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return resNOK;
        }
                
        s_privatedata.acquisition[index].startread(data);   
        
        // ok, now i trigger i2c in blocking mode
        result_t r = embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, i2caddress, static_cast<std::uint8_t>(reg), s_privatedata.acquisition[index].data, timeout);
        s_privatedata.acquisition[index].stop();
        
        return r;      
    }

    
    result_t read(Sensor s, embot::hw::BNO055::Register reg, embot::common::Data &data, const embot::common::Callback &oncompletion)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = sensor2index(s);
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return resNOK;
        }
        
        s_privatedata.acquisition[index].startread(data, oncompletion); 
                       
        // ok, now i trigger i2c.
        embot::common::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);
        embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, i2caddress, static_cast<std::uint8_t>(reg), s_privatedata.acquisition[index].data, cbk);
                
        return resOK;        
    }
    
    
    result_t get(Sensor s, Info &info, embot::common::relTime timeout)
    {
        embot::common::Data data(&info, sizeof(info));
        result_t r = read(s, embot::hw::BNO055::Register::CHIP_ID, data, timeout);  
        return r;        
    }

    result_t acquisition(Sensor s, DataSet dataset, const embot::common::Callback &oncompletion)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = sensor2index(s);
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return resNOK;
        }
        
        // start a read of nbytes which will go into buffer[]
        const std::uint8_t nbytes = static_cast<std::uint8_t>(dataset);
        const Register reg = Register::DATASET_START;
        
        s_privatedata.acquisition[index].startread(nbytes, oncompletion); 
                       
        // ok, now i trigger i2c.
        embot::common::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);
        embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, i2caddress, static_cast<std::uint8_t>(reg), s_privatedata.acquisition[index].data, cbk);
                
        return resOK;          
    }
    
    result_t acquisition(Sensor s, std::uint8_t reg, std::uint8_t size, const embot::common::Callback &oncompletion)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = sensor2index(s);
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return resNOK;
        }
                
        s_privatedata.acquisition[index].clear();
        s_privatedata.acquisition[index].ongoing = true;
        s_privatedata.acquisition[index].done = false;
        s_privatedata.acquisition[index].userdefCBK = oncompletion;
        s_privatedata.acquisition[index].data.size = size; // more bytes
        
        s_privatedata.acquisition[index].startread(size, oncompletion);
        
        // ok, now i trigger i2c.
        embot::common::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);
        embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, i2caddress, reg, s_privatedata.acquisition[index].data, cbk);
        
        
        return resOK;
    }
    
    result_t set(Sensor s, Mode m, embot::common::relTime timeout)
    {        
        if(false == initialised(s))
        {
            return resNOK;
        } 
        
        if((Mode::none == m) || (Mode::maxnumberof == m))
        {
            return resNOK;
        }

        std::uint8_t index = sensor2index(s);
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return resNOK;
        } 
        
        return s_programregister(s, static_cast<std::uint8_t>(Register::OPR_MODE), static_cast<std::uint8_t>(m), timeout);                                        
    }
    
    result_t write(Sensor s, embot::hw::BNO055::Register reg, std::uint8_t value, const embot::common::relTime timeout)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = sensor2index(s);
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return resNOK;
        } 
        
        return s_programregister(s, static_cast<std::uint8_t>(reg), value, timeout);                         
    }
    
    result_t write(Sensor s, embot::hw::BNO055::Register reg, std::uint8_t value, const embot::common::Callback &oncompletion)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = sensor2index(s);
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return resNOK;
        } 
        
        return s_writeregister(s, static_cast<std::uint8_t>(reg), value, oncompletion);                         
    }    
    
    
    bool isalive(Sensor s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = sensor2index(s);
        
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return false;
        }

        return embot::hw::i2c::ping(s_privatedata.config[index].i2cdes.bus, i2caddress);        
    }

    
    bool isready(Sensor s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = sensor2index(s);

        return s_privatedata.acquisition[index].done;        
    }
    
    
    result_t read(Sensor s, std::uint8_t &value)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        if(false == isready(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = sensor2index(s);
        
        if(true != s_privatedata.acquisition[index].data.isvalid())
        {
            return resNOK;
        }
        
        value = s_privatedata.acquisition[index].data.U08(0);
  
        return resOK;        
    }
    

    result_t read(Sensor s, embot::common::Data &data)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        if(false == isready(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = sensor2index(s);
        data = s_privatedata.acquisition[index].data;
        
        return resOK;        
    }  

//    result_t read(Sensor s, Fulldata &fulldata)
//    { 
//        if(false == initialised(s))
//        {
//            return resNOK;
//        } 

//        if(false == isready(s))
//        {
//            return resNOK;
//        }
//        
//        std::uint8_t index = sensor2index(s);
//        fulldata.load(s_privatedata.acquisition[index].data.pointer);
//        
//        return resOK;   
//    }  

    result_t read(Sensor s, Fulldata &fulldata)
    { 
        if(false == initialised(s))
        {
            return resNOK;
        } 

        if(false == isready(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = sensor2index(s);
        fulldata.load(s_privatedata.acquisition[index].data.pointer);
        
        return resOK;   
    }    
    

//    result_t read(Sensor s, Triple &triple)
//    {
//        if(false == initialised(s))
//        {
//            return resNOK;
//        } 

//        if(false == isready(s))
//        {
//            return resNOK;
//        }
//        
//        std::uint8_t index = sensor2index(s);

//        
//        return resOK;        
//    } 
    
    
    static void s_powerOFF(void)
    {
        embot::hw::gpio::set(gpioBOOT, embot::hw::gpio::State::RESET);
        embot::hw::gpio::set(gpioRESET, embot::hw::gpio::State::RESET);        
    }
    
    static void s_powerON(embot::common::relTime waittime)
    {        
        embot::hw::gpio::set(gpioBOOT, embot::hw::gpio::State::SET);
        embot::hw::gpio::set(gpioRESET, embot::hw::gpio::State::SET);
        
        embot::hw::sys::delay(waittime);         
    }
    
    static result_t s_programregister_safe(Sensor s, std::uint8_t reg, std::uint8_t val, embot::common::relTime timeout)
    {
        std::uint8_t index = sensor2index(s);   
        embot::common::Data data(&val, 1);        
        result_t r = embot::hw::i2c::write(s_privatedata.config[index].i2cdes.bus, i2caddress, reg, data, timeout);                
        return r;
    }

    
    static result_t s_programregister(Sensor s, std::uint8_t reg, std::uint8_t val, embot::common::relTime timeout)
    {
        std::uint8_t index = sensor2index(s);   
        s_privatedata.acquisition[index].startwrite(val);        
        result_t r = embot::hw::i2c::write(s_privatedata.config[index].i2cdes.bus, i2caddress, reg, s_privatedata.acquisition[index].data, timeout);  
        s_privatedata.acquisition[index].stop();        
        return r;
    }
    
    
    static result_t s_writeregister(Sensor s, std::uint8_t reg, std::uint8_t val, const embot::common::Callback &oncompletion)
    {
        std::uint8_t index = sensor2index(s);   
        s_privatedata.acquisition[index].startwrite(val, oncompletion);   
        embot::common::Callback cbk(s_sharedCBK, &s_privatedata.acquisition[index]);        
        result_t r = embot::hw::i2c::write(s_privatedata.config[index].i2cdes.bus, i2caddress, reg, s_privatedata.acquisition[index].data, cbk);  
      
        return r;
    }
    
    static void s_sharedCBK(void *p)
    {
        Acquisition *acq = reinterpret_cast<Acquisition*>(p);                
        acq->stop();
    }
    
    // static functions used to test the peripheral
    
    static result_t s_start(Sensor s)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        std::uint8_t index = sensor2index(s);
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return resNOK;
        }
        
        // i2c must not be busy
        if(true == embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus))
        {
            return resNOK;
        } 

        const std::uint8_t range02g = 0x0c;
        const std::uint8_t range04g = 0x0d;
        const std::uint8_t range08g = 0x0e;
        const std::uint8_t range16g = 0x0f;
        s_programregister(s, static_cast<std::uint8_t>(Register::PAGE_ID), 0x01, 20*embot::common::time1millisec);        
        s_programregister(s, 0x08, range02g, 20*embot::common::time1millisec);          
        s_programregister(s, static_cast<std::uint8_t>(Register::PAGE_ID), 0x00, 20*embot::common::time1millisec);  

        // unit_sel: 0x80 is m/sec^2, 0x81 is mg
        s_programregister(s, 0x3B, 0x81, 20*embot::common::time1millisec);        

        // i set the content of some registers:
        // reg_PWR_MODE 0x3E, bits [1,0]        -> power mode
        // value:
        //        #define BNO055_POWER_MODE_NORMAL	    (0X00)
        //        #define BNO055_POWER_MODE_LOWPOWER    (0X01)
        //        #define BNO055_POWER_MODE_SUSPEND	    (0X02)
        // use: BNO055_POWER_MODE_NORMAL 0x02
        //
        // reg_OPR_MODE 0x3D, bits [3,0]        -> operation mode
        // value:       
        //        #define BNO055_OPERATION_MODE_CONFIG			(0X00)
        //        #define BNO055_OPERATION_MODE_ACCONLY			(0X01)
        //        #define BNO055_OPERATION_MODE_MAGONLY			(0X02)
        //        #define BNO055_OPERATION_MODE_GYRONLY			(0X03)
        //        #define BNO055_OPERATION_MODE_ACCMAG			(0X04)
        //        #define BNO055_OPERATION_MODE_ACCGYRO			(0X05)
        //        #define BNO055_OPERATION_MODE_MAGGYRO			(0X06)
        //        #define BNO055_OPERATION_MODE_AMG				(0X07)
        //        #define BNO055_OPERATION_MODE_IMUPLUS			(0X08)
        //        #define BNO055_OPERATION_MODE_COMPASS			(0X09)
        //        #define BNO055_OPERATION_MODE_M4G				(0X0A)
        //        #define BNO055_OPERATION_MODE_NDOF_FMC_OFF    (0X0B)
        //        #define BNO055_OPERATION_MODE_NDOF		    (0X0C)
        // use: BNO055_OPERATION_MODE_AMG 0x07
        
        // provo a scrivere su registro ... il valore ...
        
        s_programregister(s, static_cast<std::uint8_t>(Register::OPR_MODE), 0x07, 20*embot::common::time1millisec);
                
        
        return resOK;
    }    
    
}}} // namespace embot { namespace hw { namespace BNO055 {



#endif //defined(EMBOT_BNO055_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

