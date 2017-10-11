
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
        CHIP_ID         = 0x00, // 7 bytes RO with many info:  
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

    enum class DataSet
    {
        A               = 6,        // acc: 6 bytes
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
    
    
//    struct Triple
//    {
//        std::int16_t    x;
//        std::int16_t    y;
//        std::int16_t    z;
//        Triple() { reset(); }
//        Triple(std::int16_t _x, std::int16_t _y, std::int16_t _z) { load(_x, _y, _z); }
//        void load(std::int16_t _x, std::int16_t _y, std::int16_t _z) { x = _x; y = _y; z = _z; }  
//        void reset() { x = 0; y = 0; z = 0; }
//        void load(void *littleendianmemory) { std::uint16_t *u16 = reinterpret_cast<std::uint16_t*>(littleendianmemory); x = u16[0]; y = u16[1]; z = u16[2]; }
//    };

//    struct Quadruple
//    {
//        std::int16_t    w;
//        std::int16_t    x;
//        std::int16_t    y;
//        std::int16_t    z;
//        Quadruple() { reset(); }
//        Quadruple(std::int16_t _w, std::int16_t _x, std::int16_t _y, std::int16_t _z) { load(_w, _x, _y, _z); }
//        void load(std::int16_t _w, std::int16_t _x, std::int16_t _y, std::int16_t _z) { w = _w; x = _x; y = _y; z = _z; }  
//        void reset() { w = 0; x = 0; y = 0; z = 0; }
//        void load(void *littleendianmemory) { std::uint16_t *u16 = reinterpret_cast<std::uint16_t*>(littleendianmemory); w = u16[0]; x = u16[1]; y = u16[2]; z = u16[3]; }
//    };  
    
//    struct Fulldata
//    {
//        Triple          acc;
//        Triple          mag;
//        Triple          gyr;
//        Triple          eul;
//        Quadruple       qua;
//        Triple          lia;
//        Triple          grv;
//        std::uint8_t    temperature;
//        std::uint8_t    calibstatus;
//        std::uint8_t    systemstatus;  
//        Fulldata() { temperature = 0; calibstatus = 0; systemstatus = 0; }
//        void load(void *mem) 
//        { 
//            std::uint8_t *m = reinterpret_cast<std::uint8_t*>(mem); 
//            acc.load(m); mag.load(&m[6]); gyr.load(&m[12]); eul.load(&m[18]);
//            qua.load(&m[24]);
//            lia.load(&m[32]); grv.load(&m[38]);
//            temperature = m[44]; calibstatus = m[45]; systemstatus = m[46];
//        }
//    };  

    struct Fulldata
    {
        embot::common::Triple<std::int16_t>     acc;
        embot::common::Triple<std::int16_t>     mag;
        embot::common::Triple<std::int16_t>     gyr;
        embot::common::Triple<std::int16_t>     eul;
        embot::common::Quadruple<std::int16_t>  qua;
        embot::common::Triple<std::int16_t>     lia;
        embot::common::Triple<std::int16_t>     grv;
        std::uint8_t                            temperature;
        std::uint8_t                            calibstatus;
        std::uint8_t                            systemstatus;  
        Fulldata() { temperature = 0; calibstatus = 0; systemstatus = 0; }
        void load(void *mem) 
        { 
            std::uint8_t *m = reinterpret_cast<std::uint8_t*>(mem); 
            acc.load(m); mag.load(&m[6]); gyr.load(&m[12]); eul.load(&m[18]);
            qua.load(&m[24]);
            lia.load(&m[32]); grv.load(&m[38]);
            temperature = m[44]; calibstatus = m[45]; systemstatus = m[46];
        }
    };        
    
    bool supported(Sensor s);
    
    bool initialised(Sensor s);
    
    result_t init(Sensor s, const Config &config);
        
    bool isalive(Sensor s);
    
    bool isbusbusy(Sensor s);
    

    result_t set(Sensor s, Mode m, embot::common::relTime timeout);
        
    
//    result_t read(Sensor s, embot::hw::BNO055::Register reg, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
//    result_t read(Sensor s, embot::hw::BNO055::Register reg, std::uint8_t &value, const embot::common::relTime timeout);
    
    
    // i write value into register reg. 
    result_t write(Sensor s, embot::hw::BNO055::Register reg, std::uint8_t value, const embot::common::relTime timeout);
    result_t write(Sensor s, embot::hw::BNO055::Register reg, std::uint8_t value, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
    
    // i read from register reg, for data.size positions and i place results into data.pointer (which MUST point to at least data.size bytes)
    result_t read(Sensor s, embot::hw::BNO055::Register reg, embot::common::Data &data, const embot::common::relTime timeout);
    result_t read(Sensor s, embot::hw::BNO055::Register reg, embot::common::Data &data, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));


    result_t get(Sensor s, Info &info, embot::common::relTime timeout);
    
    // usage 1: 
    // a. we call acquistion() with a callback. 
    // b. at expiry of the callback we verify with isready() and then we read with read().
    // usage 2:
    // a. we call acquistion() without a callback.
    // b. we loop until isready() returns true.
    // c. we read with read()
    result_t acquisition(Sensor s, std::uint8_t reg, std::uint8_t size, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
    result_t acquisition(Sensor s, DataSet dataset, const embot::common::Callback &oncompletion = embot::common::Callback(nullptr, nullptr));
    bool isacquiring(Sensor s);
    bool isready(Sensor s);
    
//    result_t read(Sensor s, Triple &triple);  
    result_t read(Sensor s, std::uint8_t &value); 
    result_t read(Sensor s, embot::common::Data &data);    

    
    result_t read(Sensor s, Fulldata &fulldata);
 
}}} // namespace embot { namespace hw { namespace BNO055 {
    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


