
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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BNO055_H_
#define _EMBOT_HW_BNO055_H_

#include "embot_common.h"
#include "embot_binary.h"
#include "embot_hw.h"


#include "embot_hw_i2c.h"

    
namespace embot { namespace hw { namespace BNO055 {
         
    enum class Sensor { one = 0, none = 32, maxnumberof = 1 };
    
        
    struct Config
    {  
        embot::hw::i2c::Descriptor      i2cdes;        
        Config(embot::hw::i2c::Bus b, std::uint32_t s) : i2cdes(b, s) {}        
        Config() : i2cdes(embot::hw::i2c::Bus::one, 400000) {}
        Config(const embot::hw::i2c::Descriptor &des) : i2cdes(des) {}
    };
    
    
    enum class Mode 
    { 
        CONFIG = 0, 
        ACCONLY = 1, MAGONLY = 2, GYRONLY = 3, ACCMAG = 4, ACCGYRO = 5, MAGGYRO = 6, AMG = 7,   // non fusion modes
        IMU = 8, COMPASS = 9, M4G = 10, NDOF_FMC_OFF = 11, NDOF = 12,                           // fusion modes 
        none = 32, maxnumberof = 13 
    };
        
     
    enum class Register 
    { 
        CHIP_ID         = 0x00, // 7 bytes RO with many info. see embot::hw::BNO055::Info 
        PAGE_ID         = 0x07, // 1 byte RW. it changes page. unfortunately there are no registers defined for page 1
        DATASET_START   = 0x08, // how many as we want: acc + mag + gyr + eul + qua + lia + grv + temp + calibstat + stresult (total = 47 bytes).
        ACC_DATA        = 0x08, // 6 bytes RO
        MAG_DATA        = 0x0E, // 6 bytes RO
        GYR_DATA        = 0x14, // 6 bytes RO
        EUL_DATA        = 0x1A, // 6 bytes RO
        QUA_DATA        = 0x20, // 8 bytes RO
        LIA_DATA        = 0x28, // 6 bytes RO
        GRV_DATA        = 0x2E, // 6 bytes RO
        TEMP            = 0x34, // 1 byte RO
        CALIB_STAT      = 0x35, // 1 byte RO in bit pairs: sys|gyr|acc|mag (3 is fully calib, 0 is not calib)
        ST_RESULT       = 0x36, // 1 byte RO, but only ls nibble: mcu|gyr|mag|acc (1 is ok, 0 is ko)
        OPR_MODE        = 0x3D, 
        PWR_MODE        = 0x3E
    };  

    enum class Set
    {
        A               = 6,        // acc: 6 bytes (1 Triple<std::uint16_t>)
        AMG             = 18,       // acc+mag+gyr: 18 bytes    
        AMGE            = 24,       // acc+mag+gyr+eul: 24 bytes 
        AMGEQ           = 32,       // acc+mag+gyr+eul+quat: 32 bytes 
        AMGEQL          = 38,       // acc+mag+gyr+eul+quat+lia: 38 bytes
        AMGEQLG         = 44,       // acc+mag+gyr+eul+quat+lia+grv: 44 bytes        
        FULL            = 47        // acc+mag+gyr+eul+quat+lia+grv+temp+calib+res: 47 bytes     
    };
    
          
    struct Info
    {   // 7 registers from Register::CHIP_ID
        std::uint8_t    chipID;         // RO: 0xA0
        std::uint8_t    accID;          // RO: 0xFB
        std::uint8_t    magID;          // RO: 0x32
        std::uint8_t    gyrID;          // RO: 0x0F
        std::uint16_t   swREV;          // RO: 
        std::uint8_t    blVER;          // RO:      
        Info() { clear(); }
        void clear() { chipID = 0; accID = 0; magID = 0; gyrID = 0; swREV = 0; blVER = 0; }
        void load(void *mem) 
        { 
            std::uint8_t *m = reinterpret_cast<std::uint8_t*>(mem); 
            chipID = m[0]; accID = m[1]; magID = m[2]; gyrID = m[3]; 
            swREV = m[4] | (static_cast<std::uint16_t>(m[5]) << 8);
            blVER = m[6];
        }
        bool isvalid() { if((0xA0==chipID)&&(0xFB==accID)&&(0x32==magID)&&(0x0F==gyrID)){return true;} return false; }
    };
         
    static const float accRES = 0.01f;              // = 1/100 [m/(s*s)]
    static const float magRES = 0.0625f;            // = 1/16 [microTesla]
    static const float gyrRES = 0.0625f;            // = 1/16 [deg/s]
    static const float eulRES = 0.0625f;            // = 1/16 [deg]
    static const float quaRES = 0.00006103515625f;  // = 1/16384 [quaternion]
    static const float liaRES = 0.01f;              // = 1/100 [m/(s*s)]
    static const float grvRES = 0.01f;              // = 1/100 [m/(s*s)]
    static const float tmpRES = 1.0f;              // = 1 [Celsius Deg]
    
