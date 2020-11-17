
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Andrea Mura
 * email:   marco.accame@iit.it, andrea.mura@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_ads122c04.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_ads122c04_bsp.h"

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


#if !defined(EMBOT_ENABLE_hw_ads122c04)

namespace embot { namespace hw { namespace ads122c04 {

    bool supported(ADS122C04 s)                                                                         { return false; }
    bool initialised(ADS122C04 s)                                                                       { return false; }
    result_t init(ADS122C04 s, const Config &config)                                                    { return resNOK; }    
    bool isalive(embot::hw::ADS122C04 s, embot::core::relTime timeout)                                  { return false; }    
    bool isacquiring(embot::hw::ADS122C04 s)                                                            { return false; }    
    bool canacquire(embot::hw::ADS122C04 s)                                                             { return false; }        
    result_t acquisition(embot::hw::ADS122C04 s, const embot::core::Callback &oncompletion)             { return resNOK; }
    bool operationdone(embot::hw::ADS122C04 s)                                                          { return false; }
    result_t read(embot::hw::ADS122C04 s, Value &temp)                                                  { return resNOK; }   

}}} // namespace embot { namespace hw { namespace ADS122C04 {


#else

namespace embot { namespace hw { namespace ads122c04 {
              
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(ADS122C04 a)
    {
        return embot::hw::ads122c04::getBSP().supported(a);
    }
    
    bool initialised(ADS122C04 a)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(a));
    }    
      

    struct Acquisition
    {
        static constexpr uint8_t sizeofdata = 3; // 24 bits, one sample
        volatile bool done {false};
        volatile bool ongoing {false};
        Channel channel {Channel::one};
        Values values {};
        std::uint8_t rxdata[sizeofdata] {0};
        embot::core::Callback userdefCBK {};  
        void clear() { done = false; ongoing = false; channel = Channel::one; memset(rxdata, 0, sizeof(rxdata)); userdefCBK.clear(); }        
        Acquisition() = default;        
    };
    
    struct PrivateData
    {
        embot::hw::i2c::Descriptor i2cdes[embot::core::tointegral(AD7147::maxnumberof)];     
        Config config[embot::core::tointegral(ADS122C04::maxnumberof)];        
        Acquisition acquisition[embot::core::tointegral(ADS122C04::maxnumberof)];
        PrivateData() = default;
    };
    
    
    
    static PrivateData s_privatedata;
    
    static void sharedCBK(void *p)
    {
        Acquisition *acq = reinterpret_cast<Acquisition*>(p);        
        
#if defined(ads122c04_FAKEmode)        
        static volatile uint32_t tmp = 0;
        tmp++;
        acq->values.v1 = tmp;
        acq->values.v2 = tmp+1;
#else
              
        uint32_t tmp = (static_cast<uint32_t>(acq->rxdata[0]) << 16) + (static_cast<uint32_t>(acq->rxdata[1]) << 8) + static_cast<uint32_t>(acq->rxdata[2]);
        
        if(Channel::one == acq->channel)
        {
            acq->values.v1 = tmp;
        }
        else if(Channel::two == acq->channel)
        {
            acq->values.v2 = tmp;
        }
       
#endif        
        acq->ongoing = false;
        acq->done = true;
               
        acq->userdefCBK.execute();
    }
    
    // build the registers of the ADS upon embot::hw::REG<>

    struct REG0 : public REG<reg08type, 3>
    {   // [ mux:4 | gain:3 | bypass:1 ]
        enum class FIELD : uint8_t { bypass = 0, gain = 1, mux = 2 };
        static constexpr REG::Fields fields {1, 3, 4};
        REG0(reg08type *mem = nullptr) : REG<reg08type, 3>(mem, &fields) {}
            
        static constexpr uint8_t address {0};            
        void load(reg08type *mem) { memory = mem; }                
        bool set(const FIELD tag, const uint8_t value) { return setfield(embot::core::tointegral(tag), value); }
        reg08type get(const FIELD tag) { return getfield(embot::core::tointegral(tag)); }
    };    
    
    struct REG1 : public REG<reg08type, 5>
    {   // [ dr:3 | mode:1 | cm:1 | vref:2 | ts:1 ]
        enum class FIELD : uint8_t { ts = 0, vref = 1, cm = 2, mode = 3, dr = 4 };
        static constexpr REG::Fields fields {1, 2, 1, 1, 3};
        REG1(reg08type *mem = nullptr) : REG<reg08type, 5>(mem, &fields) {}
            
