/*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
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

#include "embot_hw_tlv493d.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_tlv493d_bsp.h"

#include <cstring>
#include <vector>
#include <math.h>
#include "embot_core_binary.h"
#include "embot_hw_sys.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


#define PII 3.14159265



#if defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)
#include "embot_os_theCallbackManager.h"
#include "embot_os_Timer.h"

namespace embot { namespace hw { namespace tlv493d {
std::array<embot::os::Timer*, embot::core::tointegral(embot::hw::TLV493D::maxnumberof)> emulatedMODE_timers4callback {nullptr}; 
constexpr uint32_t emulatedMODE_maskofUNresponsivesensors = 0;  // every sensor is responsive
//constexpr uint32_t emulatedMODE_maskofUNresponsivesensors =  
//    embot::core::binary::mask::pos2mask<uint32_t>(embot::hw::TLV493D::one);
//constexpr uint32_t emulatedMODE_maskofUNresponsivesensors =  
//    embot::core::binary::mask::pos2mask<uint32_t>(embot::hw::TLV493D::two) | 
//    embot::core::binary::mask::pos2mask<uint32_t>(embot::hw::TLV493D::six);

#define SINUSOID

#if defined(CHAINSAW)
Position emulatedMODE_getposition(TLV493D h)
{
    static constexpr std::array<Position, embot::core::tointegral(embot::hw::TLV493D::maxnumberof)> min {    0,  4500,  9000, 13500, 18000, 22500};
    static constexpr std::array<Position, embot::core::tointegral(embot::hw::TLV493D::maxnumberof)> max { 4500,  9000, 13500, 18000, 22500, 27000};
    static std::array<Position, embot::core::tointegral(embot::hw::TLV493D::maxnumberof)> pos (min); //{0, 1000, 2000, 3000, 4000, 5000}; 
    
    if(false == supported(h))
    {
        return 0;        
    }
    
    Position p = pos[embot::core::tointegral(h)];
    
    pos[embot::core::tointegral(h)] += 100;
    if(pos[embot::core::tointegral(h)] >= max[embot::core::tointegral(h)])
    {
        pos[embot::core::tointegral(h)] = min[embot::core::tointegral(h)];
    }
    
    return p;
}
#elif defined(SINUSOID)

#include <math.h>
constexpr double pie = 3.14159265;

Position emulatedMODE_getposition(TLV493D h)
{
    static constexpr std::array<Position, embot::core::tointegral(embot::hw::TLV493D::maxnumberof)> min {  500,  500,  500,   500,  500, 500};
    static constexpr std::array<Position, embot::core::tointegral(embot::hw::TLV493D::maxnumberof)> max { 8500,  8500, 8500, 8500, 8500, 8500};
    static std::array<uint16_t, embot::core::tointegral(embot::hw::TLV493D::maxnumberof)> step {0, 0, 0, 0, 0, 0}; 
    
    if(false == supported(h))
    {
        return 0;        
    }
    
    int16_t p = step[embot::core::tointegral(h)];
    
    step[embot::core::tointegral(h)] += 1;
    if(step[embot::core::tointegral(h)] >= 360)
    {
        step[embot::core::tointegral(h)] = 0;
    }
    
    // now i compute teh sine and i move in range [0, 1]
    double v = (sin(p*pie/180.0) + 1.0) / 2.0;
    
    // now in p i put the range
    
    v = min[embot::core::tointegral(h)] + v*(max[embot::core::tointegral(h)] - min[embot::core::tointegral(h)]);
    
    Position pos = static_cast<Position>(v);
    return pos;
}

#else
#error pls define a waveform
#endif

}}}

#endif

using namespace embot::hw;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

#if 0
    volatile int16_t Tv = 0;
    volatile int16_t Xv = 0;
    volatile int16_t Yv = 0;
    volatile int16_t Zv = 0;
    volatile bool Vv = false;    
    
    volatile double AN12 = 0;
    volatile double AN10 = 0;
    volatile double AN08 = 0;
    
    volatile int8_t X8 = 0;
    volatile int8_t Y8 = 0;
    volatile double AN8 = 0;
    
    volatile int32_t angle12 = 0; 
    volatile int32_t angle10 = 0; 
    volatile int32_t angle08 = 0; 
    volatile int32_t angle8 = 0; 
    
    volatile uint32_t power = 0;

#endif

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

std::string embot::hw::tlv493d::to_string(embot::hw::TLV493D id)
{
    constexpr std::array<const char *, embot::core::tointegral(embot::hw::TLV493D::maxnumberof)> TLV493D_map =
    { 
        "TLV493D::one", "TLV493D::two", "TLV493D::three", "TLV493D::four", "TLV493D::five", "TLV493D::six"
    };
    uint8_t pos = embot::core::tointegral(id);
    return (pos < TLV493D_map.size()) ? TLV493D_map[pos] : "TLV493D::none";    
}


#if !defined(EMBOT_ENABLE_hw_tlv493d)

namespace embot { namespace hw { namespace tlv493d {

    bool supported(TLV493D h) { return false; }
    bool initialised(TLV493D h) { return false; }
    result_t init(TLV493D h, const Config &config) { return resNOK; }    
    bool isalive(embot::hw::TLV493D h, embot::core::relTime timeout) { return false; }
    bool isacquiring(embot::hw::TLV493D h) { return false; }
    bool canacquire(embot::hw::TLV493D h) { return false; }    
    result_t acquisition(embot::hw::TLV493D h, const embot::core::Callback &oncompletion) { return resNOK; }
    bool operationdone(embot::hw::TLV493D h) { return false; }
    result_t read(embot::hw::TLV493D h, Position &temp) { return resNOK; }    

}}} // namespace embot { namespace hw { namespace TLV493D {


#else

namespace embot { namespace hw { namespace tlv493d {
              
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(TLV493D h)
    {
        return embot::hw::tlv493d::getBSP().supported(h);
    }
    
    bool initialised(TLV493D h)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(h));
    }    
    
    
    struct RegisterMap
    {
        static constexpr std::uint8_t readsize = 10;
        static constexpr std::uint8_t writesize = 4;
        std::uint8_t readmemory[readsize];
        std::uint8_t writememory[writesize];
        bool isvalid() const 
        {
            if((0b00000000 != (readmemory[3] & 0b00000011))) { return false; }  // CH must  be 00 else a conversion is ongoing
            if((0b00110000 != (readmemory[5] & 0b01110000))) { return false; }     // T+FF+PD (pos 4, 5, 6) must be 011 else ...         
            return true;
        }
        constexpr std::uint16_t getXU() const { return static_cast<std::uint16_t>( (static_cast<std::uint16_t>(readmemory[0]) << 4) | static_cast<std::uint16_t>(readmemory[4]>>4) ); }  
        constexpr std::uint16_t getYU() const { return static_cast<std::int16_t>( (static_cast<std::uint16_t>(readmemory[1]) << 4) | static_cast<std::uint16_t>(readmemory[4]&0x0F) ); }             
        constexpr std::uint16_t getZU() const { return static_cast<std::uint16_t>( (static_cast<std::uint16_t>(readmemory[2]) << 4) | static_cast<std::uint16_t>(readmemory[5]&0x0F) ); }
        constexpr std::uint16_t getTU() const { return static_cast<std::uint16_t>( (static_cast<std::uint16_t>(readmemory[3]&0xF0) << 4) | static_cast<std::uint16_t>(readmemory[6]) ); }
        static constexpr std::int16_t toi16(std::uint16_t u) { return ( (0==(u&0x0800)) ? u : ((0x07ff&u)-2048) ); } 
        constexpr std::int16_t getX() const { return toi16(getXU()); }
        constexpr std::int16_t getY() const { return toi16(getYU()); }
        constexpr std::int16_t getZ() const { return toi16(getZU()); }
        
        constexpr std::int16_t getT() const { return toi16(getTU())-340; }
        
        constexpr std::uint8_t getInitialWRITE0() const { return 0; }
        constexpr std::uint8_t getInitialWRITE1() const { return readmemory[7] & 0x18; }
        constexpr std::uint8_t getInitialWRITE2() const { return readmemory[8]; }
        constexpr std::uint8_t getInitialWRITE3() const { return readmemory[9] & 0x1f; }
// ok     void setWRITE(uint8_t paritybit = 0b1, uint8_t i2caddr = 0b00, uint8_t intena = 0b0, uint8_t fastena = 0b1, uint8_t lowena = 0b0, 
//                      uint8_t tempena = 0b0, uint8_t lpperiod = 0b0, uint8_t parena = 0b0)
        struct config_t
        {
            enum class adr : uint8_t { zero = 0, one = 1, two = 2, three = 3 }; 
            uint32_t    paritybit   : 1;
            uint32_t    i2caddr     : 2;
            uint32_t    intena      : 1; 
            uint32_t    fastena     : 1;
            uint32_t    lowena      : 1;
            uint32_t    tempena     : 1;
            uint32_t    lpperiod    : 1;
            uint32_t    parena      : 1;
            constexpr config_t(uint8_t _paritybit, uint8_t _i2caddr, uint8_t _intena, uint8_t _fastena, uint8_t _lowena, 
                uint8_t _tempena, uint8_t _lpperiod, uint8_t _parena) :
                paritybit(_paritybit), i2caddr(_i2caddr), intena(_intena), fastena(_fastena), lowena(_lowena), tempena(_tempena), lpperiod(_lpperiod), parena(_parena)
            {
            }
            constexpr config_t() :
                paritybit(0b1), i2caddr(0b00), intena(0b0), fastena(0b1), lowena(0b0), tempena(0b0), lpperiod(0b0), parena(0b0)
            {
            }
            constexpr config_t(adr a) :
                paritybit(0b1), i2caddr(embot::core::tointegral(a)), intena(0b0), fastena(0b1), lowena(0b0), tempena(0b0), lpperiod(0b0), parena(0b0)
            {
            }
        };
       

        void setWRITE(const config_t &cfg)
        {
            writememory[0] = getInitialWRITE0();
            writememory[1] = getInitialWRITE1() | ((cfg.paritybit&0b1) << 7) | ((cfg.i2caddr&0b11) << 5) | ((cfg.intena&0b1) << 2) | ((cfg.fastena&0b1) << 1) | ((cfg.lowena&0b1));
            writememory[2] = getInitialWRITE2();
            writememory[3] = getInitialWRITE3() | ((cfg.tempena&0b1) << 7) | ((cfg.lpperiod&0b1) << 6) | ((cfg.parena&0b1) << 5);
        }          

        std::int8_t getX08() const { return ( (readmemory[0] & 0x7f) - (128*(readmemory[0]>>7)) ); }  
        std::int8_t getY08() const { return ( (readmemory[1] & 0x7f) - (128*(readmemory[1]>>7)) ); }
        
    };
    
    static constexpr RegisterMap::config_t defconfig { };
    
      

    struct Acquisition
    {
        static constexpr std::uint8_t rxdatasize = 8;
        TLV493D id;
        volatile bool done;
        volatile bool ongoing;
        Position position;
        RegisterMap registermap;;
        embot::core::Callback userdefCBK;
        void init(TLV493D _id) { clear(); id = _id; }        
        void clear() { done = false; ongoing = false; position = 0; std::memset(registermap.readmemory, 0, sizeof(registermap.readmemory)); userdefCBK.clear(); }         
    };
    
    
    struct PrivateData
    {
        embot::hw::i2c::Descriptor i2cdes[embot::core::tointegral(TLV493D::maxnumberof)];   
        Config config[embot::core::tointegral(TLV493D::maxnumberof)];        
        Acquisition acquisition[embot::core::tointegral(TLV493D::maxnumberof)];
        PrivateData() { }
    };
    

    // this device works with no register addressing.
    // static const embot::hw::i2c::Reg registerToRead {embot::hw::i2c::Reg::addrNONE, embot::hw::i2c::Reg::Size::eightbits};
    
    static PrivateData s_privatedata;

        
    result_t s_sensor_reset(TLV493D h);
    
    result_t s_sensor_init(TLV493D h); 
    
    static void sharedCBK(void *p)
    {
        Acquisition *acq = reinterpret_cast<Acquisition*>(p);    
        
        //std::int16_t tempval = acq->registermap.getT();
        //bool isava = acq->registermap.isvalid();               
        
        volatile std::int16_t Xv = acq->registermap.getX();
        volatile std::int16_t Yv = acq->registermap.getY();
        //constexpr double todeg = 57.29577957855;  
        constexpr double tocentdeg = 5729.577957855;
        volatile double an12atan2 = atan2(Yv, Xv) * tocentdeg + 18000.0;

        acq->position = static_cast<Position>(an12atan2);
#if defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)
        acq->position = emulatedMODE_getposition(acq->id);
#endif        
        acq->ongoing = false;
        acq->done = true;
        
        acq->userdefCBK.execute();
    }
              
    result_t init(TLV493D h, const Config &config)
    {
        if(false == supported(h))
        {
            return resNOK;
        }
        
        if(true == initialised(h))
        {
            return resOK;
        }
        
        
        std::uint8_t index = embot::core::tointegral(h);
               
        
#if !defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)        
        // init peripheral
        embot::hw::tlv493d::getBSP().init(h);                             
        // init i2c with the relevant bus specified by the bsp for this chip
        embot::hw::i2c::init(embot::hw::tlv493d::getBSP().getPROP(h)->i2cdes.bus, {});
#endif 
        
        // load config etc
        s_privatedata.i2cdes[index] = embot::hw::tlv493d::getBSP().getPROP(h)->i2cdes;
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].init(h);

#if !defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)          
        // sensor init
        if(resOK != s_sensor_init(h))
        {
            return resNOK;
        }            
#else
        // we emulate the sensor action with a timer which executes the callback
        emulatedMODE_timers4callback[embot::core::tointegral(h)] = new embot::os::Timer;  
#endif

        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(h));                
        return resOK;
    }

    
    bool isacquiring(TLV493D h)
    {
        if(false == initialised(h))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(h);        
        return s_privatedata.acquisition[index].ongoing;     
    }
    
    
    bool canacquire(TLV493D h)
    {
        if(false == initialised(h))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(h);  
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return false;
        }

#if defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)             
        return true;
#endif         
        return !embot::hw::i2c::isbusy(s_privatedata.i2cdes[index].bus);             
    }    
    
    result_t acquisition(TLV493D h, const embot::core::Callback &oncompletion)
    {
        if(false == canacquire(h))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::core::tointegral(h);
                
        s_privatedata.acquisition[index].clear();
        s_privatedata.acquisition[index].ongoing = true;
        s_privatedata.acquisition[index].done = false;
        s_privatedata.acquisition[index].userdefCBK = oncompletion;              
        
        // ok, now i trigger i2c.
        embot::core::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        embot::core::Data data = embot::core::Data(&s_privatedata.acquisition[index].registermap.readmemory[0], sizeof(s_privatedata.acquisition[index].registermap.readmemory));
#if defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)
        // we config the timer to execute the callback only if the sensor is responsive
        if(false == embot::core::binary::bit::check(emulatedMODE_maskofUNresponsivesensors, embot::core::tointegral(h)))
        {
            emulatedMODE_timers4callback[index]->stop();
            embot::os::Action act { embot::os::CallbackToThread(cbk, embot::os::theCallbackManager::getInstance().thread()) };
            embot::os::Timer::Config tcfg(1*embot::core::time1millisec, act, embot::os::Timer::Mode::oneshot);
            emulatedMODE_timers4callback[index]->start(tcfg);
            //cbk.execute();            
        }     
        return resOK;
#endif         
        embot::hw::i2c::receive(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, data, cbk);                
        return resOK;
    }
    
    bool isalive(TLV493D h, embot::core::relTime timeout)
    {
        if(false == initialised(h))
        {
            return false;
        } 
#if defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)
        return true;
#endif        
        std::uint8_t index = embot::core::tointegral(h);
        return embot::hw::i2c::ping(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, timeout);  
    }

    
    bool operationdone(TLV493D h)
    {
        if(false == initialised(h))
        {
            return false;
        } 

        return s_privatedata.acquisition[embot::core::tointegral(h)].done;        
    } 
    
    

    
    result_t read(TLV493D h, Position &position)
    {
        if(false == initialised(h))
        {
            return resNOK;
        } 

        if(false == operationdone(h))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::core::tointegral(h);
        position = s_privatedata.acquisition[index].position;
        
        return resOK;  
        
#if 0   // tests        
        
        constexpr double todeg = 57.29577957855;    
        

        
        
        Vv = s_privatedata.acquisition[index].registermap.isvalid();
        Tv = s_privatedata.acquisition[index].registermap.getT();
        Xv = s_privatedata.acquisition[index].registermap.getX();
        Yv = s_privatedata.acquisition[index].registermap.getY();
        Zv = s_privatedata.acquisition[index].registermap.getZ();
        
        
//        static volatile int16_t XMAX = -20000;
//        static volatile int16_t XMIN = +20000;
//        static volatile int16_t YMAX = -20000;
//        static volatile int16_t YMIN = +20000;
//        if(XMAX < Xv) XMAX = Xv;
//        if(YMAX < Yv) YMAX = Yv;
//        if(XMIN > Xv) XMIN = Xv;
//        if(YMIN > Xv) YMIN = Xv;
//        
//        static volatile double Xscale = 1;
//        Xscale = (XMAX-XMIN)/2.0;
//        static volatile double Yscale = 1;
//        Yscale = (YMAX-YMIN)/2.0;
//        static volatile double Y = 1;
//        Y = Yv / Yscale;
//        static volatile double X = 1;
//        X = Xv / Xscale;
//        
//        
//        
//        static volatile double ratio = 0;        
//        ratio = (X != 0) ? (Y/X) : 0;
//        static volatile double ANC = 0;
//        ANC = atan(ratio) * todeg;
        
        
        AN12 = atan(static_cast<double>(Yv)/static_cast<double>(Xv)) * todeg;

        
        int16_t y10 = Yv/4;
        int16_t x10 = Xv/4;
        AN10 = atan(static_cast<double>(y10)/static_cast<double>(x10)) * todeg;


        int16_t y08 = Yv/16;
        int16_t x08 = Xv/16;
        AN08 = atan(static_cast<double>(y08)/static_cast<double>(x08)) * todeg;
        
        
        X8 = s_privatedata.acquisition[index].registermap.getX08();
        Y8 = s_privatedata.acquisition[index].registermap.getY08();
        if(0 == X8)
        {
            if(0 == Y8)
            {   
                AN8 = AN8; // previous value ...
            }
            else
            {
                AN8 = (Y8>0) ? (+90.0) : (-90.0);
            }
        }
        else
        {
            AN8 = atan(static_cast<double>(Y8)/static_cast<double>(X8)) * todeg;
        }
        
        angle12 = floor(100*AN12);
        angle10 = floor(100*AN10);
        angle8 = floor(100*AN8);
        angle08 = floor(100*AN08);
        
        power = Xv*Xv + Yv*Yv;
        
        static volatile double an12atan2 = 0; 
        an12atan2 = atan2(Yv, Xv) * todeg;
        
  
        return resOK;  
#endif // tests        
    }
    

//#define embot_hw_tlv493d_discover
#if defined(embot_hw_tlv493d_discover)
    static std::vector<embot::hw::i2c::ADR> addresses;    
    volatile bool ff {false};
#endif
    
    result_t s_sensor_init(TLV493D h)
    {
        std::uint8_t index = embot::core::tointegral(h);
        
        std::uint8_t txdata[1] = {0};
        embot::core::Data data = embot::core::Data(txdata, 1);
        volatile result_t r = resOK;
        
        // see: Figure 9 Sequence for power-up and sensor initialization for single use (pag 15 of user manual of chip)
        
         
        // 1. reset the chip and wait for some time         
//        s_sensor_reset(h);
           
        // 1.a make sure the chip has a good address in the bus         
        if(false == embot::hw::i2c::ping(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, 3*embot::core::time1millisec))
        {
            #if defined(embot_hw_tlv493d_discover)
            // discover the boards
            ff = embot::hw::i2c::discover(s_privatedata.config[index].i2cdes.bus, addresses);
            ff = ff;
            #endif
            return resNOK;
        }
                       
        // 2. read the registers
        data.load(s_privatedata.acquisition[index].registermap.readmemory, s_privatedata.acquisition[index].registermap.readsize); 
        //r = embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, registerToRead, data, embot::core::time1second);
        r = embot::hw::i2c::receive(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, data, embot::core::time1second);
        r = r;
        
        // 3. impose a mode.
        s_privatedata.acquisition[index].registermap.setWRITE({}); // defconfig
        data.load(s_privatedata.acquisition[index].registermap.writememory, s_privatedata.acquisition[index].registermap.writesize); 
        //r = embot::hw::i2c::write(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, registerToRead, data, embot::core::time1second);
        r = embot::hw::i2c::transmit(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, data, embot::core::time1second);
        
        // wait a bit
        embot::hw::sys::delay(10*embot::core::time1millisec);
        
        return resOK;        
    }
    
    result_t s_sensor_reset(TLV493D h)
    {
        std::uint8_t index = embot::core::tointegral(h);
        
        // see: Figure 15 Reset frame 0x00 with address setting (pag 21 of user manual of chip)
        // we transmit 0x00 on the bus and wait for some time (at least 14usec)
        // we do that by transmitting to address 0x00 a total of 0 bytes.
        
        embot::core::Data dummy;
        dummy.clear();
        volatile result_t r1 = embot::hw::i2c::tx(s_privatedata.i2cdes[index].bus, 0x00, dummy, 3*embot::core::time1millisec);             
        r1 = r1;
        // extra 3 ms.
        embot::hw::sys::delay(3*embot::core::time1millisec);

        return resOK;        
    }
    
 
}}} // namespace embot { namespace hw { namespace tlv493d {



#endif //defined(EMBOT_TLV493D_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