    struct Data
    {   // it holds the Set::FULL lot
        embot::common::Triple<std::int16_t>     acc;    // acc.z = 9.8 means: horizontally placed 
        embot::common::Triple<std::int16_t>     mag;
        embot::common::Triple<std::int16_t>     gyr;
        embot::common::Triple<std::int16_t>     eul;    // eul.x = 0 means: directed towards NORTH
        embot::common::Quadruple<std::int16_t>  qua;
        embot::common::Triple<std::int16_t>     lia;
        embot::common::Triple<std::int16_t>     grv;
        std::uint8_t                            temperature;
        std::uint8_t                            calibstatus;
        std::uint8_t                            systemstatus;  
        Data() { temperature = 0; calibstatus = 0; systemstatus = 0; }
        void load(void *mem) 
        {
            std::uint8_t *m = reinterpret_cast<std::uint8_t*>(mem); 
            acc.load(m); mag.load(&m[6]); gyr.load(&m[12]); eul.load(&m[18]);
            qua.load(&m[24]);
            lia.load(&m[32]); grv.load(&m[38]);
            temperature = m[44]; calibstatus = m[45]; systemstatus = m[46];
        }
        void getACC(embot::common::Triple<float> &a) const { a.x = accRES * acc.x; a.y = accRES * acc.y; a.z = accRES * acc.z; }
        void getMAG(embot::common::Triple<float> &a) const { a.x = magRES * mag.x; a.y = magRES * mag.y; a.z = magRES * mag.z; }
        void getGYR(embot::common::Triple<float> &a) const { a.x = gyrRES * gyr.x; a.y = gyrRES * gyr.y; a.z = gyrRES * gyr.z; }            
        void getEUL(embot::common::Triple<float> &a) const { a.x = eulRES * eul.x; a.y = eulRES * eul.y; a.z = eulRES * eul.z; }
        void getQUA(embot::common::Quadruple<float> &a) const { a.w = quaRES * qua.w; a.x = quaRES * qua.x; a.y = quaRES * qua.y; a.z = quaRES * qua.z; }
        void getLIA(embot::common::Triple<float> &a) const { a.x = liaRES * lia.x; a.y = liaRES * lia.y; a.z = liaRES * lia.z; }
        void getGRV(embot::common::Triple<float> &a) const { a.x = grvRES * grv.x; a.y = grvRES * grv.y; a.z = grvRES * grv.z; }
        void getTemperature(float &a) const { a = tmpRES * temperature; }
        std::uint8_t calibrationOfACC() const { return(embot::binary::pair::get(calibstatus, 1)); } // 3 is ok, 1 is not calibrated
        std::uint8_t calibrationOfGYR() const { return(embot::binary::pair::get(calibstatus, 2)); } // 3 is ok, 1 is not calibrated
        std::uint8_t calibrationOfMAG() const { return(embot::binary::pair::get(calibstatus, 0)); } // 3 is ok, 1 is not calibrated
        //std::uint8_t calibrationOfSYS() const { return(embot::binary::pair::get(calibstatus, 4)); } // 3 is ok, 1 is not calibrated BUT ALWAYS ZERO
    };        
    
    
    
    bool supported(Sensor s);
    
    bool initialised(Sensor s);
    
    // the init() starts the chip, prepares i2c, pings it. if all ok it returns resOK.
    // the working mode is however Mode::CONFIG
    result_t init(Sensor s, const Config &config);
    
    // after that init() returns resOK we can check if it is alive. we can specify a timeout
    bool isalive(Sensor s, embot::common::relTime timeout = 3*embot::common::time1millisec);
    
    // we can get info
    result_t get(Sensor s, Info &info, embot::common::relTime timeout = 3*embot::common::time1millisec);
    
    // we can now set a working mode. we can specify a timeout
    result_t set(Sensor s, Mode m, embot::common::relTime timeout = 3*embot::common::time1millisec);
    
    // we must check that nobody is using the sensor, maybe in non-blocking mode some time earlier
    bool isacquiring(Sensor s);
    
    // we check isacquiring() but also if any other device is using i2c bus
    bool canacquire(Sensor s);
    
    // we start acquisition of a set.
    // if returns resOK, we know that acquisition is over if it is called oncompletion() or when operationdone() is true;
    result_t acquisition(Sensor s, Set set, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
    
    result_t acquisition(Sensor s, Set set, Data &data, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
    
    // it tells if a previous operation (acquisition, read, write) is over
    bool operationdone(Sensor s);
    
    // ok, now we can read data previously acquired
    result_t read(Sensor s, Data &data);
    
    // here are some write() and read() funtions which operate directly on a single register reg 
    
    
    // we write a single byte into a register.
    // blocking or non-blocking mode. we check end of oepration either with operationdone() or with the execution of oncompletion().      
    result_t write(Sensor s, embot::hw::BNO055::Register reg, std::uint8_t value, const embot::common::relTime timeout);
    result_t write(Sensor s, embot::hw::BNO055::Register reg, std::uint8_t value, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));

    // we read from register reg, for data.size positions and we place results into data.pointer (which MUST point to at least data.size bytes)
    // blocking or non-blocking mode. we check end of oepration either with operationdone() or with the execution of oncompletion().  
    result_t read(Sensor s, embot::hw::BNO055::Register reg, embot::common::Data &data, const embot::common::relTime timeout);
    result_t read(Sensor s, embot::hw::BNO055::Register reg, embot::common::Data &data, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));

 
}}} // namespace embot { namespace hw { namespace BNO055 {
    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