        static constexpr uint8_t address {1};            
        void load(reg08type *mem) { memory = mem; }                
        bool set(const FIELD tag, const uint8_t value) { return setfield(embot::core::tointegral(tag), value); }
        reg08type get(const FIELD tag) { return getfield(embot::core::tointegral(tag)); }
    };    
           
    struct REG2 : public REG<reg08type, 5>
    {   // [ drdy:1 | dcnt:1 | crc:2 | bcs:1 | idad:3 ]
        enum class FIELD : uint8_t { idad = 0, bcs = 1, crc = 2, dcnt = 3, drdy = 4 };
        static constexpr REG::Fields fields {3, 1, 2, 1, 1};
        REG2(reg08type *mem = nullptr) : REG<reg08type, 5>(mem, &fields) {}
            
        static constexpr uint8_t address {2};            
        void load(reg08type *mem) { memory = mem; }                
        bool set(const FIELD tag, const uint8_t value) { return setfield(embot::core::tointegral(tag), value); }
        reg08type get(const FIELD tag) { return getfield(embot::core::tointegral(tag)); }
    };      
   
    struct REG3 : public REG<reg08type, 3>
    {   // [ i1mux:3 | i2mux:3 | unused:2 ]
        enum class FIELD : uint8_t { unused = 0, i2mux = 3, i1mux = 3 };
        static constexpr REG::Fields fields {2, 3, 3};
        REG3(reg08type *mem = nullptr) : REG<reg08type, 3>(mem, &fields) {}
            
        static constexpr uint8_t address {3};            
        void load(reg08type *mem) { memory = mem; }                
        bool set(const FIELD tag, const uint8_t value) { return setfield(embot::core::tointegral(tag), value); }
        reg08type get(const FIELD tag) { return getfield(embot::core::tointegral(tag)); }
    }; 
    
//#define ADS122C04_RESET         0x06
//#define ADS122C04_START         0x08
//#define ADS122C04_POWERDOWN     0x02
//#define ADS122C04_RDATA         0x10
//#define ADS122C04_RREG          0x20
//#define ADS122C04_WREG          0x40
//#define ADS122C04_REGISTER0     0
//#define ADS122C04_REGISTER1     1
//#define ADS122C04_REGISTER2     2
//#define ADS122C04_REGISTER3     3

// REGISTER0
//#define ADS122C04_CHANNEL1          0x00        // MUX[3:0] = 0000 : AINP = AIN0, AINN = AIN1
//#define ADS122C04_CHANNEL2          0x06        // MUX[3:0] = 0110 : AINP = AIN2, AINN = AIN3
//#define ADS122C04_GAINx1            0x00        // GAIN[2:0]
//#define ADS122C04_PGA_BYPASS_EN     0x00        // PGA enabled (default)
//#define ADS122C04_PGA_BYPASS_DIS    0x01        // PGA disabled and bypassed

// REGISTER1
//#define ADS122C04_DR_1kSPS          0x06        // DR[2:0]
//#define ADS122C04_NORMAL            0x00        // 0 : Normal mode (256-kHz modulator clock, default)
//#define ADS122C04_TURBO             0x01        // 1 : Turbo mode (512-kHz modulator clock)
//#define ADS122C04_SINGLESHOT        0x00        // Single-shot conversion mode
//#define ADS122C04_CM                0x01        // Continuous conversion mode

    
    // use a struct to model the behaviour of the ADS
    
    struct chipADS122C04
    {
        embot::hw::I2C bus { embot::hw::I2C::one};
        embot::hw::i2c::ADR adr {0};
        embot::core::relTime conversiontime {1040};
        
        enum class REG : uint8_t { zero = 0, one = 1, two = 2, three = 3 };
        
        enum class CMD : uint8_t { reset = 0x06, start = 0x08, powerdown = 0x02, none = 0xff };
        
        static constexpr std::uint8_t RDATA = 0x10;
        static constexpr std::uint8_t RREG  = 0x20;
        static constexpr std::uint8_t WREG  = 0x40;
        static constexpr std::uint8_t CHANNEL1 = 0x00;
        static constexpr std::uint8_t CHANNEL2 = 0x06;
        static constexpr std::uint8_t DR_1kSPS = 0x06;
        static constexpr std::uint8_t CM_SINGLESHOT = 0x00;
        
        // direct access to registers w/ set() / get()
        REG0 r0 {&memory[0]};
        REG1 r1 {&memory[1]};
        REG2 r2 {&memory[2]};
        REG3 r3 {&memory[3]};
        
        
        uint8_t memory[4] {0};  
        
        chipADS122C04() = default; 

        void setaddress(embot::hw::I2C b, embot::hw::i2c::ADR a)
        {
            bus = b;
            adr = a;            
        }
        
        result_t writeregister(REG r, embot::core::relTime timeout = 3*embot::core::time1millisec)
        {
            uint8_t regWrite[2] = {0};
            regWrite[0] = WREG + (embot::core::tointegral(r)<<2);
            regWrite[1] = memory[embot::core::tointegral(r)];
            return embot::hw::i2c::tx(bus, adr, {&regWrite, 2}, timeout);
        } 
        
        result_t readregister(REG r, embot::core::relTime timeout = 3*embot::core::time1millisec)
        {
            uint8_t reg = RREG + (embot::core::tointegral(r)<<2);
            embot::core::Data dest = {&memory[embot::core::tointegral(r)], 1};
            return embot::hw::i2c::read(bus, adr, {reg, embot::hw::i2c::Reg::Size::eightbits}, dest, timeout);
        }  
                
        result_t readeveryregister(embot::core::relTime timeout = 12*embot::core::time1millisec)
        {
            result_t res = readregister(REG::zero, timeout/4);
            
            if(result_t::OK == res)
                res = readregister(REG::one, timeout/4);
            if(result_t::OK == res)
                res = readregister(REG::two, timeout/4);
            if(result_t::OK == res)
                res = readregister(REG::three, timeout/4);
            
            return res;
        } 

        result_t sendcommand(CMD cmd, embot::core::relTime timeout = 3*embot::core::time1millisec)
        {
            volatile result_t res = result_t::NOK;     
            volatile uint8_t v1 = 0;        
            // 1. reset
            uint8_t command {embot::core::tointegral(CMD::none)}; 
            embot::core::relTime delay = 0;
            switch(cmd)
            {
                case CMD::reset:
                {
                    delay = embot::core::time1millisec;
                    command = embot::core::tointegral(CMD::reset);
                } break;
                
                case CMD::start:
                {
                    delay = 0;
                    command = embot::core::tointegral(CMD::start);
                } break;
                            
                default:
                {                    
                } break;
            }
            
            if(255 != command)
            {
                res = embot::hw::i2c::tx(bus, adr, {&command, 1}, timeout);
                if(delay > 0)
                {
                    embot::hw::sys::delay(delay);  
                }
            }            

            return res;                        
        }
        
        result_t setchannel(Channel chn, embot::core::relTime timeout = 3*embot::core::time1millisec)
        {
            result_t res = result_t::NOK;
            volatile uint8_t v1 = 0;
            r0.set(REG0::FIELD::mux, (Channel::one == chn) ? CHANNEL1 : CHANNEL2);
            v1 = r0.get(REG0::FIELD::mux);
            res = writeregister(chipADS122C04::REG::zero, timeout);                
            return res;
        }
        
        enum class CFG : uint8_t { normal = 0, turbo = 1  };
        result_t sendconfig(CFG cfg, embot::core::relTime timeout = 3*embot::core::time1millisec)
        {
            switch(cfg)
            {
                default:
                case CFG::normal:
                {
                    r1.set(REG1::FIELD::cm, CM_SINGLESHOT);
                    // normal mode @ 1000 sps
                    r1.set(REG1::FIELD::mode, 0); 
                    r1.set(REG1::FIELD::dr, DR_1kSPS);                
                    conversiontime = 1040; // see datasheet page 28 (8.3.6 Conversion Times)                   
                } break;

                case CFG::turbo:
                {
                    r1.set(REG1::FIELD::cm, CM_SINGLESHOT);
                    // turbo mode @ 2000 sps
                    r1.set(REG1::FIELD::mode, 1); 
                    r1.set(REG1::FIELD::dr, DR_1kSPS);
                    conversiontime = 520;  // see datasheet page 28 (8.3.6 Conversion Times)                   
                } break;
            }

            return writeregister(chipADS122C04::REG::one, timeout);
        }

/*
8.4.3.2 Turbo Mode
Applications that require higher data rates up to 2 kSPS can operate the device in turbo mode. In this mode, the
internal modulator runs at a higher frequency of fMOD = fCLK / 4 = 512 kHz. Compared to normal mode, the device
power consumption increases because the modulator runs at a higher frequency. Running the ADS122C04 in
turbo mode at a comparable output data rate as in normal mode yields better noise performance. For example,
the input-referred noise at 90 SPS in turbo mode is lower than the input-referred noise at 90 SPS in normal
mode.                
*/
        enum class SPS : uint8_t { twothousand = 6, onethousandtwohundred = 5, sixhundredsixty = 4, threehundredfifty = 3 };
        enum class GAIN : uint8_t { one = 0, two = 1, four = 2, eigth = 3, sixteen = 4, thirtytwo = 5, sixtyfour = 6, onehundredtwentyeigth = 7 };
        result_t configure(SPS sps, GAIN gain, embot::core::relTime timeout = 3*embot::core::time1millisec)
        {
            static constexpr embot::core::relTime LUTconversiontimesturbo[] = 
            {   // as indexed by valure of register DR. see see datasheet page 28 (8.3.6 Conversion Times) and page 41 (8.6.2 Register Descriptions)
                25010, 11140, 5650, 2900, 1540, 860, 520 
            };
            
            // single shot, always turbo mode
            r1.set(REG1::FIELD::cm, CM_SINGLESHOT);
            r1.set(REG1::FIELD::mode, 1);
            
            // the values of GAIN and SPS are already the values of the related registers ...
            r0.set(REG0::FIELD::gain, embot::core::tointegral(gain));
            r1.set(REG1::FIELD::dr, embot::core::tointegral(sps));
            conversiontime = LUTconversiontimesturbo[embot::core::tointegral(sps)];
            
            // now i write the registers
            result_t r = writeregister(chipADS122C04::REG::zero, timeout);
            if(result_t::OK != r)
            {
                return r;
            }
            return writeregister(chipADS122C04::REG::one, timeout);
        }  
        
        // blocking mode
        result_t startconversion()
        {
            return sendcommand(chipADS122C04::CMD::start); 
        }
        
        // after startconversion() we must wait for some time that the conversion is effectively done
        embot::core::relTime getConversionTime() const
        {
            return conversiontime;
        }   
        
        // non blocking mode: at end it puts results inside data and execute the callback
        result_t retrievevalue(embot::core::Data &destination, const embot::core::Callback &oncompletion)
        {
            return embot::hw::i2c::read(bus, adr, chipADS122C04::RDATA, destination, oncompletion);
        }
        


    }; 


    // in future, if multiple chips are required: do it a array and put it into s_privatedata;
    chipADS122C04 _ads_chip;
    
              
    result_t init(ADS122C04 s, const Config &config)
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
        embot::hw::ads122c04::getBSP().init(s);
        
        std::uint8_t index = embot::core::tointegral(s);
        
#if defined(ads122c04_FAKEmode)
#else        
                
        // init i2c with the relevant bus specified by the bsp for this chip
        embot::hw::i2c::init(embot::hw::ads122c04::getBSP().getPROP(s)->i2cdes.bus, {});
        if(false == embot::hw::i2c::ping(embot::hw::ads122c04::getBSP().getPROP(s)->i2cdes.bus, embot::hw::ads122c04::getBSP().getPROP(s)->i2cdes.adr, 3*embot::core::time1millisec))
        {
            return resNOK;
        }
                       
#endif        
        s_privatedata.i2cdes[index] = embot::hw::ads122c04::getBSP().getPROP(s)->i2cdes;
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].clear();
        
#if defined(ads122c04_FAKEmode)
#else          
        // we need to perform chip initialization
        
        _ads_chip.setaddress(embot::hw::ads122c04::getBSP().getPROP(s)->i2cdes.bus, embot::hw::ads122c04::getBSP().getPROP(s)->i2cdes.adr);
        
        volatile result_t res = result_t::NOK;     
        // 1. reset
        _ads_chip.sendcommand(chipADS122C04::CMD::reset);
        
        // 2. configure registers                 
        res = _ads_chip.configure(chipADS122C04::SPS::threehundredfifty, chipADS122C04::GAIN::onehundredtwentyeigth, 3*embot::core::time1millisec);
       
//        // 3. sanity check: reading back the config registers.
//        memset(_ads_chip.memory, 0, sizeof(_ads_chip.memory));
//        res = _ads_chip.readeveryregister();
//        volatile uint8_t v1 = _ads_chip.r1.get(REG1::FIELD::dr);
//        v1 = v1;        
#endif        
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(s));
                
        return resOK;
    }

    
    bool isacquiring(ADS122C04 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(s);        
        return s_privatedata.acquisition[index].ongoing;     
    }
    
    
    bool canacquire(ADS122C04 s)
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
#if defined(ads122c04_FAKEmode)
        return true;
#else         
        return !embot::hw::i2c::isbusy(s_privatedata.i2cdes[index].bus);  
#endif        
    }  


    result_t commute(embot::hw::ADS122C04 s, Channel channel, const embot::core::Callback &oncompletion)
    {
        if(false == canacquire(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::core::tointegral(s);  
        
        // set channel
        volatile result_t res = result_t::NOK;

        // the following two operations are in blocking mode ... ahi! and take about 200 usec in total.
        // think of a non blocking mode.
        //#warning ADS122: think of a non-blocking acquisition start mode
        res = _ads_chip.setchannel(channel, 3*embot::core::time1millisec); 
        //embot::hw::sys::delay(50);
        // start the conversion        
        res = _ads_chip.startconversion(); 
        
        // now we return. however we call the callback
        oncompletion.execute();
            
        return resOK;                
    }
    
    embot::core::relTime conversiontime(embot::hw::ADS122C04 s)
    {
        if(false == initialised(s))
        {
            return 0;
        } 
        return _ads_chip.getConversionTime();
    }
    
      
    result_t acquire(embot::hw::ADS122C04 s, Channel channel, const embot::core::Callback &oncompletion)
    {
        if(false == canacquire(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::core::tointegral(s);  
                        
        s_privatedata.acquisition[index].clear();
        s_privatedata.acquisition[index].ongoing = true;
        s_privatedata.acquisition[index].done = false;
        s_privatedata.acquisition[index].channel = channel;
        s_privatedata.acquisition[index].userdefCBK = oncompletion;
        
        // ok, now i trigger the reading of the value.
        embot::core::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        embot::core::Data data = embot::core::Data(&s_privatedata.acquisition[index].rxdata[0], sizeof(s_privatedata.acquisition[index].rxdata));
        _ads_chip.retrievevalue(data, cbk);
               
        return resOK;
    }        
    
    result_t acquisition(ADS122C04 s, Channel channel, const embot::core::Callback &oncompletion)
    {
        if(false == canacquire(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::core::tointegral(s);  
        
        // set channel
        volatile result_t res = result_t::NOK;

        // the following two operations are in blocking mode ... ahi! and take about 200 usec in total.
        // think of a non blocking mode.
        //#warning ADS122: think of a non-blocking acquisition start mode
        res = _ads_chip.setchannel(channel, 3*embot::core::time1millisec); 
        //embot::hw::sys::delay(50);
        // start the conversion        
        res = _ads_chip.startconversion(); 
        // we must wait for the conversion to be done
        embot::hw::sys::delay(_ads_chip.getConversionTime()); 
                
        s_privatedata.acquisition[index].clear();
        s_privatedata.acquisition[index].ongoing = true;
        s_privatedata.acquisition[index].done = false;
        s_privatedata.acquisition[index].channel = channel;
        s_privatedata.acquisition[index].userdefCBK = oncompletion;

#if defined(ads122c04_FAKEmode)
        embot::core::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        cbk.execute();
#else         
        // ok, now i trigger the reading of the value.
        embot::core::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        embot::core::Data data = embot::core::Data(&s_privatedata.acquisition[index].rxdata[0], sizeof(s_privatedata.acquisition[index].rxdata));
        _ads_chip.retrievevalue(data, cbk);
#endif                
        return resOK;
    }
    
    bool isalive(ADS122C04 s, embot::core::relTime timeout)
    {
        if(false == initialised(s))
        {
            return false;
        } 
#if defined(ads122c04_FAKEmode)
        return true;
#else 
        std::uint8_t index = embot::core::tointegral(s);
        return embot::hw::i2c::ping(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, timeout);  
#endif
    }

    
    bool operationdone(ADS122C04 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        return s_privatedata.acquisition[embot::core::tointegral(s)].done;        
    } 
    
    
    result_t read(ADS122C04 s, Values &vals)
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
        vals.v1 = s_privatedata.acquisition[index].values.v1;
        vals.v2 = s_privatedata.acquisition[index].values.v2;
  
        return resOK;        
    }
    
 
}}} // namespace embot { namespace hw { namespace ads122c04 {



#endif //defined(EMBOT_ADS122C04_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

